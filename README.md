# 🤖 ABU Robocon 2027

Project đồ án tốt nghiệp và phát triển robot tham gia **ABU Robocon 2027**.

Hệ thống gồm hai robot:

* **TR – Transport Robot**
* **BR – Building Robot**

Kiến trúc điều khiển được chia thành hai tầng:

```text
┌──────────────────────────────────────────────┐
│              HIGH-LEVEL SYSTEM               │
│                                              │
│ ROS 2 / SLAM / Nav2 / Computer Vision / AI  │
│                Mini PC / PC                  │
└──────────────────────┬───────────────────────┘
                       │
                 UART / USB / CAN
                       │
                       ▼
┌──────────────────────────────────────────────┐
│               LOW-LEVEL SYSTEM               │
│                                              │
│                    STM32                     │
│                                              │
│  Encoder │ PID │ PWM │ Motor │ GPIO │ I/O   │
└──────────────────────┬───────────────────────┘
                       │
                       ▼
                 Motor Driver
                       │
                       ▼
                     Motors
```

---

# 1. 🤖 Robot System

## TR – Transport Robot

TR đảm nhiệm:

* Di chuyển trên sân.
* Thu thập vật liệu.
* Vận chuyển vật liệu.
* Đưa block đến khu vực Transfer.
* Vận chuyển Mustika.
* Điều khiển các cơ cấu gắp và giao vật.

Kiến trúc:

```text
Controller
     │
     ▼
   STM32
     │
 ┌───┼─────────────┐
 │   │             │
PID Encoder     Actuator
 │   │             │
 └───┼─────────────┘
     │
     ▼
Motor Driver
     │
     ▼
   Motors
```

STM32 là bộ điều khiển thời gian thực chính của TR.

---

# 2. 🏗️ BR – Building Robot

BR đảm nhiệm:

* Nhận block từ TR.
* Di chuyển giữa các tầng.
* Xây dựng Tower.
* Vận chuyển các vật thể theo luật thi đấu.
* Điều khiển cơ cấu gắp và đặt.
* Tự động định vị và di chuyển.

BR sử dụng **Mini PC + STM32**.

```text
                  ┌──────────────┐
                  │    Camera    │
                  └──────┬───────┘
                         │
                  ┌──────▼───────┐
                  │   Mini PC    │
                  │              │
                  │ ROS 2        │
                  │ SLAM         │
                  │ Nav2         │
                  │ OpenCV       │
                  │ YOLO / AI    │
                  └──────┬───────┘
                         │
                     UART / USB
                         │
                  ┌──────▼───────┐
                  │    STM32     │
                  │              │
                  │ PID          │
                  │ Encoder      │
                  │ Motor        │
                  │ GPIO         │
                  └──────┬───────┘
                         │
                    Motor Driver
                         │
                         ▼
                       Motors
```

---

# 3. 🧠 Kiến trúc điều khiển

Project sử dụng kiến trúc **High-Level / Low-Level Control**.

## High-Level

High-Level chịu trách nhiệm:

* Nhận biết môi trường.
* Xử lý camera.
* Point Cloud.
* LiDAR.
* SLAM.
* Localization.
* Navigation.
* Path Planning.
* AI / Object Detection.
* Quyết định robot cần đi đâu.
* Gửi command xuống STM32.

Các thành phần chính:

```text
ROS 2
SLAM
Nav2
RPLIDAR
Orbbec Astra Pro
OpenCV
YOLO / CNN
Mini PC
```

---

# 4. ⚡ Low-Level – STM32

STM32 chịu trách nhiệm toàn bộ phần điều khiển thời gian thực.

```text
                STM32
                  │
      ┌───────────┼───────────┐
      │           │           │
   Encoder       PID        GPIO
      │           │           │
      └───────────┼───────────┘
                  │
                 PWM
                  │
                  ▼
             Motor Driver
                  │
                  ▼
                Motor
```

STM32 xử lý:

* Encoder.
* RPM.
* PID tốc độ.
* PID vị trí.
* PWM.
* Direction.
* Motor control.
* Odometry.
* Limit switch.
* Sensor.
* Cơ cấu chấp hành.
* Giao tiếp UART/CAN/USB.
* Safety stop.

---

# 5. 🔄 Luồng điều khiển

Ví dụ BR:

```text
Camera + LiDAR
       │
       ▼
     Mini PC
       │
       ├── SLAM
       ├── Nav2
       ├── Vision
       └── Path Planning
       │
       ▼
 velocity command
       │
       ▼
     STM32
       │
       ├── Calculate target RPM
       ├── Read Encoder
       ├── PID
       └── Generate PWM
       │
       ▼
   Motor Driver
       │
       ▼
     Motors
       │
       ▼
    Encoder
       │
       └──────────────► STM32
```

Điều này giúp ROS 2 không cần trực tiếp tạo PWM cho motor.

---

# 6. 📡 Communication

Giao tiếp giữa High-Level và STM32 có thể sử dụng:

```text
Mini PC
   │
   │ UART / USB
   ▼
STM32
```

Dữ liệu từ Mini PC xuống STM32:

```text
CMD_VEL
TARGET_RPM
MOTOR_COMMAND
ACTUATOR_COMMAND
STOP
MODE
```

Dữ liệu STM32 gửi lên:

```text
ENCODER
RPM
ODOMETRY
MOTOR_STATUS
BATTERY_STATUS
ERROR
LIMIT_SWITCH
```

Ví dụ packet:

```text
<VEL,0.8,0.0,0.0>
```

hoặc:

```text
<BR,VEL,0.8,0.0,0.0>
```

STM32 nhận command → tính tốc độ từng motor → PID → PWM.

---

# 7. 🚗 Motor Control

Hệ thống sử dụng motor DC có encoder.

```text
Target RPM
    │
    ▼
   PID
    │
    ▼
   PWM
    │
    ▼
Motor Driver
    │
    ▼
 Motor
    │
    ▼
 Encoder
    │
    └──────────────► STM32
```

PID được sử dụng để đảm bảo:

```text
Target Speed ≈ Actual Speed
```

Ví dụ:

```text
Target RPM = 500

Encoder
   ↓
Actual RPM = 480
   ↓
Error = 20
   ↓
PID
   ↓
Increase PWM
```

---

# 8. 🛞 Robot Drive

Tùy robot, STM32 tính toán tốc độ cho từng bánh.

Ví dụ robot mecanum:

```text
             FRONT

       FL              FR
        ○              ○
         \            /
          \          /
           ROBOT
          /          \
         /            \
        ○              ○
       RL              RR

              BACK
```

Với:

```text
Vx = forward velocity
Vy = lateral velocity
Wz = angular velocity
```

STM32 chuyển đổi:

```text
Vx + Vy + Wz
       │
       ▼
Wheel Kinematics
       │
       ▼
FL FR RL RR RPM
       │
       ▼
PID
       │
       ▼
PWM
```

---

# 9. 📷 Computer Vision

BR sử dụng:

```text
Orbbec Astra Pro
       │
       ├── RGB
       ├── Depth
       └── Point Cloud
              │
              ▼
            Mini PC
              │
       ┌──────┴───────┐
       │              │
     OpenCV          YOLO
       │              │
       └──────┬───────┘
              ▼
       Object Detection
```

Các đối tượng cần nhận biết có thể bao gồm:

* Block.
* Stair.
* Tower.
* Khu vực đặt vật.
* Vật thể trên sân.

---

# 10. 📡 LiDAR

BR sử dụng:

```text
RPLIDAR A1M8
      │
      ▼
   Mini PC
      │
      ▼
   ROS 2
      │
      ▼
    /scan
      │
      ▼
     SLAM
```

LiDAR chủ yếu phục vụ:

* Mapping.
* Localization.
* Obstacle detection.
* Navigation.

---

# 11. 🗺️ SLAM

BR sử dụng ROS 2 để xây dựng bản đồ.

```text
RPLIDAR
   │
   ▼
 /scan
   │
   ▼
 SLAM
   │
   ├── Map
   └── Pose
        │
        ▼
       Nav2
```

Kết hợp:

```text
LiDAR
  +
Odometry
  +
TF
  ↓
SLAM
  ↓
Map + Robot Pose
```

---

# 12. 🧭 Navigation

Navigation của BR:

```text
        Map
         │
         ▼
       Nav2
         │
   ┌─────┴─────┐
   │           │
Planner     Controller
   │           │
   └─────┬─────┘
         ▼
      cmd_vel
         │
         ▼
       STM32
         │
         ▼
       Motors
```

Nav2 quyết định robot cần di chuyển như thế nào.

STM32 quyết định motor phải quay như thế nào.

---

# 13. 🔧 ROS 2 Architecture

BR:

```text
                    ROS 2
                      │
       ┌──────────────┼──────────────┐
       │              │              │
    Camera          LiDAR          Nav2
       │              │              │
       ▼              ▼              │
   PointCloud       /scan            │
       │              │              │
       └───────┬──────┘              │
               ▼                     │
              SLAM                   │
               │                     │
               ▼                     │
              TF                     │
               │                     │
               └──────────┬──────────┘
                          ▼
                       cmd_vel
                          │
                          ▼
                        STM32
```

---

# 14. 📁 Repository Structure

Đề xuất cấu trúc:

```text
Robocon2027/
│
├── README.md
│
├── TR/
│   ├── README.md
│   ├── STM32/
│   │   ├── Core/
│   │   ├── Drivers/
│   │   ├── Motor/
│   │   ├── Encoder/
│   │   ├── PID/
│   │   └── Communication/
│   │
│   ├── CAD/
│   └── Documentation/
│
├── BR/
│   ├── README.md
│   │
│   ├── STM32/
│   │   ├── Core/
│   │   ├── Drivers/
│   │   ├── Motor/
│   │   ├── Encoder/
│   │   ├── PID/
│   │   └── Communication/
│   │
│   ├── ROS2/
│   │   ├── src/
│   │   ├── launch/
│   │   ├── config/
│   │   ├── worlds/
│   │   └── models/
│   │
│   ├── Vision/
│   ├── SLAM/
│   ├── Nav2/
│   └── CAD/
│
├── simulation/
│   └── Gazebo/
│
├── docs/
│
└── scripts/
```

---

# 15. 🧪 Simulation

Trước khi chạy robot thật, hệ thống được kiểm thử trên Gazebo.

```text
Gazebo
   │
   ├── Robot Model
   ├── Motors
   ├── Wheels
   ├── Sensors
   ├── LiDAR
   └── Camera
        │
        ▼
      ROS 2
        │
        ├── SLAM
        └── Nav2
```

Simulation giúp kiểm tra:

* Kinematics.
* TF.
* Odometry.
* SLAM.
* Navigation.
* Sensor.
* Robot model.
* Controller.

STM32 thật sẽ được tích hợp sau khi logic điều khiển được kiểm chứng.

---

# 16. 🔌 Hardware Architecture

## TR

```text
Battery
  │
  ├──────────────► Motor Driver
  │                    │
  │                    ▼
  │                  Motors
  │                    │
  │                 Encoder
  │                    │
  │                    ▼
  │                  STM32
  │
  └──────────────► DC/DC
                       │
                       ▼
                     STM32
```

## BR

```text
Battery
  │
  ├──────────────► Motor Driver
  │                    │
  │                    ▼
  │                  Motors
  │                    │
  │                 Encoder
  │                    │
  │                    ▼
  │                  STM32
  │
  └──────────────► DC/DC
                       │
                       ├── STM32
                       └── Mini PC

Camera ───────────────► Mini PC
RPLIDAR ──────────────► Mini PC

Mini PC ◄─────────────► STM32
```

---

# 17. 🛡️ Safety

STM32 phải có cơ chế dừng độc lập.

```text
STOP command
     │
     ▼
   STM32
     │
     ├── PWM = 0
     ├── Motor OFF
     └── Actuator SAFE
```

Các điều kiện dừng:

* Emergency stop.
* Mất communication.
* Encoder lỗi.
* Motor lỗi.
* Điện áp thấp.
* Quá dòng.
* Timeout command.

Ví dụ:

```text
if communication_timeout:
    motor_pwm = 0
```

---

# 18. 🔋 Power System

Nguồn được chia thành:

```text
Battery
   │
   ├── Motor Power
   │
   └── DC/DC
          │
          ├── STM32
          ├── Sensors
          └── Mini PC
```

Không cấp nguồn motor trực tiếp từ STM32.

STM32 chỉ điều khiển:

```text
PWM
DIR
ENABLE
```

Motor Driver chịu trách nhiệm công suất.

---

# 19. 💻 Development Environment

## STM32

Có thể sử dụng:

* STM32CubeIDE
* STM32CubeMX
* STM32 HAL
* ARM GCC
* ST-LINK

## ROS 2

BR sử dụng:

```text
Ubuntu
ROS 2
Gazebo
SLAM
Nav2
OpenCV
YOLO
```

## Version Control

```text
Git
GitHub
```

Repository:

```text
Robocon2027
```

---

# 20. 🔄 Development Workflow

Quy trình phát triển:

```text
Mechanical Design
       ↓
Motor Test
       ↓
Encoder Test
       ↓
STM32 PWM
       ↓
PID
       ↓
Communication
       ↓
Odometry
       ↓
ROS 2
       ↓
SLAM
       ↓
Nav2
       ↓
Vision
       ↓
Integration
       ↓
Robot Test
```

---

# 21. 🧪 Testing Levels

## Level 1 – Motor

```text
STM32
 ↓
PWM
 ↓
Motor
```

## Level 2 – Encoder

```text
Motor
 ↓
Encoder
 ↓
STM32
 ↓
RPM
```

## Level 3 – PID

```text
Target RPM
 ↓
PID
 ↓
PWM
 ↓
Motor
 ↓
Encoder
 ↓
Feedback
```

## Level 4 – Communication

```text
PC
 ↓
UART
 ↓
STM32
 ↓
Motor
```

## Level 5 – ROS 2

```text
ROS 2
 ↓
cmd_vel
 ↓
STM32
 ↓
Motor
```

## Level 6 – Full Robot

```text
Camera
   +
LiDAR
   +
ROS 2
   +
SLAM
   +
Nav2
   +
STM32
   +
Motor
```

---

# 22. 🎯 Main Objective

Mục tiêu cuối cùng của hệ thống:

```text
                 ABU ROBOCON 2027
                         │
             ┌───────────┴───────────┐
             │                       │
             ▼                       ▼
            TR                      BR
             │                       │
          STM32              Mini PC + STM32
             │                       │
        Motor Control        ROS 2 + AI + SLAM
             │                       │
             └───────────┬───────────┘
                         │
                         ▼
                  AUTONOMOUS ROBOT
```

Kiến trúc được thiết kế theo nguyên tắc:

> **Mini PC/ROS 2 quyết định robot phải làm gì, STM32 quyết định motor phải làm như thế nào.**

STM32 là tầng **real-time control**, còn ROS 2/Mini PC là tầng **high-level intelligence**.

---

# 23. 📌 Project Status

### TR

* [x] Thiết kế kiến trúc STM32
* [ ] Motor control
* [ ] Encoder
* [ ] PID
* [ ] Communication
* [ ] Odometry
* [ ] Cơ cấu chấp hành
* [ ] Full robot integration

### BR

* [x] Mini PC architecture
* [x] Camera
* [x] RPLIDAR
* [x] ROS 2
* [ ] STM32 motor control
* [ ] Encoder
* [ ] PID
* [ ] UART/USB communication
* [ ] Odometry
* [ ] SLAM
* [ ] Nav2
* [ ] Vision
* [ ] YOLO
* [ ] Full robot integration

### Simulation

* [x] Gazebo
* [x] Robot model
* [ ] Complete sensor simulation
* [ ] SLAM
* [ ] Nav2
* [ ] Full autonomous test

---

# 24. 🚀 Final Architecture

```text
                         ROBOT SYSTEM
                              │
             ┌────────────────┴────────────────┐
             │                                 │
             ▼                                 ▼
             TR                                BR
             │                                 │
        Controller                          Mini PC
             │                                 │
             ▼                           ┌─────┼─────┐
          STM32                           │     │     │
             │                          Camera LiDAR ROS2
             │                           │     │     │
             │                           └──┬──┴──┬──┘
             │                              │     │
             │                            SLAM  Nav2
             │                              │     │
             │                              └──┬──┘
             │                                 │
             │                            cmd_vel
             │                                 │
             │                                 ▼
             │                              STM32
             │                                 │
             └──────────────┐          ┌───────┘
                            ▼          ▼
                         PID + Encoder
                              │
                              ▼
                        Motor Drivers
                              │
                              ▼
                            Motors
```

## Core Principle

```text
HIGH LEVEL
───────────
ROS 2
SLAM
Nav2
Vision
AI
Planning

        ↓ command

LOW LEVEL
──────────
STM32
Encoder
PID
PWM
Motor
Actuator

        ↓

PHYSICAL ROBOT
```

**STM32 không thay thế ROS 2/Mini PC và ROS 2/Mini PC cũng không thay thế STM32. Hai tầng phối hợp với nhau để tạo thành hệ thống robot hoàn chỉnh.**
