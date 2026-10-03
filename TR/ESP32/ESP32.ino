#include <WiFi.h>
#include <WiFiUdp.h>

const char* ssid = "FABLAB821823M";
const char* password = "codientu";

WiFiUDP udp;

const int UDP_PORT = 5005;

#define RXD2 16
#define TXD2 17

void setup()
{
    Serial.begin(115200);

    Serial2.begin(
        115200,
        SERIAL_8N1,
        RXD2,
        TXD2
    );

    WiFi.begin(
        ssid,
        password
    );

    Serial.print("Connecting WiFi");

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }

    Serial.println();

    Serial.println("WiFi connected");

    Serial.print("ESP32 IP: ");
    Serial.println(WiFi.localIP());

    udp.begin(UDP_PORT);

    Serial.print("UDP port: ");
    Serial.println(UDP_PORT);
}

void loop()
{
    int packetSize = udp.parsePacket();

    if (packetSize > 0)
    {
        char buffer[128];

        int len = udp.read(
            buffer,
            sizeof(buffer) - 1
        );

        if (len > 0)
        {
            buffer[len] = '\0';

            Serial.print("RX UDP: ");
            Serial.println(buffer);

            Serial2.print(buffer);

            if (buffer[len - 1] != '\n')
            {
                Serial2.print('\n');
            }
        }
    }

    while (Serial2.available())
    {
        char c = Serial2.read();

        Serial.write(c);
    }
}