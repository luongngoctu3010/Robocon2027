# 🤖 ABU ROBOCON 2027

<p align="center">

<img src="https://img.shields.io/badge/ABU%20ROBOCON-2027-blue?style=for-the-badge"/>
<img src="https://img.shields.io/badge/ROBOTICS-Project-orange?style=for-the-badge"/>
<img src="https://img.shields.io/badge/STM32-Firmware-blue?style=for-the-badge"/>
<img src="https://img.shields.io/badge/Jetson-AI%20%26%20Vision-green?style=for-the-badge"/>
<img src="https://img.shields.io/badge/C%2FC%2B%2B-Development-red?style=for-the-badge"/>

</p>

<p align="center">
  <b>Dự án thiết kế và chế tạo robot tham gia ABU Robocon 2027</b>
</p>

<p align="center">
  <i>Cơ khí • Điện • Điều khiển • STM32 • Jetson • Computer Vision</i>
</p>

---

## 📖 Giới thiệu

Đây là repository lưu trữ **toàn bộ mã nguồn, thiết kế, tài liệu kỹ thuật và quá trình phát triển hệ thống robot ABU Robocon 2027** của đội.

Hệ thống gồm hai robot chính:

* 🤖 **TR — Robot vận chuyển**
* 🤖 **BR — Robot xây dựng**

Các robot được phát triển theo hướng tích hợp:

**Cơ khí → Điện → Điều khiển → STM32 → Jetson → Camera → Hệ thống hoàn chỉnh**

Repository được sử dụng để quản lý mã nguồn, CAD, sơ đồ điện, tài liệu thiết kế, quá trình thử nghiệm và phối hợp phát triển giữa các thành viên.

---

# 🎯 Mục tiêu dự án

### Mục tiêu chính

* Thiết kế và chế tạo robot TR và BR.
* Đảm bảo robot hoạt động ổn định trên sân thi đấu.
* Đạt vận tốc thiết kế yêu cầu.
* Điều khiển động cơ bằng encoder và PID.
* Xây dựng cơ cấu gắp, nâng và đặt vật thể.
* Tích hợp STM32 làm bộ điều khiển thời gian thực.
* Tích hợp Jetson cho xử lý cấp cao và thị giác máy tính.
* Xây dựng hệ thống Camera / Computer Vision.
* Đồng bộ hoạt động giữa TR và BR.
* Kiểm thử toàn bộ bài thi trước khi thi đấu.

---

# 🤖 Cấu hình hệ thống

| Hạng mục           | Thông số mục tiêu     |
| ------------------ | --------------------- |
| Số robot           | 2                     |
| Robot 1            | TR                    |
| Robot 2            | BR                    |
| Điện áp hệ thống   | 24V                   |
| Vận tốc mục tiêu   | ≥ 1,3 m/s             |
| Vận tốc thiết kế   | ≥ 1,5 m/s             |
| Đường kính bánh    | Ø120 mm               |
| Khối lượng robot   | khoảng 45–50 kg       |
| Vi điều khiển      | STM32                 |
| Máy tính xử lý     | NVIDIA Jetson         |
| Camera             | Camera Vision         |
| Phản hồi động cơ   | Encoder               |
| Điều khiển động cơ | PID                   |
| Giao tiếp          | CAN / UART / Wireless |

> ⚠️ Các thông số cuối cùng sẽ được cập nhật theo thiết kế thực tế và yêu cầu của luật thi đấu.

---

# 🏗️ Kiến trúc hệ thống

```text
                         ┌─────────────────────┐
                         │      NGƯỜI ĐIỀU KHIỂN │
                         └──────────┬──────────┘
                                    │
                              Kết nối không dây
                                    │
                                    ▼
                    ┌───────────────────────────┐
                    │          JETSON           │
                    │                           │
                    │ • Xử lý Camera            │
                    │ • Computer Vision         │
                    │ • Điều khiển cấp cao      │
                    │ • Xử lý dữ liệu           │
                    └─────────────┬─────────────┘
                                  │
                              UART / CAN
                                  │
                                  ▼
                    ┌───────────────────────────┐
                    │           STM32           │
                    │                           │
                    │ • Điều khiển động cơ      │
                    │ • Encoder                 │
                    │ • PID                     │
                    │ • Cảm biến                │
                    │ • Cơ cấu chấp hành        │
                    │ • Hệ thống an toàn        │
                    └─────────────┬─────────────┘
                                  │
                    ┌─────────────┼─────────────┐
                    │             │             │
                    ▼             ▼             ▼
                 Động cơ 1     Động cơ 2     Cơ cấu
                    │             │          chấp hành
                    └─────────────┴─────────────┘
                                  │
                                  ▼
                           HỆ THỐNG ROBOT
```

---

# 🤖 Robot TR — Robot vận chuyển

## Chức năng

Robot TR đảm nhiệm các nhiệm vụ liên quan đến:

* Di chuyển vật thể.
* Gắp và vận chuyển vật thể.
* Đưa vật thể đến vị trí yêu cầu.
* Di chuyển qua các khu vực của sân.
* Lên và xuống dốc.
* Phối hợp với robot BR.
* Hoàn thành các lượt vận chuyển trong thời gian thi đấu.

## Cấu trúc

```text
TR
├── Khung robot
├── Hệ thống bánh xe
├── Động cơ truyền động
├── Encoder
├── Driver động cơ
├── Bộ điều khiển PID
├── Cơ cấu gắp
├── Cơ cấu nâng
├── STM32
├── Cảm biến
└── Hệ thống nguồn 24V
```

---

# 🤖 Robot BR — Robot xây dựng

## Chức năng

Robot BR đảm nhiệm:

* Nhận vật thể từ TR.
* Vận chuyển vật thể đến khu vực xây dựng.
* Gắp và đặt vật thể.
* Thực hiện cơ cấu xây dựng.
* Hoàn thành nhiệm vụ theo chiến thuật của đội.
* Phối hợp với TR trong toàn bộ trận đấu.

## Cấu trúc

```text
BR
├── Khung robot
├── Hệ thống truyền động
├── Động cơ
├── Encoder
├── Driver
├── Cơ cấu xây dựng
├── Cơ cấu nâng
├── STM32
├── Cảm biến
└── Hệ thống nguồn 24V
```

---

# ⚙️ Hệ thống cơ khí

## Hệ thống truyền động

Thiết kế hiện tại:

* Bánh xe: **Ø120 mm**
* Truyền động vi sai.
* Động cơ DC giảm tốc hành tinh.
* Encoder phản hồi.
* Điều khiển tốc độ vòng kín.
* Điều khiển PID.
* Điện áp động cơ: **24V**.

### Tính toán sơ bộ

Với:

```text
D = 120 mm
R = 60 mm
```

Chu vi bánh:

```text
C = πD
  ≈ 0,377 m
```

Với vận tốc mục tiêu:

```text
v = 1,5 m/s
```

Tốc độ quay bánh xấp xỉ:

```text
RPM = v × 60 / (πD)

RPM ≈ 239 vòng/phút
```

Việc lựa chọn động cơ và tỷ số truyền cuối cùng phụ thuộc vào:

* Khối lượng robot.
* Gia tốc yêu cầu.
* Mô-men động cơ.
* Độ bám bánh.
* Độ dốc sân.
* Tỷ số truyền.
* Điện áp nguồn.
* Hiệu suất hệ thống.

---

# ⚡ Hệ thống điện

## Kiến trúc nguồn

```text
                    PIN 24V
                       │
                  Cầu chì chính
                       │
                 EMERGENCY STOP
                       │
              ┌────────┴────────┐
              │                 │
              ▼                 ▼
       DRIVER ĐỘNG CƠ       DC/DC
              │                 │
              │          ┌──────┴──────┐
              │          │             │
              │          ▼             ▼
              │        STM32         JETSON
              │
              ▼
          ĐỘNG CƠ
```

## Thành phần chính

* Pin 24V.
* Cầu chì bảo vệ.
* Emergency STOP.
* Driver động cơ.
* Bộ DC/DC.
* STM32.
* Jetson.
* Encoder.
* Cảm biến.
* Camera.
* Mạch phân phối nguồn.
* Thiết bị giao tiếp.

---

# 🧠 Hệ thống điều khiển STM32

STM32 đảm nhiệm các chức năng điều khiển thời gian thực.

### Chức năng

* Phát PWM.
* Đọc Encoder.
* Tính tốc độ động cơ.
* Điều khiển PID.
* Điều khiển cơ cấu chấp hành.
* Đọc cảm biến.
* Giao tiếp CAN/UART.
* Giám sát lỗi.
* Xử lý tín hiệu Emergency STOP.

### Cấu trúc PID

```text
Tốc độ đặt
    │
    ▼
  Sai số
    │
    ▼
┌──────────────┐
│     PID      │
│              │
│  P + I + D   │
└──────┬───────┘
       │
       ▼
  Driver động cơ
       │
       ▼
     Động cơ
       │
       ▼
    Encoder
       │
       └──────────────► Phản hồi
```

---

# 👁️ Hệ thống Camera & Computer Vision

Camera kết hợp với Jetson để xử lý thông tin hình ảnh.

### Chức năng dự kiến

* Nhận diện vật thể.
* Nhận diện vị trí.
* Xác định mục tiêu.
* Hỗ trợ căn chỉnh robot.
* Ước lượng vị trí.
* Hỗ trợ tự động hóa một số thao tác.

```text
CAMERA
   │
   ▼
 JETSON
   │
   ├── Xử lý ảnh
   ├── Nhận diện vật thể
   ├── Ước lượng vị trí
   └── Ra quyết định
            │
            ▼
          STM32
```

---

# 💻 Kiến trúc phần mềm

```text
Robocon2027/
│
├── firmware/
│   └── STM32/
│       ├── motor/
│       ├── encoder/
│       ├── pid/
│       ├── sensor/
│       ├── actuator/
│       └── communication/
│
├── software/
│   └── Jetson/
│       ├── camera/
│       ├── vision/
│       ├── localization/
│       └── control/
│
└── tools/
    ├── calibration/
    ├── testing/
    └── logging/
```

---

# 🌿 Quản lý Branch

Repository sử dụng các branch riêng cho từng phần của dự án:

```text
main
│
├── TR
├── BR
├── STM32
├── Jetson
└── Camera
```

| Branch   | Phụ trách         |
| -------- | ----------------- |
| `main`   | Phiên bản ổn định |
| `TR`     | Robot vận chuyển  |
| `BR`     | Robot xây dựng    |
| `STM32`  | Firmware STM32    |
| `Jetson` | Phần mềm Jetson   |
| `Camera` | Computer Vision   |

### Quy trình làm việc

```text
Tạo Issue
    ↓
Tạo / chuyển Branch
    ↓
Lập trình
    ↓
Commit
    ↓
Push
    ↓
Pull Request
    ↓
Kiểm tra
    ↓
Merge vào main
```

> `main` chỉ nên chứa phiên bản đã được kiểm tra và tích hợp ổn định.

---

# 📁 Cấu trúc Repository

```text
Robocon2027/
│
├── README.md
├── LICENSE
├── .gitignore
│
├── docs/
│   ├── rulebook/
│   ├── thiet-ke/
│   ├── dien/
│   ├── co-khi/
│   ├── chien-thuat/
│   └── testing/
│
├── CAD/
│   ├── TR/
│   └── BR/
│
├── firmware/
│   └── STM32/
│
├── software/
│   └── Jetson/
│
├── vision/
│   └── Camera/
│
├── hardware/
│   ├── BOM/
│   ├── schematic/
│   └── wiring/
│
├── simulation/
│
├── tests/
│
└── logs/
```

---

# 🧪 Quy trình kiểm thử

## 1. Kiểm thử linh kiện

* Động cơ.
* Encoder.
* Driver.
* Cảm biến.
* Cơ cấu chấp hành.
* Giao tiếp.

## 2. Kiểm thử từng hệ thống

* Hệ thống truyền động.
* Cơ cấu gắp.
* Cơ cấu nâng.
* Cơ cấu xây dựng.
* Camera.
* Jetson.

## 3. Kiểm thử robot

* Chạy thẳng.
* Quay.
* Tăng tốc.
* Phanh.
* Điều khiển PID.
* Gắp vật thể.
* Đặt vật thể.
* Lên dốc.
* Xuống dốc.

## 4. Kiểm thử toàn hệ thống

* TR + BR.
* Giao tiếp giữa hai robot.
* Thực hiện toàn bộ chiến thuật.
* Chạy thử trận đấu.
* Kiểm tra trong 180 giây.
* Kiểm tra các tình huống lỗi.

---

# 📅 Tiến độ dự án

| Giai đoạn | Công việc                   |
| --------- | --------------------------- |
| 01        | Phân tích luật              |
| 02        | Xây dựng kiến trúc hệ thống |
| 03        | Thiết kế cơ khí             |
| 04        | Thiết kế điện               |
| 05        | Chọn linh kiện              |
| 06        | Gia công                    |
| 07        | Lập trình STM32             |
| 08        | Lập trình Jetson            |
| 09        | Computer Vision             |
| 10        | Tích hợp robot              |
| 11        | Tuning PID                  |
| 12        | Chạy thử sân                |
| 13        | Mô phỏng trận đấu           |
| 14        | Hoàn thiện                  |

---

# 📊 Trạng thái hiện tại

| Hệ thống       | Trạng thái        |
| -------------- | ----------------- |
| Phân tích luật | 🟡 Đang thực hiện |
| Thiết kế TR    | 🟡 Đang thực hiện |
| Thiết kế BR    | 🟡 Đang thực hiện |
| Cơ khí         | 🟡 Đang thực hiện |
| Điện           | 🟡 Đang thực hiện |
| STM32          | 🟡 Đang thực hiện |
| Jetson         | ⚪ Chuẩn bị        |
| Camera         | ⚪ Chuẩn bị        |
| PID            | ⚪ Chuẩn bị        |
| Tích hợp       | ⚪ Chuẩn bị        |
| Chạy thử sân   | ⚪ Chuẩn bị        |

**Chú thích:**

* 🟢 Hoàn thành
* 🟡 Đang thực hiện
* ⚪ Chưa thực hiện
* 🔴 Đang gặp vấn đề

---

# 📸 Hình ảnh dự án

## Robot TR

> Thêm hình ảnh CAD hoặc robot thực tế tại đây.

```text
docs/images/TR.jpg
```

## Robot BR

> Thêm hình ảnh CAD hoặc robot thực tế tại đây.

```text
docs/images/BR.jpg
```

## Hệ thống hoàn chỉnh

> Thêm hình ảnh TR + BR sau khi hoàn thiện.

---

# 🎥 Video

Các video thử nghiệm sẽ được cập nhật tại đây:

* 🎬 Test động cơ
* 🎬 Test PID
* 🎬 Test cơ cấu gắp
* 🎬 Test cơ cấu xây dựng
* 🎬 Test Camera
* 🎬 Test TR
* 🎬 Test BR
* 🎬 Test toàn bộ trận đấu

---

# 🛠️ Công cụ sử dụng

### Lập trình

* C
* C++
* Python

### STM32

* STM32CubeIDE
* STM32CubeMX
* HAL / LL

### Jetson

* Python
* C++
* OpenCV
* ROS 2 *(nếu sử dụng)*

### Cơ khí

* SolidWorks
* Fusion 360
* CAD
* CNC
* Laser
* 3D Printing

### Quản lý dự án

* Git
* GitHub
* Issues
* Pull Requests
* Branches

---

# ⚠️ An toàn

Robot sử dụng động cơ công suất lớn, pin 24V và các cơ cấu chuyển động nhanh.

Trước khi vận hành:

* Kiểm tra cực tính nguồn.
* Kiểm tra toàn bộ dây điện.
* Kiểm tra cầu chì.
* Kiểm tra Emergency STOP.
* Kiểm tra driver động cơ.
* Cố định các cơ cấu cơ khí.
* Không đứng trong vùng chuyển động của robot.
* Chạy thử ở tốc độ thấp trước.
* Chỉ chạy tốc độ tối đa sau khi hệ thống đã được kiểm tra.

---

# 🏆 ABU Robocon 2027

> **Thiết kế → Chế tạo → Lập trình → Kiểm thử → Thi đấu**

<p align="center">

### 🤖 ROBOTICS TEAM — ROBOCON 2027

<b>TR • BR • STM32 • JETSON • CAMERA</b>

</p>

---

## 👥 Thành viên

| STT | Thành viên | Phụ trách         |
| --: | ---------- | ----------------- |
|  01 | —          | TR                |
|  02 | —          | BR                |
|  03 | —          | STM32             |
|  04 | —          | Jetson            |
|  05 | —          | Camera            |
|  06 | —          | Tích hợp hệ thống |

> Danh sách thành viên sẽ được cập nhật trong quá trình phát triển dự án.

