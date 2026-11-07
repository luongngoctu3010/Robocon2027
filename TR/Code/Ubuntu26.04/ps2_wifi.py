#!/usr/bin/env python3

import pygame
import socket
import time
import signal
import sys

# ============================================================
# ESP32 WIFI CONFIG
# ============================================================

ESP32_IP = "192.168.1.100"
ESP32_PORT = 5000

# Tần số gửi dữ liệu
SEND_HZ = 50
SEND_PERIOD = 1.0 / SEND_HZ

# Deadzone joystick
DEADZONE = 0.08

# ============================================================
# PS2 AXIS
# ============================================================

AXIS_LX = 0
AXIS_LY = 1
AXIS_RX = 2
AXIS_RY = 3

# ============================================================
# INIT PYGAME
# ============================================================

pygame.init()
pygame.joystick.init()

print()
print("==============================================")
print("      PS2 -> UBUNTU -> ESP32 UDP")
print("==============================================")

# ------------------------------------------------------------
# CHECK PS2
# ------------------------------------------------------------

num_joysticks = pygame.joystick.get_count()

print("Joystick found:", num_joysticks)

if num_joysticks == 0:

    print()
    print("KHONG TIM THAY PS2!")
    print()
    print("Kiem tra:")
    print("  ls /dev/input/js*")
    print("  lsusb")
    print("  bluetoothctl")
    print()

    pygame.quit()
    sys.exit(1)

# ------------------------------------------------------------
# SELECT PS2
# ------------------------------------------------------------

joystick = pygame.joystick.Joystick(0)
joystick.init()

print()
print("PS2 CONNECTED")
print("----------------------------------------------")
print("Name    :", joystick.get_name())
print("Axes    :", joystick.get_numaxes())
print("Buttons :", joystick.get_numbuttons())
print("Hats    :", joystick.get_numhats())
print("----------------------------------------------")

# ============================================================
# UDP SOCKET
# ============================================================

sock = socket.socket(
    socket.AF_INET,
    socket.SOCK_DGRAM
)

# ============================================================
# PRINT CONFIG
# ============================================================

print()
print("ESP32 IP   :", ESP32_IP)
print("ESP32 PORT :", ESP32_PORT)
print("SEND RATE  :", SEND_HZ, "Hz")
print()

# ============================================================
# DEADZONE
# ============================================================

def apply_deadzone(value):

    if abs(value) < DEADZONE:
        return 0.0

    sign = 1.0 if value >= 0 else -1.0

    value = (
        abs(value) - DEADZONE
    ) / (
        1.0 - DEADZONE
    )

    return sign * value


# ============================================================
# AXIS -1...+1 -> 0...255
# ============================================================

def axis_to_255(value):

    value = apply_deadzone(value)

    result = int(
        (value + 1.0) * 127.5
    )

    result = max(
        0,
        min(255, result)
    )

    return result


# ============================================================
# READ BUTTONS
# ============================================================

def get_buttons():

    buttons = 0

    for i in range(
        joystick.get_numbuttons()
    ):

        if joystick.get_button(i):

            buttons |= (
                1 << i
            )

    return buttons


# ============================================================
# SEND STOP
# ============================================================

def send_stop():

    packet = (
        "127,127,127,127,0\n"
    )

    try:

        sock.sendto(
            packet.encode(),
            (
                ESP32_IP,
                ESP32_PORT
            )
        )

        print(
            "STOP -> ESP32"
        )

    except Exception as e:

        print(
            "STOP ERROR:",
            e
        )


# ============================================================
# CTRL+C
# ============================================================

def signal_handler(
    sig,
    frame
):

    print()
    print(
        "Ctrl+C -> STOP ROBOT"
    )

    send_stop()

    time.sleep(0.1)

    pygame.quit()
    sock.close()

    print(
        "Program stopped."
    )

    sys.exit(0)


signal.signal(
    signal.SIGINT,
    signal_handler
)

# ============================================================
# START
# ============================================================

print("==============================================")
print("        START UDP CONTROL")
print("==============================================")
print()
print("Dang gui du lieu PS2...")
print("Nhan Ctrl+C de dung.")
print()

# ============================================================
# MAIN LOOP
# ============================================================

next_time = time.monotonic()

last_packet = None

while True:

    # --------------------------------------------------------
    # PYGAME EVENTS
    # --------------------------------------------------------

    pygame.event.pump()

    # --------------------------------------------------------
    # READ AXES
    # --------------------------------------------------------

    try:

        lx_raw = joystick.get_axis(
            AXIS_LX
        )

        ly_raw = joystick.get_axis(
            AXIS_LY
        )

        rx_raw = joystick.get_axis(
            AXIS_RX
        )

        ry_raw = joystick.get_axis(
            AXIS_RY
        )

    except Exception as e:

        print(
            "PS2 READ ERROR:",
            e
        )

        send_stop()

        time.sleep(0.1)

        continue

    # --------------------------------------------------------
    # CONVERT AXES
    # --------------------------------------------------------

    lx = axis_to_255(
        lx_raw
    )

    ly = axis_to_255(
        ly_raw
    )

    rx = axis_to_255(
        rx_raw
    )

    ry = axis_to_255(
        ry_raw
    )

    # --------------------------------------------------------
    # BUTTONS
    # --------------------------------------------------------

    buttons = get_buttons()

    # --------------------------------------------------------
    # PACKET
    # --------------------------------------------------------

    packet = (
        f"{lx},"
        f"{ly},"
        f"{rx},"
        f"{ry},"
        f"{buttons}\n"
    )

    # --------------------------------------------------------
    # SEND UDP
    # --------------------------------------------------------

    try:

        sock.sendto(
            packet.encode(),
            (
                ESP32_IP,
                ESP32_PORT
            )
        )

    except Exception as e:

        print(
            "UDP ERROR:",
            e
        )

    # --------------------------------------------------------
    # DEBUG
    # Chỉ in khi packet thay đổi
    # --------------------------------------------------------

    if packet != last_packet:

        print(
            "PS2:",
            packet.strip()
        )

        last_packet = packet

    # --------------------------------------------------------
    # 50 Hz
    # --------------------------------------------------------

    next_time += SEND_PERIOD

    sleep_time = (
        next_time
        - time.monotonic()
    )

    if sleep_time > 0:

        time.sleep(
            sleep_time
        )

    else:

        next_time = time.monotonic()
