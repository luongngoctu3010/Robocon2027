# 🤖 ABU ROBOCON 2027 – TR & BR

Repository lưu trữ mã nguồn, thiết kế phần cứng, phần mềm điều khiển và tài liệu kỹ thuật của dự án robot tham gia ABU Robocon 2027.

Hệ thống gồm hai robot:

- **TR (Throwing Robot):** Robot điều khiển bằng tay thông qua tay cầm PS2.
- **BR (Building Robot):** Robot tự động sử dụng Camera 3D, LiDAR, Mini PC, ROS 2 và hệ thống điều khiển STM32.

---

# 1. SYSTEM OVERVIEW

Hệ thống được chia thành hai kiến trúc điều khiển độc lập:

| Robot | Chế độ hoạt động | Bộ điều khiển chính |
|---|---|---|
| TR | Điều khiển bằng tay | Arduino Mega 2560 |
| BR | Điều khiển tự động | Mini PC + STM32F767ZIT6 |

## 1.1. TR – Throwing Robot

TR sử dụng tay cầm PS2 để điều khiển chuyển động và các cơ cấu chấp hành.

### Sơ đồ kiến trúc điều khiển

```text
             PS2 CONTROLLER
                    |
                    | USB
                    |
                    v
             MINI PC
                    |
                    | WiFi
                    |
                    v
                 ESP32
                    |
                    | UART
                    |
                    v
             ARDUINO MEGA 2560
                    |
          +---------+----------+
          |                    |
          v                    v
    DRIVE SYSTEM          GRIPPER SYSTEM
          |                    |
     4 x IBT-2             VAN DIEN TU 5/2
          |                    |
     4 DC MOTORS           XI LANH KHI NEN
          |                    |
     4 MECANUM WHEELS      CO CAU GAP
          |                    |
          v                    v
       ENCODERS          LIMIT SENSORS
          |                    |
          +---------+----------+
                    |
                    v
             ARDUINO MEGA
```

### A. Hệ thống điều khiển chuyển động

| Thành phần | Chức năng |
|---|---|
| PS2 Controller | Điều khiển robot bằng tay |
| Mini PC | Tiếp nhận và xử lý tín hiệu điều khiển |
| ESP32 | Truyền dữ liệu điều khiển qua WiFi |
| Arduino Mega 2560 | Bộ điều khiển trung tâm TR |
| IBT-2 | Driver điều khiển động cơ DC |
| 4 DC Motors | Tạo chuyển động cho robot |
| 4 Mecanum Wheels | Di chuyển đa hướng |
| Encoders | Phản hồi tốc độ và số vòng quay động cơ |

Arduino Mega 2560 nhận lệnh từ ESP32, xử lý tín hiệu điều khiển và xuất PWM/Direction đến 4 module IBT-2.

Encoder cung cấp tín hiệu phản hồi để đo tốc độ và hỗ trợ điều khiển chuyển động.

### B. Cơ cấu gắp

```text
ARDUINO MEGA 2560
        |
        v
VAN DIEN TU 5/2
        |
        v
XI LANH KHI NEN
        |
        v
CO CAU GAP
        |
        v
CAM BIEN HANH TRINH
        |
        +------> ARDUINO MEGA 2560
```

Chức năng:
- Điều khiển van điện từ 5/2.
- Điều khiển xi lanh khí nén thực hiện thao tác gắp và nhả.
- Cảm biến hành trình xác nhận vị trí cơ cấu.
- Arduino Mega xử lý tín hiệu phản hồi và điều khiển trình tự hoạt động.

---

# 2. BR – BUILDING ROBOT

BR hoạt động tự động, sử dụng Camera 3D và LiDAR để nhận biết môi trường, định vị và lập kế hoạch di chuyển.

Mini PC thực hiện xử lý dữ liệu cảm biến, ROS 2, SLAM, nhận dạng vật thể và điều hướng.

STM32F767ZIT6 nhận lệnh chuyển động qua Ethernet UDP và điều khiển các hệ thống chấp hành.

## 2.1. Sơ đồ kiến trúc tổng thể

```text
             CAMERA 3D
                  |
                  |
                LIDAR
                  |
                  v
          MINI PC ASUS PN63-S1
                  |
          +-------+-------+
          |               |
          v               v
       ROS 2           COMPUTER VISION
          |               |
          |               |
          +-------+-------+
                  |
             MOTION COMMAND
                  |
                  v
             ETHERNET UDP
                  |
                  v
          STM32F767ZIT6
                  |
       +----------+-----------+
       |          |           |
       v          v           v
    USART1     USART2      GPIO/PWM
       |          |           |
       v          v           v
 HOVERBOARD  HOVERBOARD    TB6600
   STM32F103   GD32F103       |
       |          |           v
       |          |       STEPPER MOTOR
       |          |           |
       v          v           v
    2 BLDC     2 BLDC      LIFT SYSTEM
    MOTORS     MOTORS
       |          |
       +-----+----+
             |
         4 WHEELS
             |
             v
       HALL FEEDBACK

          STM32F767ZIT6
                  |
                  v
          VAN DIEN TU 5/2
                  |
                  v
          XI LANH KHI NEN
                  |
                  v
              CO CAU GAP
                  |
                  v
          CAM BIEN HANH TRINH
```

## 2.2. Hệ thống xử lý và nhận thức môi trường

| Thành phần | Chức năng |
|---|---|
| Mini PC ASUS PN63-S1 | Máy tính xử lý trung tâm |
| Camera 3D | Thu thập dữ liệu hình ảnh và chiều sâu |
| LiDAR | Quét khoảng cách và xây dựng bản đồ môi trường |
| ROS 2 | Hệ điều hành robot và quản lý các node |
| SLAM | Định vị đồng thời xây dựng bản đồ |
| Nav2 | Lập kế hoạch và điều hướng tự động |
| Computer Vision | Nhận dạng và xác định vị trí vật thể |

Các chức năng xử lý dự kiến:

- Thu thập dữ liệu Camera 3D và LiDAR.
- Xây dựng bản đồ môi trường bằng SLAM.
- Định vị robot.
- Nhận dạng các vật thể và vị trí thao tác.
- Lập kế hoạch đường đi.
- Tính toán vận tốc tuyến tính và vận tốc góc.
- Gửi lệnh điều khiển đến STM32F767ZIT6.

## 2.3. Hệ thống truyền thông Ethernet UDP

```text
MINI PC ASUS PN63-S1
        |
        | Ethernet
        | UDP
        v
STM32F767ZIT6
```

Ethernet UDP được sử dụng để truyền lệnh điều khiển từ Mini PC đến STM32F767ZIT6.

Thông tin truyền thông dự kiến:

| Thông số | Cấu hình |
|---|---|
| Giao thức | UDP |
| Giao tiếp vật lý | Ethernet |
| Tốc độ Ethernet | 10/100 Mbps |
| IP Mini PC | 192.168.1.20 |
| IP STM32 | 192.168.1.100 |
| UDP Port | 5000 (dự kiến) |
| Tần số gửi lệnh | 100 Hz (mục tiêu) |

*Các thông số IP và UDP Port cần thống nhất với firmware thực tế.*

Các nội dung cần triển khai:
- Cấu hình Ethernet và LwIP.
- Nhận dữ liệu UDP trên STM32.
- Kiểm tra checksum và số thứ tự gói tin.
- Phát hiện mất kết nối.
- Phản hồi trạng thái hệ thống.
- Đưa robot về trạng thái an toàn khi mất lệnh điều khiển.

## 2.4. Hệ thống truyền động bánh xe

BR sử dụng 4 động cơ BLDC, được điều khiển bởi hai bo Hoverboard.

```text
                 STM32F767ZIT6
                        |
              +---------+---------+
              |                   |
           USART1               USART2
              |                   |
              v                   v
        STM32F103RCT6        GD32F103RCT6
        HOVERBOARD           HOVERBOARD
              |                   |
        +-----+-----+       +-----+-----+
        |           |       |           |
        v           v       v           v
       BLDC        BLDC    BLDC        BLDC
        FL          FR      RL          RR
```

Phân công điều khiển:

| Bộ điều khiển | Động cơ | Vị trí |
|---|---|---|
| STM32F103RCT6 | 2 BLDC | Cầu trước: FL, FR |
| GD32F103RCT6 | 2 BLDC | Cầu sau: RL, RR |

Chức năng:
- Nhận lệnh vận tốc từ STM32F767ZIT6 qua UART.
- Điều khiển tốc độ và chiều quay động cơ BLDC.
- Đọc tín hiệu Hall để xác định tốc độ và trạng thái động cơ.
- Gửi dữ liệu phản hồi về STM32F767ZIT6.
- Phối hợp điều khiển 4 bánh xe để robot di chuyển.

## 2.5. Cơ cấu nâng hạ và leo cầu thang

```text
          STM32F767ZIT6
                 |
                 v
              TB6600
                 |
                 v
           STEPPER MOTOR
                 |
                 v
          LIFT MECHANISM
                 |
                 v
          LIMIT SENSORS
                 |
                 v
          STM32F767ZIT6
```

Thành phần:

| Thành phần | Chức năng |
|---|---|
| STM32F767ZIT6 | Điều khiển cơ cấu nâng hạ |
| TB6600 | Driver điều khiển động cơ bước |
| Stepper Motor | Tạo chuyển động nâng hạ |
| Cơ cấu cơ khí | Nâng, hạ và hỗ trợ leo cầu thang |
| Cảm biến hành trình | Xác nhận vị trí giới hạn |

STM32F767ZIT6 phát tín hiệu STEP/DIR/ENA đến TB6600 để điều khiển động cơ bước.

Cảm biến hành trình được sử dụng để xác định vị trí trên và dưới của cơ cấu, đồng thời hỗ trợ bảo vệ khi vượt quá giới hạn hành trình.

## 2.6. Cơ cấu gắp

```text
          STM32F767ZIT6
                 |
                 v
          VAN DIEN TU 5/2
                 |
                 v
          XI LANH KHI NEN
                 |
                 v
              CO CAU GAP
                 |
                 v
          CAM BIEN HANH TRINH
                 |
                 v
          STM32F767ZIT6
```

Chức năng:
- Điều khiển van điện từ 5/2.
- Điều khiển xi lanh khí nén.
- Thực hiện thao tác gắp và nhả vật thể.
- Đọc cảm biến hành trình để xác nhận trạng thái cơ cấu.
- Phối hợp với chương trình tự động trên Mini PC.

---

# 3. COMMUNICATION ARCHITECTURE

| Kết nối | Giao thức | Thiết bị |
|---|---|---|
| PS2 → Mini PC | USB | TR |
| Mini PC → ESP32 | WiFi | TR |
| ESP32 → Arduino Mega | UART | TR |
| Mini PC → STM32F767 | Ethernet UDP | BR |
| STM32F767 → Hoverboard F103 | USART1 | BR |
| STM32F767 → Hoverboard GD32 | USART2 | BR |
| STM32F767 → TB6600 | STEP/DIR/ENA | BR |
| STM32 → Van điện từ | GPIO/Driver | TR & BR |
| Encoder → Arduino Mega | Digital Interrupt | TR |
| Hall → Hoverboard MCU | Hall Sensor | BR |
| Limit Sensor → MCU | Digital Input | TR & BR |

---

# 4. FEEDBACK & CONTROL SYSTEM

Hệ thống sử dụng các loại cảm biến phản hồi để giám sát và điều khiển chuyển động.

| Cảm biến | Robot | Mục đích |
|---|---|---|
| Encoder | TR | Phản hồi tốc độ và vị trí bánh xe |
| Hall Sensor | BR | Phản hồi trạng thái và tốc độ BLDC |
| Limit Sensor | TR | Xác định vị trí cơ cấu gắp |
| Limit Sensor | BR | Xác định vị trí cơ cấu nâng hạ và gắp |
| Camera 3D | BR | Nhận biết vật thể và chiều sâu |
| LiDAR | BR | Định vị và xây dựng bản đồ |

---

# 5. SOFTWARE & DEVELOPMENT TOOLS

| Công cụ | Mục đích |
|---|---|
| Arduino IDE | Lập trình Arduino Mega và ESP32 |
| STM32CubeIDE | Phát triển firmware STM32 |
| PlatformIO | Build firmware Hoverboard |
| ROS 2 | Điều khiển robot tự động |
| SLAM Toolbox | Xây dựng bản đồ |
| Nav2 | Điều hướng tự động |
| Python | Xử lý dữ liệu và Computer Vision |
| OpenCV | Xử lý hình ảnh |
| YOLO | Nhận dạng vật thể |
| Git & GitHub | Quản lý mã nguồn |
| CAD Software | Thiết kế cơ khí và lắp ráp |

---

# 6. REPOSITORY STRUCTURE

```text
Robocon2027/
│
├── README.md
│
├── TR/
│   ├── CAD/
│   │
│   ├── Code/
│   │   ├── ESP32/
│   │   └── ArduinoMega2560/
│   │       └── Arduinomega2560.ino
│   │
│   ├── Electronics/
│   ├── Mechanical/
│   └── Documents/
│
├── BR/
│   ├── CAD/
│   │
│   ├── Code/
│   │   ├── MiniPC/
│   │   │   ├── br_navigation/
│   │   │   ├── br_slam/
│   │   │   └── computer_vision/
│   │   │
│   │   ├── STM32F767/
│   │   │
│   │   ├── Hoverboard_STM32F103/
│   │   │
│   │   └── Hoverboard_GD32F103/
│   │
│   ├── Electronics/
│   ├── Mechanical/
│   └── Documents/
│
└── Documents/
    ├── Rulebook/
    ├── Strategy/
    ├── System_Architecture/
    └── Reports/
```

---

# 7. DEVELOPMENT OBJECTIVES

Các mục tiêu phát triển chính của dự án:

### TR
- Hoàn thiện điều khiển bằng tay cầm PS2.
- Truyền dữ liệu điều khiển qua WiFi.
- Điều khiển 4 động cơ DC bằng IBT-2.
- Phản hồi encoder và kiểm soát chuyển động.
- Hoàn thiện cơ cấu gắp khí nén.

### BR
- Hoàn thiện hệ thống Camera 3D và LiDAR.
- Triển khai SLAM và Navigation bằng ROS 2.
- Hoàn thiện truyền thông Ethernet UDP giữa Mini PC và STM32F767.
- Điều khiển 4 động cơ BLDC bằng hai bo Hoverboard.
- Hoàn thiện cơ cấu nâng hạ, leo cầu thang.
- Hoàn thiện cơ cấu gắp khí nén.
- Đồng bộ điều khiển chuyển động và các cơ cấu tự động.

---

# 8. PROJECT STATUS

Repository được cập nhật trong quá trình thiết kế, chế tạo, lập trình, thử nghiệm và hoàn thiện hai robot TR và BR.

Các nội dung cập nhật bao gồm:
- Mechanical Design.
- Electrical & Electronics.
- Embedded Firmware.
- ROS 2 & Navigation.
- Communication Protocol.
- Sensor Feedback.
- System Integration.
- Testing & Optimization.

---

**ABU ROBOCON 2027 – TR & BR**  
*Mechanical Design | Embedded Systems | Robotics | Autonomous Navigation*
