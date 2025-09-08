from pydantic import BaseModel
from models import BeaconType # Note the relative import

# --- Beacon Schemas (for API data validation) ---
class BeaconBase(BaseModel):
    name: str
    type: BeaconType

class BeaconCreate(BeaconBase):
    pass

class Beacon(BeaconBase):
    id: int

    class Config:
        from_attributes = True

# --- Student Schemas (for API data validation) ---
class StudentBase(BaseModel):
    name: str

class StudentCreate(StudentBase):
    pass

class Student(StudentBase):
    id: int

    class Config:
        from_attributes = True
