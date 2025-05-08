# Robo Arm zen - Robotics Summer project 2025-26
This repository is intended to manage the codebase for the **Robotics Arm summer project 2025-26** (part of robotics club). Ideally, it can be extended to include other related resources as well (CAD model, calculation(Matlab), Datasheet of the electronic components etc.) Over time, it will serve as both a proof of work and a **well-documented reference** to help others continue the project beyond its initial requirements

## Adjenda
(In continuation with the alredy existing hardware)
- replace all motors with [SC15](https://www.waveshare.com/wiki/SC15_Servo)
- use the waveshare SC15 servo drive with esp32 more info [here](https://www.waveshare.com/wiki/Servo_Driver_with_ESP32#SC_Servo)
- add a battery powersource (6-12v)
- Redesign the Gripper (will be good if you can upload the final CAD models here as well)
- design the SC15 motor mounts
- Coding part
  - solve the inverse kinematic, (or have atleast a rough estimate via trial and error)
  - make the controlls in cylindrical coordinates `[r, h, φ]`.
  - design a UI(web) to controll to wirelessly, (as seen in driver demo)


## Progress 
Need to break each tasks for ease and also distrubute the task day wise.
For now this is just an overview.
### 🔧 Hardware Upgrades
- [ ] Design motor mounts for SC15 servos
- [ ] Redesign and upload CAD files for the new gripper
- [ ] Replace all motors with SC15 servos
- [ ] Integrate SC15 servo driver with ESP32 
- [ ] Add 6–12V battery power source


### 🧠 Software Tasks
- [ ] Implement inverse kinematics (approximate solution acceptable)
- [ ] Develop cylindrical coordinate-based controls `[r, h, φ]`
- [ ] Build a basic web UI for wireless control (refer to Waveshare demo)

## resources
- https://www.waveshare.com/wiki/SC15_Servo
- https://www.waveshare.com/wiki/Servo_Driver_with_ESP32#SC_Servo

## 🤝 Contributor Notes
- will be following standard git workflow [here](https://www.geeksforgeeks.org/git-workflows-with-open-source-collaboration/)
- Feel free to suggest improvements via Issues or Discussions

