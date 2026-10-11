// =====================================================
// TR ROBOCON 2027 - MECANUM + ENCODER + PID
//
// PS2
//   ↓
// Ubuntu
//   ↓ WiFi UDP
// ESP32
//   ↓ UART
// MEGA 2560
//   ↓
// 4 x IBT-2
//   ↓
// 4 x MECANUM MOTOR + ENCODER
//
// MOTOR POSITION - NHIN TU TREN XUONG:
//
//                 +Y = TIEN
//                     ↑
//
//          M2                 M1
//       TRAI TRUOC        PHAI TRUOC
//
//   -X ←───────────────┼───────────────→ +X
//
//          M3                 M4
//        TRAI SAU           PHAI SAU
//
//                     ↓
//                 -Y = LUI
//
// =====================================================

#include <Arduino.h>
#include <math.h>

// =====================================================
// MOTOR 1 - TRUOC PHAI
// =====================================================

#define M1_RPWM 5
#define M1_LPWM 6
#define M1_RE   22
#define M1_LE   23

// =====================================================
// MOTOR 2 - TRUOC TRAI
// =====================================================

#define M2_RPWM 7
#define M2_LPWM 8
#define M2_RE   24
#define M2_LE   25

// =====================================================
// MOTOR 3 - SAU TRAI
// =====================================================

#define M3_RPWM 9
#define M3_LPWM 10
#define M3_RE   26
#define M3_LE   27

// =====================================================
// MOTOR 4 - SAU PHAI
// =====================================================

#define M4_RPWM 11
#define M4_LPWM 12
#define M4_RE   28
#define M4_LE   29

// =====================================================
// ENCODER PIN
//
// M1: A = D2,  B = D38
// M2: A = D3,  B = D39
// M3: A = D20, B = D40
// M4: A = D21, B = D41
//
// Mega 2560:
// D2, D3, D20, D21 ho tro external interrupt.
//
// =====================================================

#define M1_ENC_A 2
#define M1_ENC_B 38

#define M2_ENC_A 3
#define M2_ENC_B 39

#define M3_ENC_A 20
#define M3_ENC_B 40

#define M4_ENC_A 21
#define M4_ENC_B 41

// =====================================================
// PS2 DATA
// =====================================================

int LX = 128;
int LY = 128;
int RX = 128;
int RY = 128;

// =====================================================
// UART BUFFER
// =====================================================

String rxLine = "";

// =====================================================
// WATCHDOG
// =====================================================

unsigned long lastPacketTime = 0;

const unsigned long WATCHDOG_TIMEOUT = 300;

// =====================================================
// CONTROL
// =====================================================

const int DEADZONE = 15;

// =====================================================
// ORIGINAL MAX SPEED
// =====================================================

const int MAX_SPEED = 80;

// =====================================================
// MOTOR DIRECTION CORRECTION
// GIU NGUYEN HUONG MOTOR GOC
// =====================================================

const int M1_DIR = 1;
const int M2_DIR = -1;
const int M3_DIR = -1;
const int M4_DIR = 1;

// =====================================================
// PID CONFIGURATION
//
// COUNTS_PER_REV:
// So xung dem duoc trong 1 vong truc dang do.
//
// Phai hieu chinh theo encoder va hop so thuc te.
// Gia tri 600 ben duoi CHI LA GIA TRI KHOI TAO.
//
// MAX_TARGET_RPM:
// RPM muc tieu toi da khi lenh mecanum dat MAX_SPEED.
//
// =====================================================

const float COUNTS_PER_REV[4] = {
    600.0f,
    600.0f,
    600.0f,
    600.0f
};

// Doi thanh -1 neu chieu dem encoder nguoc voi chieu quy uoc.
const int ENCODER_DIR[4] = {
    1, 1, 1, 1
};

const float MAX_TARGET_RPM = 100.0f;

// PID gains - gia tri khoi dau, can tinh chinh thuc te.
float Kp[4] = {
    1.5f, 1.5f, 1.5f, 1.5f
};

float Ki[4] = {
    0.5f, 0.5f, 0.5f, 0.5f
};

float Kd[4] = {
    0.0f, 0.0f, 0.0f, 0.0f
};

// Chu ky cap nhat PID: 20 ms
const unsigned long PID_INTERVAL_MS = 20;

// Chu ky in Serial: 200 ms
const unsigned long DEBUG_INTERVAL_MS = 200;

// Gioi han PWM PID
const int PID_PWM_LIMIT = 255;

// Gioi han tich phan
const float INTEGRAL_LIMIT = 300.0f;

// =====================================================
// ENCODER VARIABLES
// =====================================================

volatile long encoderCount[4] = {
    0, 0, 0, 0
};

long previousEncoderCount[4] = {
    0, 0, 0, 0
};

// =====================================================
// PID VARIABLES
// =====================================================

// RPM muc tieu
float targetRPM[4] = {
    0, 0, 0, 0
};

// RPM do duoc
float measuredRPM[4] = {
    0, 0, 0, 0
};

// Sai so RPM
float pidError[4] = {
    0, 0, 0, 0
};

// Sai so truoc
float previousError[4] = {
    0, 0, 0, 0
};

// Tich phan
float integral[4] = {
    0, 0, 0, 0
};

// Dao ham
float derivative[4] = {
    0, 0, 0, 0
};

// PWM PID
int pidPWM[4] = {
    0, 0, 0, 0
};

unsigned long lastPIDTime = 0;
unsigned long lastDebugTime = 0;

// =====================================================
// ENCODER ISR
//
// Dem suon len cua kenh A.
// Doc kenh B de xac dinh chieu.
//
// Neu chieu RPM hien thi nguoc quy uoc,
// hieu chinh ENCODER_DIR cua tung motor.
//
// =====================================================

void encoderISR1()
{
    if (digitalRead(M1_ENC_B))
    {
        encoderCount[0]++;
    }
    else
    {
        encoderCount[0]--;
    }
}

void encoderISR2()
{
    if (digitalRead(M2_ENC_B))
    {
        encoderCount[1]++;
    }
    else
    {
        encoderCount[1]--;
    }
}

void encoderISR3()
{
    if (digitalRead(M3_ENC_B))
    {
        encoderCount[2]++;
    }
    else
    {
        encoderCount[2]--;
    }
}

void encoderISR4()
{
    if (digitalRead(M4_ENC_B))
    {
        encoderCount[3]++;
    }
    else
    {
        encoderCount[3]--;
    }
}

// =====================================================
// MOTOR 1
// =====================================================

void motor1(int speed)
{
    speed = speed * M1_DIR;

    speed = constrain(speed, -255, 255);

    if (speed > 0)
    {
        analogWrite(M1_RPWM, speed);
        analogWrite(M1_LPWM, 0);
    }
    else if (speed < 0)
    {
        analogWrite(M1_RPWM, 0);
        analogWrite(M1_LPWM, -speed);
    }
    else
    {
        analogWrite(M1_RPWM, 0);
        analogWrite(M1_LPWM, 0);
    }
}

// =====================================================
// MOTOR 2
// =====================================================

void motor2(int speed)
{
    speed = speed * M2_DIR;

    speed = constrain(speed, -255, 255);

    if (speed > 0)
    {
        analogWrite(M2_RPWM, speed);
        analogWrite(M2_LPWM, 0);
    }
    else if (speed < 0)
    {
        analogWrite(M2_RPWM, 0);
        analogWrite(M2_LPWM, -speed);
    }
    else
    {
        analogWrite(M2_RPWM, 0);
        analogWrite(M2_LPWM, 0);
    }
}

// =====================================================
// MOTOR 3
// =====================================================

void motor3(int speed)
{
    speed = speed * M3_DIR;

    speed = constrain(speed, -255, 255);

    if (speed > 0)
    {
        analogWrite(M3_RPWM, speed);
        analogWrite(M3_LPWM, 0);
    }
    else if (speed < 0)
    {
        analogWrite(M3_RPWM, 0);
        analogWrite(M3_LPWM, -speed);
    }
    else
    {
        analogWrite(M3_RPWM, 0);
        analogWrite(M3_LPWM, 0);
    }
}

// =====================================================
// MOTOR 4
// =====================================================

void motor4(int speed)
{
    speed = speed * M4_DIR;

    speed = constrain(speed, -255, 255);

    if (speed > 0)
    {
        analogWrite(M4_RPWM, speed);
        analogWrite(M4_LPWM, 0);
    }
    else if (speed < 0)
    {
        analogWrite(M4_RPWM, 0);
        analogWrite(M4_LPWM, -speed);
    }
    else
    {
        analogWrite(M4_RPWM, 0);
        analogWrite(M4_LPWM, 0);
    }
}

// =====================================================
// STOP ALL
// =====================================================

void stopAll()
{
    motor1(0);
    motor2(0);
    motor3(0);
    motor4(0);
}

// =====================================================
// RESET PID
// =====================================================

void resetPID()
{
    for (int i = 0; i < 4; i++)
    {
        targetRPM[i] = 0.0f;
        measuredRPM[i] = 0.0f;
        pidError[i] = 0.0f;
        previousError[i] = 0.0f;
        integral[i] = 0.0f;
        derivative[i] = 0.0f;
        pidPWM[i] = 0;
    }

    lastPIDTime = millis();
}

// =====================================================
// ENABLE IBT-2
// =====================================================

void enableDrivers()
{
    digitalWrite(M1_RE, HIGH);
    digitalWrite(M1_LE, HIGH);

    digitalWrite(M2_RE, HIGH);
    digitalWrite(M2_LE, HIGH);

    digitalWrite(M3_RE, HIGH);
    digitalWrite(M3_LE, HIGH);

    digitalWrite(M4_RE, HIGH);
    digitalWrite(M4_LE, HIGH);
}

// =====================================================
// AXIS -> SPEED
// =====================================================

int axisToSpeed(int value)
{
    int speed = value - 128;

    // DEADZONE
    if (abs(speed) < DEADZONE)
    {
        speed = 0;
    }

    // SCALE
    speed = map(
        speed,
        -127,
        127,
        -MAX_SPEED,
        MAX_SPEED
    );

    return constrain(
        speed,
        -MAX_SPEED,
        MAX_SPEED
    );
}

// =====================================================
// MECANUM CONTROL
//
// +Y = TIEN
// -Y = LUI
// +X = PHAI
// -X = TRAI
// +W = QUAY TRAI
// -W = QUAY PHAI
//
// M1 = Vy + Vx + W
// M2 = Vy - Vx - W
// M3 = Vy + Vx - W
// M4 = Vy - Vx + W
//
// PID nhan RPM muc tieu tu ket qua mecanum.
// =====================================================

void mecanumControl()
{
    // X
    int Vx = -axisToSpeed(LX);

    // Y
    int Vy = -axisToSpeed(LY);

    // ROTATION
    int W = -axisToSpeed(RX);

    // MECANUM
    int M1 = Vy + Vx + W;
    int M2 = Vy - Vx - W;
    int M3 = Vy + Vx - W;
    int M4 = Vy - Vx + W;

    // NORMALIZE
    int maxValue = abs(M1);

    if (abs(M2) > maxValue)
    {
        maxValue = abs(M2);
    }

    if (abs(M3) > maxValue)
    {
        maxValue = abs(M3);
    }

    if (abs(M4) > maxValue)
    {
        maxValue = abs(M4);
    }

    if (maxValue > MAX_SPEED)
    {
        M1 = M1 * MAX_SPEED / maxValue;
        M2 = M2 * MAX_SPEED / maxValue;
        M3 = M3 * MAX_SPEED / maxValue;
        M4 = M4 * MAX_SPEED / maxValue;
    }

    // =================================================
    // CONVERT MECANUM COMMAND TO TARGET RPM
    //
    // M = +/- MAX_SPEED
    // => RPM muc tieu = +/- MAX_TARGET_RPM
    //
    // Khong goi motor1..motor4 truc tiep o day.
    // PID se tinh PWM va dieu khien motor.
    // =================================================

    targetRPM[0] =
        (float)M1 * MAX_TARGET_RPM / MAX_SPEED;

    targetRPM[1] =
        (float)M2 * MAX_TARGET_RPM / MAX_SPEED;

    targetRPM[2] =
        (float)M3 * MAX_TARGET_RPM / MAX_SPEED;

    targetRPM[3] =
        (float)M4 * MAX_TARGET_RPM / MAX_SPEED;
}

// =====================================================
// UPDATE PID
//
// Moi PID_INTERVAL_MS:
// 1. Doc encoder
// 2. Tinh RPM
// 3. Tinh sai so
// 4. Tinh PID
// 5. Xuat PWM den 4 motor
//
// =====================================================

void updatePID()
{
    unsigned long now = millis();

    unsigned long elapsed = now - lastPIDTime;

    if (elapsed < PID_INTERVAL_MS)
    {
        return;
    }

    if (elapsed == 0)
    {
        return;
    }

    float dt = elapsed / 1000.0f;

    lastPIDTime = now;

    long currentCount[4];

    // Doc bo dem encoder an toan.
    noInterrupts();

    currentCount[0] = encoderCount[0];
    currentCount[1] = encoderCount[1];
    currentCount[2] = encoderCount[2];
    currentCount[3] = encoderCount[3];

    interrupts();

    // Tinh RPM va PID tung motor.
    for (int i = 0; i < 4; i++)
    {
        long deltaCount =
            currentCount[i] - previousEncoderCount[i];

        previousEncoderCount[i] = currentCount[i];

        // RPM = so vong / thoi gian (phut)
        measuredRPM[i] =
            ((float)deltaCount * 60.0f) /
            (COUNTS_PER_REV[i] * dt);

        // Hieu chinh chieu encoder.
        measuredRPM[i] *= ENCODER_DIR[i];

        // Sai so.
        pidError[i] =
            targetRPM[i] - measuredRPM[i];

        // Neu khong co lenh chay, dung motor va xoa PID.
        if (fabs(targetRPM[i]) < 0.01f)
        {
            integral[i] = 0.0f;
            derivative[i] = 0.0f;
            previousError[i] = 0.0f;
            pidError[i] = 0.0f;
            pidPWM[i] = 0;

            continue;
        }

        // Tich phan.
        integral[i] += pidError[i] * dt;

        integral[i] = constrain(
            integral[i],
            -INTEGRAL_LIMIT,
            INTEGRAL_LIMIT
        );

        // Dao ham.
        derivative[i] =
            (pidError[i] - previousError[i]) / dt;

        // Cong thuc PID.
        float output =
            Kp[i] * pidError[i] +
            Ki[i] * integral[i] +
            Kd[i] * derivative[i];

        // Gioi han PWM.
        output = constrain(
            output,
            -PID_PWM_LIMIT,
            PID_PWM_LIMIT
        );

        pidPWM[i] = (int)output;

        previousError[i] = pidError[i];
    }

    // Xuat PWM den driver.
    motor1(pidPWM[0]);
    motor2(pidPWM[1]);
    motor3(pidPWM[2]);
    motor4(pidPWM[3]);
}

// =====================================================
// PRINT PID DEBUG
//
// Serial Monitor: 115200 baud
//
// M1[T/R/E/P] = Target RPM / Real RPM / Error / PWM
//
// =====================================================

void printPIDDebug()
{
    unsigned long now = millis();

    if (now - lastDebugTime < DEBUG_INTERVAL_MS)
    {
        return;
    }

    lastDebugTime = now;

    Serial.print("M1[T/R/E/P]=");
    Serial.print(targetRPM[0], 1);
    Serial.print("/");
    Serial.print(measuredRPM[0], 1);
    Serial.print("/");
    Serial.print(pidError[0], 1);
    Serial.print("/");
    Serial.print(pidPWM[0]);

    Serial.print(" | M2[T/R/E/P]=");
    Serial.print(targetRPM[1], 1);
    Serial.print("/");
    Serial.print(measuredRPM[1], 1);
    Serial.print("/");
    Serial.print(pidError[1], 1);
    Serial.print("/");
    Serial.print(pidPWM[1]);

    Serial.print(" | M3[T/R/E/P]=");
    Serial.print(targetRPM[2], 1);
    Serial.print("/");
    Serial.print(measuredRPM[2], 1);
    Serial.print("/");
    Serial.print(pidError[2], 1);
    Serial.print("/");
    Serial.print(pidPWM[2]);

    Serial.print(" | M4[T/R/E/P]=");
    Serial.print(targetRPM[3], 1);
    Serial.print("/");
    Serial.print(measuredRPM[3], 1);
    Serial.print("/");
    Serial.print(pidError[3], 1);
    Serial.print("/");
    Serial.println(pidPWM[3]);
}

// =====================================================
// PARSE UART
//
// Packet:
// LX,LY,RX,RY
//
// Example:
// 128,128,128,128
// =====================================================

void parseCommand(String data)
{
    int start = data.indexOf(':');

    if (start >= 0)
    {
        data = data.substring(start + 1);
        data.trim();
    }

    // FIND COMMA
    int p1 = data.indexOf(',');

    int p2 = data.indexOf(
        ',',
        p1 + 1
    );

    int p3 = data.indexOf(
        ',',
        p2 + 1
    );

    // INVALID PACKET
    if (p1 < 0 || p2 < 0 || p3 < 0)
    {
        Serial.print("BAD PACKET: ");
        Serial.println(data);

        return;
    }

    // READ
    LX = data.substring(0, p1).toInt();

    LY = data.substring(
        p1 + 1,
        p2
    ).toInt();

    RX = data.substring(
        p2 + 1,
        p3
    ).toInt();

    RY = data.substring(
        p3 + 1
    ).toInt();

    // LIMIT
    LX = constrain(LX, 0, 255);
    LY = constrain(LY, 0, 255);
    RX = constrain(RX, 0, 255);
    RY = constrain(RY, 0, 255);

    // UPDATE WATCHDOG
    lastPacketTime = millis();

    // DEBUG
    Serial.print("LX=");
    Serial.print(LX);

    Serial.print(" LY=");
    Serial.print(LY);

    Serial.print(" RX=");
    Serial.print(RX);

    Serial.print(" RY=");
    Serial.println(RY);
}

// =====================================================
// SETUP
// =====================================================

void setup()
{
    // USB SERIAL
    Serial.begin(115200);

    // UART1
    // D18 = TX1
    // D19 = RX1
    Serial1.begin(115200);

    // M1
    pinMode(M1_RPWM, OUTPUT);
    pinMode(M1_LPWM, OUTPUT);
    pinMode(M1_RE, OUTPUT);
    pinMode(M1_LE, OUTPUT);

    // M2
    pinMode(M2_RPWM, OUTPUT);
    pinMode(M2_LPWM, OUTPUT);
    pinMode(M2_RE, OUTPUT);
    pinMode(M2_LE, OUTPUT);

    // M3
    pinMode(M3_RPWM, OUTPUT);
    pinMode(M3_LPWM, OUTPUT);
    pinMode(M3_RE, OUTPUT);
    pinMode(M3_LE, OUTPUT);

    // M4
    pinMode(M4_RPWM, OUTPUT);
    pinMode(M4_LPWM, OUTPUT);
    pinMode(M4_RE, OUTPUT);
    pinMode(M4_LE, OUTPUT);

    // SAFE START
    stopAll();

    // ENABLE IBT-2
    enableDrivers();

    // =================================================
    // ENCODER SETUP
    // =================================================

    pinMode(M1_ENC_A, INPUT_PULLUP);
    pinMode(M1_ENC_B, INPUT_PULLUP);

    pinMode(M2_ENC_A, INPUT_PULLUP);
    pinMode(M2_ENC_B, INPUT_PULLUP);

    pinMode(M3_ENC_A, INPUT_PULLUP);
    pinMode(M3_ENC_B, INPUT_PULLUP);

    pinMode(M4_ENC_A, INPUT_PULLUP);
    pinMode(M4_ENC_B, INPUT_PULLUP);

    attachInterrupt(
        digitalPinToInterrupt(M1_ENC_A),
        encoderISR1,
        RISING
    );

    attachInterrupt(
        digitalPinToInterrupt(M2_ENC_A),
        encoderISR2,
        RISING
    );

    attachInterrupt(
        digitalPinToInterrupt(M3_ENC_A),
        encoderISR3,
        RISING
    );

    attachInterrupt(
        digitalPinToInterrupt(M4_ENC_A),
        encoderISR4,
        RISING
    );

    // =================================================
    // INITIALIZE PID TIMERS
    // =================================================

    lastPIDTime = millis();
    lastDebugTime = millis();

    // WATCHDOG
    lastPacketTime = millis();

    // DEBUG
    Serial.println();

    Serial.println(
        "======================================"
    );

    Serial.println(
        " TR ROBOCON 2027"
    );

    Serial.println(
        " PS2 -> ESP32 -> MEGA -> IBT2"
    );

    Serial.println(
        " ENCODER + PID ENABLED"
    );

    Serial.println(
        "======================================"
    );

    Serial.println();

    Serial.println("M1 = FRONT RIGHT");
    Serial.println("M2 = FRONT LEFT");
    Serial.println("M3 = REAR LEFT");
    Serial.println("M4 = REAR RIGHT");

    Serial.println();

    Serial.println("Encoder pins:");
    Serial.println("M1 A=D2  B=D38");
    Serial.println("M2 A=D3  B=D39");
    Serial.println("M3 A=D20 B=D40");
    Serial.println("M4 A=D21 B=D41");

    Serial.println();

    Serial.println("PID debug format:");
    Serial.println("TargetRPM/RealRPM/Error/PWM");

    Serial.println();
    Serial.println("Waiting ESP32...");
}

// =====================================================
// LOOP
// =====================================================

void loop()
{
    // =================================================
    // UART1 RECEIVE
    // =================================================

    while (Serial1.available())
    {
        char c = Serial1.read();

        // END PACKET
        if (c == '\n')
        {
            if (rxLine.length() > 0)
            {
                parseCommand(rxLine);
            }

            rxLine = "";
        }

        // IGNORE CR
        else if (c != '\r')
        {
            rxLine += c;

            // SAFETY BUFFER LIMIT
            if (rxLine.length() > 100)
            {
                rxLine = "";
            }
        }
    }

    // =================================================
    // WATCHDOG
    // =================================================

    if (
        millis() - lastPacketTime >
        WATCHDOG_TIMEOUT
    )
    {
        stopAll();

        resetPID();

        return;
    }

    // =================================================
    // MECANUM TARGET
    // =================================================

    mecanumControl();

    // =================================================
    // PID CONTROL
    // =================================================

    updatePID();

    // =================================================
    // PID SERIAL DEBUG
    // =================================================

    printPIDDebug();
}