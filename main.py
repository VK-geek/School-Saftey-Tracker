import uvicorn
import json
from fastapi import FastAPI, Depends, HTTPException, Body, WebSocket, WebSocketDisconnect
from fastapi.middleware.cors import CORSMiddleware
from sqlalchemy.orm import Session
from typing import Dict, List

import models, schemas
from database import SessionLocal, engine, Base

Base.metadata.create_all(bind=engine)
app = FastAPI(title="Child Safety System API")
live_student_data: Dict[str, Dict] = {}

class ConnectionManager:
    def __init__(self):
        self.active_connections: List[WebSocket] = []
    async def connect(self, websocket: WebSocket):
        await websocket.accept()
        self.active_connections.append(websocket)
    def disconnect(self, websocket: WebSocket):
        self.active_connections.remove(websocket)
    async def broadcast_data(self, data: dict):
        message = json.dumps(data)
        for connection in self.active_connections:
            await connection.send_text(message)

manager = ConnectionManager()

def get_db():
    db = SessionLocal()
    try:
        yield db
    finally:
        db.close()

@app.on_event("startup")
def startup_event():
    db = SessionLocal()
    default_beacons = { "CLASSROOM-101": models.BeaconType.classroom, "DANGER-101": models.BeaconType.danger }
    for name, type in default_beacons.items():
        if not db.query(models.Beacon).filter(models.Beacon.name == name).first():
            db.add(models.Beacon(name=name, type=type))
            db.commit()
    db.close()

app.add_middleware(
    CORSMiddleware,
    allow_origins=["*"], allow_credentials=True,
    allow_methods=["*"], allow_headers=["*"],
)

@app.post("/update")
async def update_live_data(payload: str = Body(...)):
    try:
        parts = payload.split(',')
        if len(parts) < 7:
             raise ValueError("Payload must have 7 parts")
        data = {
            "childId": parts[0],
            "closestClassroom": parts[1],
            "classroomDist": float(parts[2]),
            "closestDanger": parts[3],
            "dangerDist": float(parts[4]), 
            "alert": bool(int(parts[5])),
            "wifiRssi": int(parts[6])
        }
        live_student_data[data["childId"]] = data
        await manager.broadcast_data(live_student_data)
        return {"status": "success"}
    except Exception as e:
        raise HTTPException(status_code=400, detail=f"Invalid payload: {e}")

@app.websocket("/ws")
async def websocket_endpoint(websocket: WebSocket):
    await manager.connect(websocket)
    await manager.broadcast_data(live_student_data)
    try:
        while True:
            await websocket.receive_text()
    except WebSocketDisconnect:
        manager.disconnect(websocket)

# --- All management endpoints are unchanged ---
@app.get("/beacons/", response_model=List[schemas.Beacon])
def read_beacons(db: Session = Depends(get_db)): return db.query(models.Beacon).all()
@app.post("/beacons/", response_model=schemas.Beacon)
def create_beacon(beacon: schemas.BeaconCreate, db: Session = Depends(get_db)):
    db_beacon = models.Beacon(**beacon.dict()); db.add(db_beacon); db.commit(); db.refresh(db_beacon); return db_beacon
@app.delete("/beacons/{beacon_id}", status_code=204)
def delete_beacon(beacon_id: int, db: Session = Depends(get_db)):
    db_beacon = db.query(models.Beacon).filter(models.Beacon.id == beacon_id).first()
    if not db_beacon: raise HTTPException(status_code=404, detail="Beacon not found")
    if db_beacon.name in ["CLASSROOM-101", "DANGER-101"]: raise HTTPException(status_code=403, detail="Cannot delete default beacon.")
    db.delete(db_beacon); db.commit(); return
@app.get("/students/", response_model=List[schemas.Student])
def read_students(db: Session = Depends(get_db)): return db.query(models.Student).all()
@app.post("/students/", response_model=schemas.Student)
def create_student(student: schemas.StudentCreate, db: Session = Depends(get_db)):
    db_student = models.Student(**student.dict()); db.add(db_student); db.commit(); db.refresh(db_student); return db_student
@app.delete("/students/{student_id}", status_code=204)
def delete_student(student_id: int, db: Session = Depends(get_db)):
    db_student = db.query(models.Student).filter(models.Student.id == student_id).first()
    if not db_student: raise HTTPException(status_code=404, detail="Student not found")
    db.delete(db_student); db.commit(); return
@app.get("/config")
def get_device_config(db: Session = Depends(get_db)):
    beacons = db.query(models.Beacon).all()
    return {"beacons": [{"name": b.name, "type": b.type.value} for b in beacons]}

