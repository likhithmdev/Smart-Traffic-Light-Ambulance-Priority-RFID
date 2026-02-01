# Smart-Traffic-Light-Ambulance-Priority-RFID
Smart Traffic Light System with Ambulance Priority using RFID. An Arduino-based 4-way traffic control system that detects authorized ambulances via RFID, overrides normal signal timing to provide priority passage, and safely resumes standard traffic flow after exit. Built for smart city applications
# Smart Traffic Light System with Ambulance Priority using RFID 🚑🚦

An Arduino-based intelligent traffic control system that automatically provides priority to ambulances at traffic junctions using RFID technology.

## 📌 Problem Statement
Emergency vehicles often lose critical time at traffic signals. Manual intervention is unreliable in high-traffic conditions.

## 💡 Solution
This system detects authorized ambulances using RFID and dynamically switches the corresponding traffic signal to GREEN, allowing uninterrupted passage. Once the ambulance exits, normal traffic flow resumes.

## ⚙️ System Overview
- Entry RFID reader detects approaching ambulance
- Arduino validates RFID UID
- Selected signal turns GREEN
- Exit RFID reader detects ambulance exit
- Normal traffic cycle resumes

## 🧰 Technologies Used
- Arduino
- RFID (MFRC522)
- SPI Communication
- Embedded C/C++

## 🚀 Key Features
- Ambulance priority control
- Secure UID verification
- Entry & exit based logic
- Real-time traffic signal switching
- Scalable smart city design

