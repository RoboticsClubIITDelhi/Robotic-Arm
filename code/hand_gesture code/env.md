## Complete Setup Guide

### Step 1 — Install Python 3.10

Download from: **https://www.python.org/downloads/release/python-31011/**

- Pick **Windows installer (64-bit)**
- ✅ Check **"Add to PATH"** during install

---

### Step 2 — Open Terminal in Project Folder

```bash
cd "C:\Users\moham\IIT\Projects\Robotic-Arm-main\code\hand_gesture code"
```

---

### Step 3 — Create Virtual Environment

```bash
py -3.13 -m venv venv
```

---

### Step 4 — Activate It

```bash
venv\Scripts\activate
```

You'll see `(venv)` appear in your terminal.

---

### Step 5 — Install Packages

```bash
pip install mediapipe==0.10.30 opencv-python numpy requests protobuf==3.20.3
```

---

### Step 6 — Upload Arduino Code

- Replace `CONNECT.h` with the fixed version
- Upload to ESP32 via Arduino IDE

---

### Step 7 — Connect to ESP32 WiFi

- WiFi name: **`ESP32_DEV`**
- Password: **`12345678`**

---

### Step 8 — Run

```bash
python hand_gesture.py
```

---

## Every Time You Reopen Terminal

```bash
cd "C:\Users\moham\IIT\Projects\Robotic-Arm-main\code\hand_gesture code"
venv\Scripts\activate
python hand_gesture.py
```
