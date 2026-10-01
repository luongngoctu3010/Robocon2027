# 🤖 ABU Robocon 2027 – TR & BR

> **Graduation Project – Robocon 2027**
> **Two-Robot System: TR + BR**
> ROS 2 · Gazebo · STM32 · Mini PC · Encoder Motor · PID Control

---

## 📌 Giới thiệu

Repository này chứa toàn bộ mã nguồn, mô phỏng, thiết kế điều khiển và tài liệu phát triển cho **đồ án tốt nghiệp ABU Robocon 2027**.

Hệ thống gồm **2 robot phối hợp**:

* **TR – Transport Robot:** robot vận chuyển vật liệu, lấy và giao các vật thể cho khu vực xây dựng.
* **BR – Build Robot:** robot nhận vật liệu từ TR, di chuyển trên khu vực thi đấu và thực hiện nhiệm vụ xây dựng.

Mục tiêu của dự án là xây dựng một hệ thống robot hoàn chỉnh từ:

**Thiết kế cơ khí → Điện → Điều khiển động cơ → Encoder/PID → ROS 2 → Gazebo → Navigation → Prototype → Test thực tế.**

---

# 🏗️ 1. Kiến trúc hệ thống

```text
                    ABU ROBOCON 2027
                           │
             ┌─────────────┴─────────────┐
             │                           │
            TR                          BR
     Transport Robot              Build Robot
             │                           │
      STM32 Nucleo               Mini PC + STM32
             │                           │
       Motor Control             High-Level Control
             │                           │
       Encoder + PID             ROS 2 / Navigation
             │                           │
      Motor Drivers              Motor Drivers
             │                           │
          Motors                     Motors
```

## TR – Transport Robot

TR tập trung vào:

* Di chuyển tốc độ cao.
* Điều khiển động cơ chính xác bằng encoder.
* PID tốc độ/vị trí.
* Cơ cấu gắp đồng thời các vật thể.
* Vận chuyển vật liệu từ khu vực lấy vật đến khu vực Transfer.
* Thực hiện nhiệm vụ Mustika/Sanctuary theo chiến thuật của đội.

### Bộ điều khiển dự kiến

```text
STM32 Nucleo
     │
     ├── Motor Driver
     │      ├── Motor 1
     │      ├── Motor 2
     │      ├── Motor 3
     │      └── Motor 4
     │
     ├── Encoder A/B
     │
     ├── Gripper / Actuator
     │
     └── Safety / STOP
```

---

# 🏗️ BR – Build Robot

BR đảm nhiệm:

* Nhận vật liệu từ TR.
* Di chuyển giữa các tầng/khu vực.
* Vận chuyển vật liệu lên khu vực xây dựng.
* Xây dựng Tower.
* Thực hiện nhiệm vụ Mustika.
* Điều khiển cơ cấu gắp/thả.
* Có khả năng leo bậc/dốc theo thiết kế cơ khí.

### Bộ điều khiển

BR sử dụng kiến trúc:

```text
                    Mini PC
                       │
                ROS 2 / Navigation
                       │
              ┌────────┴────────┐
              │                 │
           Camera            STM32
                                │
                       ┌────────┴────────┐
                       │                 │
                  Motor Driver      Actuators
                       │
                    Motors
                       │
                    Encoder
```

### Phân chia nhiệm vụ

| Thiết bị     | Nhiệm vụ                                       |
| ------------ | ---------------------------------------------- |
| Mini PC      | ROS 2, Navigation, xử lý camera, logic cấp cao |
| STM32 Nucleo | Điều khiển động cơ thời gian thực              |
| Encoder      | Phản hồi tốc độ/vị trí                         |
| Motor Driver | Điều khiển công suất động cơ                   |
| Camera       | Nhận biết môi trường/vật thể khi cần           |
| ROS 2        | Giao tiếp giữa các node                        |
| Gazebo       | Mô phỏng robot và môi trường                   |

---

# ⚙️ 2. Mục tiêu kỹ thuật

## TR

Các mục tiêu thiết kế hiện tại:

| Thông số            |                          Mục tiêu |
| ------------------- | --------------------------------: |
| Robot               |                                TR |
| Điều khiển          |                      STM32 Nucleo |
| Động cơ             |                DC motor + encoder |
| Điện áp hệ thống    | khoảng 24 V / theo cấu hình motor |
| Bánh xe             |                    khoảng Ø120 mm |
| Điều khiển          |                               PID |
| Phản hồi            |                       Encoder A/B |
| Cơ cấu gắp          |                           2E + 1S |
| Mục tiêu tốc độ     |                    khoảng 1,3 m/s |
| Khối lượng thiết kế |                      khoảng 45 kg |
| Chế độ              |        Manual + Autonomous hỗ trợ |

> Các thông số trên là mục tiêu thiết kế và sẽ được xác nhận lại bằng prototype thực tế.

---

## BR

| Thông số                     |           Mục tiêu |
| ---------------------------- | -----------------: |
| Robot                        |                 BR |
| Bộ điều khiển cấp cao        |            Mini PC |
| Bộ điều khiển thời gian thực |       STM32 Nucleo |
| Động cơ                      | DC motor + encoder |
| Điều khiển                   |                PID |
| Bánh xe                      |      Ø120 mm class |
| Cơ cấu leo                   |    đang phát triển |
| Cơ cấu gắp                   |                 Có |
| Điều hướng                   | ROS 2 / Navigation |
| Camera                       |      đang lựa chọn |
| Khối lượng thiết kế          |    khoảng 40–45 kg |

---

# 🔌 3. Hệ thống điện

Kiến trúc điện dự kiến:

```text
                 BATTERY
                    │
              Main Fuse
                    │
             Emergency STOP
                    │
          ┌─────────┴─────────┐
          │                   │
     Motor Power          DC/DC Converter
          │                   │
     Motor Driver        Logic Power
          │                   │
        Motors           STM32 / Mini PC
          │
       Encoder
          │
        STM32
```

## Nguyên tắc

* Tách nguồn động lực và nguồn logic khi cần.
* Có cầu chì bảo vệ.
* Có Emergency STOP.
* Không cấp trực tiếp nguồn động cơ vào MCU.
* Motor Driver phải phù hợp với điện áp và dòng thực tế của motor.
* Encoder được đọc trực tiếp bằng MCU.
* Tất cả GND tín hiệu phải được thiết kế đúng topology.

---

# 🧠 4. Điều khiển động cơ

Mỗi motor có encoder:

```text
             STM32
               │
        PWM / DIR / EN
               │
          Motor Driver
               │
             Motor
               │
          Encoder A/B
               │
               └──────────> STM32
```

## PID tốc độ

Nguyên lý:

```text
Target RPM
    │
    ▼
  [PID]
    │
    ▼
   PWM
    │
Motor Driver
    │
  Motor
    │
Encoder
    │
    └────────── Feedback
```

PID sẽ được hiệu chỉnh bằng thực nghiệm.

Các bước:

1. Kiểm tra chiều quay.
2. Đọc encoder.
3. Đo RPM.
4. Chạy P.
5. Thêm I.
6. Thêm D nếu cần.
7. Kiểm tra tải.
8. Kiểm tra tăng/giảm tốc.
9. Kiểm tra đồng bộ các bánh.
10. Kiểm tra khi robot mang tải.

---

# 🛞 5. Hệ truyền động TR

TR hướng tới hệ truyền động 4 bánh.

```text
       FRONT
 ┌─────────────────┐
 │  M1         M2   │
 │                  │
 │                  │
 │  M3         M4   │
 └─────────────────┘
       REAR
```

Nếu sử dụng mecanum:

```text
             FRONT

        ↗ W1       W2 ↖


        ↙ W3       W4 ↘

             REAR
```

Việc lựa chọn:

* mecanum
* bánh thường
* tỷ số truyền
* đường kính bánh
* tốc độ motor
* tải trọng

sẽ được xác nhận bằng prototype.

---

# 🦾 6. Cơ cấu gắp TR

TR cần lấy:

```text
        E
        E
        │
        │
        S ───────>
```

Cấu hình mục tiêu:

* **2E xếp chồng theo phương đứng**
* **1S nằm bên cạnh**
* Gắp đồng thời trong một chu kỳ
* Cơ cấu kẹp đảm bảo không làm rơi vật thể khi tăng tốc/phanh.

Nguyên tắc thiết kế:

```text
             E
             E
             │
      ┌──────┴──────┐
      │   GRIPPER   │
      └──────┬──────┘
             │
       ┌─────┴─────┐
       │     S     │
       └───────────┘
```

Cơ cấu có thể sử dụng:

* Motor DC
* Servo
* Thanh răng – bánh răng
* Cơ cấu linkage
* Cơ cấu kẹp song song

---

# 🏗️ 7. Cơ cấu leo bậc BR

BR là phần cơ khí có yêu cầu cao nhất.

Thiết kế đang nghiên cứu sử dụng:

* 6 bánh.
* 4 bánh chính.
* 2 bánh phụ.
* Cơ cấu nâng/hạ.
* Rack & pinion.
* Dịch chuyển trọng tâm nếu cần.

Mô hình nguyên lý:

```text
             ROBOT
     ┌───────────────────┐
     │       BATTERY     │
     │         ↓         │
     │        CG         │
     └───────────────────┘
       O               O
          O         O

             ┌───────┐
             │ STEP  │
        ┌────┘       │
        │  STEP      │
    ┌───┘            │
```

Các vấn đề phải kiểm chứng:

* Lực kéo.
* Mô-men động cơ.
* Ma sát bánh/bề mặt.
* Trượt bánh.
* Phản lực tại mép bậc.
* Mô-men lật.
* Vị trí trọng tâm.
* Gia tốc khi leo.
* Tải động.
* Độ cứng khung.
* Độ bền cơ cấu rack & pinion.

---

# 📐 8. Tính toán cơ khí

Các mô hình tính toán chính:

### Lực kéo

```text
Ftraction = μN
```

### Lực tăng tốc

```text
F = ma
```

### Mô-men bánh

```text
T = F × r
```

### Công suất

```text
P = Tω
```

### Mô-men quay

```text
ΣM = Iα
```

### Điều kiện chống lật

Kiểm tra vị trí hình chiếu của trọng tâm so với vùng tiếp xúc bánh xe.

Các tính toán cuối cùng phải được kiểm chứng bằng thử nghiệm thực tế.

---

# 🤖 9. ROS 2

Repository sử dụng ROS 2 cho phần mô phỏng và điều khiển cấp cao.

Cấu trúc dự kiến:

```text
robocon2027_ws/
│
├── src/
│   └── robocon2027_description/
│       ├── config/
│       ├── launch/
│       ├── meshes/
│       ├── urdf/
│       ├── worlds/
│       └── ...
│
├── scripts/
│
├── build/
├── install/
└── log/
```

---

# 🌎 10. Gazebo Simulation

Mục tiêu mô phỏng:

* Robot model.
* Bánh xe.
* Motor.
* Encoder.
* Camera.
* LiDAR nếu sử dụng.
* Field.
* Obstacles.
* Transfer zone.
* Tower.
* Stair/Platform.
* Navigation.

Quy trình:

```text
CAD / URDF
    ↓
Gazebo
    ↓
Sensor Simulation
    ↓
ROS 2
    ↓
Navigation
    ↓
Control
    ↓
Real Robot
```

---

# 🗺️ 11. SLAM & Navigation

BR có thể sử dụng:

```text
Camera / LiDAR
       │
       ▼
     ROS 2
       │
       ▼
     SLAM
       │
       ▼
      Map
       │
       ▼
     Nav2
       │
       ▼
   Path Planning
       │
       ▼
     STM32
       │
       ▼
     Motors
```

SLAM và Navigation chỉ được sử dụng khi phù hợp với điều kiện thi đấu và yêu cầu thời gian thực.

---

# 📷 12. Camera

Camera là một trong các hạng mục đang được hoàn thiện.

Các yêu cầu:

* Hoạt động ổn định với Mini PC.
* Độ trễ thấp.
* Có thể xử lý hình ảnh bằng OpenCV.
* Có thể nhận biết vật thể/điểm mốc.
* Có thể tích hợp ROS 2.

Pipeline dự kiến:

```text
Camera
   ↓
Image
   ↓
OpenCV
   ↓
Object / Marker Detection
   ↓
ROS 2 Topic
   ↓
Navigation / Control
```

---

# 🎮 13. Điều khiển Manual

Hệ thống hỗ trợ phát triển chế độ điều khiển thủ công để:

* Test motor.
* Test encoder.
* Test PID.
* Test mecanum.
* Test cơ cấu gắp.
* Test leo bậc.
* Debug trước khi chạy autonomous.

Thiết bị điều khiển có thể được lựa chọn trong quá trình phát triển.

---

# 🔄 14. Giao tiếp Mini PC ↔ STM32

Kiến trúc:

```text
Mini PC
   │
   │ USB / UART / CAN
   │
   ▼
STM32 Nucleo
   │
   ├── Motor Driver
   ├── Encoder
   ├── Actuator
   └── Safety
```

Mini PC gửi các lệnh cấp cao:

```text
velocity
direction
mode
target
actuator command
```

STM32 chịu trách nhiệm:

```text
PWM
Encoder
PID
Motor protection
Emergency stop
Real-time control
```

---

# 🛡️ 15. Safety

Hệ thống phải có:

* Emergency STOP.
* Software STOP.
* Motor enable/disable.
* Giới hạn tốc độ.
* Giới hạn dòng nếu driver hỗ trợ.
* Watchdog.
* Kiểm tra encoder.
* Fail-safe khi mất communication.

Nguyên tắc:

```text
Communication Lost
       │
       ▼
    STM32
       │
       ▼
Motor STOP
```

---

# 🧪 16. Quy trình kiểm thử

## Level 1 – Motor

* Kiểm tra chiều quay.
* Kiểm tra dòng không tải.
* Kiểm tra encoder.
* Kiểm tra RPM.

## Level 2 – Motor + Driver

* PWM.
* Direction.
* Brake.
* PID.

## Level 3 – Chassis

* Robot chạy thẳng.
* Quay.
* Phanh.
* Tăng tốc.

## Level 4 – Payload

* Chạy khi mang tải.
* Gắp.
* Nâng.
* Đặt.

## Level 5 – Field

* Chạy trên sân.
* Kiểm tra sai số.
* Kiểm tra vật cản.
* Kiểm tra đường đi.

## Level 6 – Full Mission

```text
START
  ↓
TR lấy vật
  ↓
TR vận chuyển
  ↓
BR nhận vật
  ↓
BR xây dựng
  ↓
TR/BR phối hợp
  ↓
SANCTUARY / FINAL TASK
```

---

# ⏱️ 17. Chiến thuật TR + BR

Mục tiêu là tối ưu nhiệm vụ trong giới hạn **180 giây**.

## TR – Trip 1

```text
Start
 ↓
S1
 ↓
Grab S1
 ↓
Start
 ↓
E
 ↓
Grab 2E
 ↓
Slope
 ↓
Transfer
 ↓
BR nhận vật liệu
```

## TR – Trip 2

```text
Start
 ↓
S2
 ↓
Grab S2
 ↓
Start
 ↓
E
 ↓
Grab 2E
 ↓
Slope
 ↓
Transfer
 ↓
BR tiếp tục xây dựng
```

## TR – Mustika

TR không cần quay lại Start sau Trip 2.

```text
Transfer
 ↓
Down Slope
 ↓
Mustika
 ↓
Wait Sanctuary
 ↓
Grab Mustika
 ↓
Transfer
 ↓
Deliver Mustika
```

---

# 🏯 18. BR Tower Sequence

BR phải phối hợp với thời điểm TR giao vật liệu.

### Tower 1

```text
Receive material
       ↓
     Build
       ↓
    Tower 1
```

### Tower 2

Chuỗi dự kiến:

```text
L1
 ↓
Receive 2E
 ↓
L2
 ↓
Place
 ↓
L1
 ↓
Take 1S
 ↓
L2
 ↓
Place
 ↓
L1
 ↓
Take Mustika
 ↓
L2
 ↓
Place
```

Thời gian chuyển tầng sẽ được tối ưu bằng thử nghiệm thực tế.

---

# 📊 19. Quản lý thời gian

Mọi nhiệm vụ phải được phân tích theo:

* Thời gian di chuyển.
* Thời gian tăng/giảm tốc.
* Thời gian gắp.
* Thời gian đặt.
* Thời gian leo dốc/bậc.
* Thời gian chờ robot còn lại.
* Thời gian xử lý lỗi.

Mục tiêu là tạo một timeline:

```text
0 s
│
├── TR
│
├──── BR
│
├──────── TR
│
├──────────── BR
│
├────────────────
│
└────────────────── 180 s
```

---

# 📁 20. Cấu trúc Repository

```text
Robocon2027/
│
├── README.md
│
├── src/
│   └── robocon2027_description/
│       ├── config/
│       ├── launch/
│       ├── meshes/
│       ├── urdf/
│       ├── worlds/
│       └── ...
│
├── scripts/
│   └── ...
│
├── backup/
│   └── ...
│
├── Huong dan Robocon 2027.odt
│
├── .gitignore
│
└── ...
```

> `build/`, `install/` và `log/` là các thư mục sinh ra trong quá trình build/chạy ROS 2 và không nên đưa vào Git nếu không cần thiết.

---

# 🖥️ 21. Môi trường phát triển

Môi trường phát triển chính:

```text
OS        : Ubuntu
ROS       : ROS 2
Simulator : Gazebo
Language  : C / C++ / Python
MCU       : STM32
High-Level: Mini PC
Version   : Git / GitHub
```

---

# 🔧 22. Cài đặt Workspace

Clone repository:

```bash
git clone https://github.com/luongngoctu3010/Robocon2027.git
```

Di chuyển vào workspace:

```bash
cd Robocon2027
```

Build:

```bash
colcon build
```

Source workspace:

```bash
source install/setup.bash
```

Kiểm tra package:

```bash
ros2 pkg list | grep robocon
```

---

# 🚀 23. Chạy mô phỏng

Sau khi workspace được build:

```bash
source install/setup.bash
```

Sau đó chạy launch file tương ứng trong package.

Ví dụ:

```bash
ros2 launch robocon2027_description <launch_file>.launch.py
```

Danh sách launch file thực tế sẽ được cập nhật theo phiên bản mô phỏng.

---

# 🧰 24. Công cụ phát triển

Các công cụ được sử dụng:

* Git
* GitHub
* VS Code
* Ubuntu
* ROS 2
* Gazebo
* RViz
* OpenCV
* STM32CubeIDE
* STM32CubeProgrammer
* ST-LINK
* KiCad / CAD tools
* Python
* C/C++

---

# 📋 25. Hardware Inventory

Các linh kiện đã có hoặc đang được sử dụng trong quá trình prototype:

### Controller

* STM32 Nucleo
* STM32 development boards
* Arduino Mega
* Arduino Uno
* ESP32

### Motor

* DC motors
* DC motors with encoder
* JGB37-545
* Các motor prototype khác

### Motor Driver

* IBT-2
* L298N – sử dụng cho các tải nhỏ/actuator phù hợp, **không dùng mặc định cho motor công suất lớn của hệ truyền động chính**.

### Communication

* USB
* UART
* Bluetooth module
* Các giao tiếp khác tùy prototype.

### Mechanical

* Aluminum profile
* Mica / plate
* Gear
* Rack
* Pulley
* Wheel
* Mecanum wheel
* Bearings
* Fasteners

---

# 🔋 26. Battery & Power

Hệ thống pin đang được thiết kế theo yêu cầu thực tế của từng robot.

Các thông số cần xác định:

* Điện áp danh định.
* Điện áp đầy.
* Dung lượng Ah.
* Dòng xả liên tục.
* Dòng xả cực đại.
* BMS.
* Fuse.
* Connector.
* DC/DC converter.

Không lựa chọn pin chỉ dựa trên Ah.

Cần kiểm tra:

```text
Motor current
     ↓
Peak current
     ↓
Driver current
     ↓
Battery discharge capability
     ↓
BMS capability
```

---

# 📐 27. CAD Checklist

Trước khi gia công:

### Kích thước

* [ ] Kích thước robot đúng rulebook.
* [ ] Kiểm tra kích thước khi mở rộng cơ cấu.
* [ ] Kiểm tra khối lượng.

### Cơ khí

* [ ] Không va chạm.
* [ ] Đủ khoảng hở.
* [ ] Đủ độ cứng.
* [ ] Kiểm tra tâm khối lượng.
* [ ] Kiểm tra chống lật.
* [ ] Kiểm tra tải.

### Cơ cấu gắp

* [ ] Gắp được toàn bộ vật thể.
* [ ] Không làm rơi khi tăng tốc.
* [ ] Không va chạm sân.
* [ ] Có giới hạn hành trình.

### Điện

* [ ] STOP.
* [ ] Fuse.
* [ ] Bảo vệ nguồn.
* [ ] Bảo vệ driver.
* [ ] Bảo vệ dây.

### Software

* [ ] Simulation.
* [ ] Test PID.
* [ ] Test encoder.
* [ ] Test communication.
* [ ] Test autonomous.

---

# 🧪 28. Simulation → Real Robot

Quy trình phát triển:

```text
                 IDEA
                   │
                   ▼
             Mathematical Model
                   │
                   ▼
                CAD Model
                   │
                   ▼
             URDF / Gazebo
                   │
                   ▼
              ROS 2 Test
                   │
                   ▼
             Motor Prototype
                   │
                   ▼
            Small-scale Test
                   │
                   ▼
             Full Robot
                   │
                   ▼
             Field Testing
                   │
                   ▼
             Competition
```

---

# 🗓️ 29. Tiến độ dự án

## Giai đoạn 1 – Rule & Strategy

* [x] Nghiên cứu luật.
* [x] Xác định TR/BR.
* [x] Xây dựng chiến thuật sơ bộ.
* [x] Xây dựng timeline 180 s.

## Giai đoạn 2 – Simulation

* [x] ROS 2 workspace.
* [x] Robot description.
* [x] Gazebo.
* [x] SLAM/NAV2 prototype.

## Giai đoạn 3 – Electronics

* [ ] Hoàn thiện STM32.
* [ ] Motor driver.
* [ ] Encoder.
* [ ] PID.
* [ ] Power system.
* [ ] Emergency STOP.

## Giai đoạn 4 – Mechanical

* [ ] CAD TR.
* [ ] CAD BR.
* [ ] Gripper.
* [ ] Tower mechanism.
* [ ] Stair mechanism.
* [ ] Battery mounting.

## Giai đoạn 5 – Prototype

* [ ] TR prototype.
* [ ] BR prototype.
* [ ] Motor test.
* [ ] Encoder test.
* [ ] PID test.
* [ ] Gripper test.
* [ ] Stair test.

## Giai đoạn 6 – Full Integration

* [ ] ROS 2 ↔ STM32.
* [ ] Camera.
* [ ] Navigation.
* [ ] TR ↔ BR coordination.
* [ ] Full field test.

## Giai đoạn 7 – Final

* [ ] 180 s full run.
* [ ] Reliability test.
* [ ] Documentation.
* [ ] Demo.
* [ ] Graduation thesis.

---

# 🎯 30. Mục tiêu cuối cùng

Dự án hướng tới một hệ thống:

```text
       ┌──────────────────────────────┐
       │       ABU ROBOCON 2027       │
       └──────────────┬───────────────┘
                      │
             ┌────────┴────────┐
             │                 │
            TR                 BR
             │                 │
        Transport            Build
             │                 │
          STM32          Mini PC + STM32
             │                 │
        PID + Encoder     ROS 2 + PID
             │                 │
             └────────┬────────┘
                      │
                COORDINATION
                      │
                      ▼
               COMPLETE MISSION
```

---

# 👨‍💻 31. Project Status

**Current status: ACTIVE DEVELOPMENT**

Các phần đang được phát triển song song:

* [x] ROS 2 workspace
* [x] Gazebo environment
* [x] Robot description
* [x] TR/BR system architecture
* [x] Initial strategy
* [x] Motor/encoder research
* [ ] Final TR mechanical design
* [ ] Final BR mechanical design
* [ ] Final power system
* [ ] Final camera
* [ ] Final mecanum wheel selection
* [ ] STM32 motor controller
* [ ] PID tuning
* [ ] Full-field test

---

# 📚 32. Documentation

Các tài liệu kỹ thuật được lưu trong repository:

* Rulebook / game analysis
* Mechanical design
* Electrical design
* Motor calculation
* Battery calculation
* Stair-climbing calculation
* TR/BR strategy
* ROS 2 documentation
* Gazebo simulation
* Testing reports
* Graduation thesis documentation

---

# ⚠️ 33. Development Notes

Đây là repository phát triển của đồ án, vì vậy:

* Một số thông số có thể thay đổi.
* Hardware prototype có thể khác thiết kế cuối.
* Các thông số motor cần được xác nhận bằng datasheet/thử nghiệm.
* Các thông số cơ khí phải được kiểm tra theo rulebook chính thức.
* Không sử dụng kết quả simulation như bằng chứng duy nhất cho khả năng hoạt động thực tế.
* Mọi cơ cấu leo bậc phải được kiểm chứng bằng prototype thực tế.

---

# 📜 34. License

Project được phát triển phục vụ mục đích:

* Đồ án tốt nghiệp.
* Nghiên cứu robot.
* Mô phỏng ROS 2/Gazebo.
* Phát triển robot thi đấu ABU Robocon.

---

# 🤖 ABU Robocon 2027

**TR + BR — Two Robots, One Mission.**

```text
        DESIGN
           ↓
       SIMULATE
           ↓
         BUILD
           ↓
         TEST
           ↓
        OPTIMIZE
           ↓
        COMPETE
```

**Repository:**
https://github.com/luongngoctu3010/Robocon2027

