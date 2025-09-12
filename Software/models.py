from sqlalchemy import Column, Integer, String, Enum
from database import Base
import enum

class BeaconType(str, enum.Enum):
    classroom = "classroom"
    danger = "danger"

class Beacon(Base):
    __tablename__ = "beacons"

    id = Column(Integer, primary_key=True, index=True)
    name = Column(String, unique=True, index=True, nullable=False)
    type = Column(Enum(BeaconType))

class Student(Base):
    __tablename__ = "students"

    id = Column(Integer, primary_key=True, index=True)
    name = Column(String, unique=True, index=True, nullable=False)
