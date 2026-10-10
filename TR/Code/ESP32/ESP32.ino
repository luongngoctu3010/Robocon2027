/*
  KIEM TRA 4 ENCODER JGB37-545
  Arduino Mega 2560

  Encoder A: D2, D3, D20, D21
  Encoder B: D38, D39, D40, D41

  Do xung khi quay banh bang tay.
*/

const byte ENC_A[4] = {2, 3, 20, 21};
const byte ENC_B[4] = {38, 39, 40, 41};

volatile long encoderCount[4] = {0, 0, 0, 0};

void isrM1() {
  if (digitalRead(ENC_B[0]))
    encoderCount[0]++;
  else
    encoderCount[0]--;
}

void isrM2() {
  if (digitalRead(ENC_B[1]))
    encoderCount[1]++;
  else
    encoderCount[1]--;
}

void isrM3() {
  if (digitalRead(ENC_B[2]))
    encoderCount[2]++;
  else
    encoderCount[2]--;
}

void isrM4() {
  if (digitalRead(ENC_B[3]))
    encoderCount[3]++;
  else
    encoderCount[3]--;
}

void setup() {
  Serial.begin(115200);

  for (byte i = 0; i < 4; i++) {
    pinMode(ENC_A[i], INPUT_PULLUP);
    pinMode(ENC_B[i], INPUT_PULLUP);
  }

  attachInterrupt(digitalPinToInterrupt(2), isrM1, CHANGE);
  attachInterrupt(digitalPinToInterrupt(3), isrM2, CHANGE);
  attachInterrupt(digitalPinToInterrupt(20), isrM3, CHANGE);
  attachInterrupt(digitalPinToInterrupt(21), isrM4, CHANGE);

  Serial.println(F("KIEM TRA 4 ENCODER JGB37-545"));
  Serial.println(F("Quay tung banh xe bang tay."));
}

void loop() {
  static unsigned long lastPrint = 0;

  if (millis() - lastPrint >= 200) {
    lastPrint = millis();

    long count[4];

    noInterrupts();
    for (byte i = 0; i < 4; i++) {
      count[i] = encoderCount[i];
    }
    interrupts();

    Serial.print(F("M1: "));
    Serial.print(count[0]);

    Serial.print(F("\tM2: "));
    Serial.print(count[1]);

    Serial.print(F("\tM3: "));
    Serial.print(count[2]);

    Serial.print(F("\tM4: "));
    Serial.println(count[3]);
  }
}