#!/usr/bin/env python3

import subprocess
import sys
import termios
import tty
import select

# =========================
# CẤU HÌNH
# =========================

SPEED = 3.0

JOINTS = {
    "LF": "/model/BR/joint/left_front_joint/cmd_vel",
    "LR": "/model/BR/joint/left_rear_joint/cmd_vel",
    "RF": "/model/BR/joint/right_front_joint/cmd_vel",
    "RR": "/model/BR/joint/right_rear_joint/cmd_vel",
}


# =========================
# GỬI VẬN TỐC CHO JOINT
# =========================

def set_joint(topic, value):
    subprocess.run(
        [
            "gz",
            "topic",
            "-t",
            topic,
            "-m",
            "gz.msgs.Double",
            "-p",
            f"data: {value}"
        ],
        stdout=subprocess.DEVNULL,
        stderr=subprocess.DEVNULL
    )


def set_wheels(lf, lr, rf, rr):

    set_joint(JOINTS["LF"], lf)
    set_joint(JOINTS["LR"], lr)
    set_joint(JOINTS["RF"], rf)
    set_joint(JOINTS["RR"], rr)


# =========================
# DỪNG
# =========================

def stop():
    set_wheels(0, 0, 0, 0)


# =========================
# ĐIỀU KHIỂN
# =========================

def forward():
    # Tiến
    set_wheels(
        SPEED,
        SPEED,
        SPEED,
        SPEED
    )


def backward():
    # Lùi
    set_wheels(
        -SPEED,
        -SPEED,
        -SPEED,
        -SPEED
    )


def left():
    # Sang trái
    set_wheels(
        -SPEED,
        SPEED,
        SPEED,
        -SPEED
    )


def right():
    # Sang phải
    set_wheels(
        SPEED,
        -SPEED,
        -SPEED,
        SPEED
    )


def rotate_left():
    # Xoay trái
    set_wheels(
        -SPEED,
        -SPEED,
        SPEED,
        SPEED
    )


def rotate_right():
    # Xoay phải
    set_wheels(
        SPEED,
        SPEED,
        -SPEED,
        -SPEED
    )


# =========================
# ĐỌC BÀN PHÍM
# =========================

def get_key():

    fd = sys.stdin.fileno()

    old_settings = termios.tcgetattr(fd)

    try:

        tty.setraw(fd)

        key = sys.stdin.read(1)

    finally:

        termios.tcsetattr(
            fd,
            termios.TCSADRAIN,
            old_settings
        )

    return key


# =========================
# MAIN
# =========================

def main():

    print()
    print("================================")
    print("     BR GAZEBO KEYBOARD")
    print("================================")
    print()
    print("W : TIEN")
    print("S : LUI")
    print("A : SANG TRAI")
    print("D : SANG PHAI")
    print("Q : XOAY TRAI")
    print("E : XOAY PHAI")
    print("X : DUNG")
    print("SPACE : DUNG")
    print("ESC : THOAT")
    print()
    print("Speed =", SPEED)
    print()

    stop()

    try:

        while True:

            key = get_key().lower()

            if key == "w":

                print("TIEN")
                forward()

            elif key == "s":

                print("LUI")
                backward()

            elif key == "a":

                print("SANG TRAI")
                left()

            elif key == "d":

                print("SANG PHAI")
                right()

            elif key == "q":

                print("XOAY TRAI")
                rotate_left()

            elif key == "e":

                print("XOAY PHAI")
                rotate_right()

            elif key == "x" or key == " ":

                print("DUNG")
                stop()

            elif ord(key) == 27:

                print("THOAT")
                stop()
                break

    except KeyboardInterrupt:

        stop()

    finally:

        stop()


if __name__ == "__main__":
    main()
