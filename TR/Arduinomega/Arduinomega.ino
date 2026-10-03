// =====================================================
// TR ROBOCON 2027
// PS2 -> Ubuntu -> WiFi -> ESP32 -> UART -> MEGA
// MEGA -> 4 IBT-2 -> 4 MECANUM MOTOR
// =====================================================


// =====================================================
// MOTOR 1
// =====================================================

#define M1_RPWM 5
#define M1_LPWM 6
#define M1_RE   22
#define M1_LE   23


// =====================================================
// MOTOR 2
// =====================================================

#define M2_RPWM 7
#define M2_LPWM 8
#define M2_RE   24
#define M2_LE   25


// =====================================================
// MOTOR 3
// =====================================================

#define M3_RPWM 9
#define M3_LPWM 10
#define M3_RE   26
#define M3_LE   27


// =====================================================
// MOTOR 4
// =====================================================

#define M4_RPWM 11
#define M4_LPWM 12
#define M4_RE   28
#define M4_LE   29


// =====================================================
// PS2 VALUES
// =====================================================

int LX = 128;
int LY = 128;
int RX = 128;
int RY = 128;


// =====================================================
// UART
// =====================================================

String rxLine = "";


// =====================================================
// WATCHDOG
// =====================================================

unsigned long lastPacketTime = 0;

const unsigned long WATCHDOG_TIMEOUT = 300;


// =====================================================
// DEADZONE
// =====================================================

const int DEADZONE = 15;


// =====================================================
// MAX SPEED
// =====================================================

// Giảm xuống 80 để test an toàn
const int MAX_SPEED = 80;


// =====================================================
// MOTOR 1
// =====================================================

void motor1(int speed)
{
    speed = constrain(
        speed,
        -255,
        255
    );

    if (speed > 0)
    {
        analogWrite(
            M1_RPWM,
            speed
        );

        analogWrite(
            M1_LPWM,
            0
        );
    }
    else
    {
        analogWrite(
            M1_RPWM,
            0
        );

        analogWrite(
            M1_LPWM,
            -speed
        );
    }
}


// =====================================================
// MOTOR 2
// =====================================================

void motor2(int speed)
{
    speed = constrain(
        speed,
        -255,
        255
    );

    if (speed > 0)
    {
        analogWrite(
            M2_RPWM,
            speed
        );

        analogWrite(
            M2_LPWM,
            0
        );
    }
    else
    {
        analogWrite(
            M2_RPWM,
            0
        );

        analogWrite(
            M2_LPWM,
            -speed
        );
    }
}


// =====================================================
// MOTOR 3
// =====================================================

void motor3(int speed)
{
    speed = constrain(
        speed,
        -255,
        255
    );

    if (speed > 0)
    {
        analogWrite(
            M3_RPWM,
            speed
        );

        analogWrite(
            M3_LPWM,
            0
        );
    }
    else
    {
        analogWrite(
            M3_RPWM,
            0
        );

        analogWrite(
            M3_LPWM,
            -speed
        );
    }
}


// =====================================================
// MOTOR 4
// =====================================================

void motor4(int speed)
{
    speed = constrain(
        speed,
        -255,
        255
    );

    if (speed > 0)
    {
        analogWrite(
            M4_RPWM,
            speed
        );

        analogWrite(
            M4_LPWM,
            0
        );
    }
    else
    {
        analogWrite(
            M4_RPWM,
            0
        );

        analogWrite(
            M4_LPWM,
            -speed
        );
    }
}


// =====================================================
// STOP
// =====================================================

void stopAll()
{
    motor1(0);
    motor2(0);
    motor3(0);
    motor4(0);
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
// AXIS TO SPEED
// =====================================================

int axisToSpeed(int value)
{
    int speed =
        value - 128;


    // Deadzone

    if (abs(speed) < DEADZONE)
    {
        speed = 0;
    }


    // Convert
    // -127 ... +127
    //
    // to
    //
    // -MAX_SPEED ... +MAX_SPEED

    speed =
        map(
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
// MECANUM
// =====================================================

void mecanumControl()
{
    // -----------------------------
    // LEFT / RIGHT
    // -----------------------------

    int Vx =
        axisToSpeed(LX);


    // -----------------------------
    // FORWARD / BACKWARD
    // -----------------------------

    int Vy =
        -axisToSpeed(LY);


    // -----------------------------
    // ROTATION
    // -----------------------------

    int W =
        axisToSpeed(RX);


    // -----------------------------
    // MECANUM EQUATIONS
    // -----------------------------

    int M1 =
        Vy
        + Vx
        + W;


    int M2 =
        Vy
        - Vx
        - W;


    int M3 =
        Vy
        - Vx
        + W;


    int M4 =
        Vy
        + Vx
        - W;


    // -----------------------------
    // NORMALIZE
    // -----------------------------

    int maxValue =
        abs(M1);


    if (abs(M2) > maxValue)
        maxValue = abs(M2);

    if (abs(M3) > maxValue)
        maxValue = abs(M3);

    if (abs(M4) > maxValue)
        maxValue = abs(M4);


    if (maxValue > MAX_SPEED)
    {
        M1 =
            M1 * MAX_SPEED /
            maxValue;

        M2 =
            M2 * MAX_SPEED /
            maxValue;

        M3 =
            M3 * MAX_SPEED /
            maxValue;

        M4 =
            M4 * MAX_SPEED /
            maxValue;
    }


    // -----------------------------
    // OUTPUT
    // -----------------------------

    motor1(M1);
    motor2(M2);
    motor3(M3);
    motor4(M4);
}


// =====================================================
// PARSE UDP/UART COMMAND
// =====================================================

void parseCommand(
    String data
)
{
    int p1 =
        data.indexOf(',');


    int p2 =
        data.indexOf(
            ',',
            p1 + 1
        );


    int p3 =
        data.indexOf(
            ',',
            p2 + 1
        );


    if (
        p1 < 0 ||
        p2 < 0 ||
        p3 < 0
    )
    {
        return;
    }


    LX =
        data.substring(
            0,
            p1
        ).toInt();


    LY =
        data.substring(
            p1 + 1,
            p2
        ).toInt();


    RX =
        data.substring(
            p2 + 1,
            p3
        ).toInt();


    RY =
        data.substring(
            p3 + 1
        ).toInt();


    LX =
        constrain(
            LX,
            0,
            255
        );


    LY =
        constrain(
            LY,
            0,
            255
        );


    RX =
        constrain(
            RX,
            0,
            255
        );


    RY =
        constrain(
            RY,
            0,
            255
        );


    lastPacketTime =
        millis();
}


// =====================================================
// SETUP
// =====================================================

void setup()
{
    Serial.begin(115200);

    Serial1.begin(115200);


    // -----------------------------
    // M1
    // -----------------------------

    pinMode(M1_RPWM, OUTPUT);
    pinMode(M1_LPWM, OUTPUT);

    pinMode(M1_RE, OUTPUT);
    pinMode(M1_LE, OUTPUT);


    // -----------------------------
    // M2
    // -----------------------------

    pinMode(M2_RPWM, OUTPUT);
    pinMode(M2_LPWM, OUTPUT);

    pinMode(M2_RE, OUTPUT);
    pinMode(M2_LE, OUTPUT);


    // -----------------------------
    // M3
    // -----------------------------

    pinMode(M3_RPWM, OUTPUT);
    pinMode(M3_LPWM, OUTPUT);

    pinMode(M3_RE, OUTPUT);
    pinMode(M3_LE, OUTPUT);


    // -----------------------------
    // M4
    // -----------------------------

    pinMode(M4_RPWM, OUTPUT);
    pinMode(M4_LPWM, OUTPUT);

    pinMode(M4_RE, OUTPUT);
    pinMode(M4_LE, OUTPUT);


    // -----------------------------
    // STOP
    // -----------------------------

    stopAll();


    // -----------------------------
    // ENABLE
    // -----------------------------

    enableDrivers();


    // -----------------------------
    // WATCHDOG
    // -----------------------------

    lastPacketTime =
        millis();


    // -----------------------------
    // START MESSAGE
    // -----------------------------

    Serial.println();
    Serial.println(
        "===================================="
    );

    Serial.println(
        " TR PS2 MECANUM CONTROL"
    );

    Serial.println(
        "===================================="
    );

    Serial.println(
        "Waiting PS2..."
    );
}


// =====================================================
// LOOP
// =====================================================

void loop()
{
    // =================================================
    // RECEIVE ESP32
    // =================================================

    while (
        Serial1.available()
    )
    {
        char c =
            Serial1.read();


        if (c == '\n')
        {
            parseCommand(
                rxLine
            );

            rxLine = "";
        }
        else if (c != '\r')
        {
            rxLine += c;
        }
    }


    // =================================================
    // WATCHDOG
    // =================================================

    if (
        millis() -
        lastPacketTime
        >
        WATCHDOG_TIMEOUT
    )
    {
        stopAll();

        return;
    }


    // =================================================
    // MECANUM
    // =================================================

    mecanumControl();
}