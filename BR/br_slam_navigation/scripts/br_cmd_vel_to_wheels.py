#!/usr/bin/env python3

import time

import rclpy
from rclpy.node import Node
from geometry_msgs.msg import Twist
from std_msgs.msg import Float64


class BRCmdVelToWheels(Node):
    def __init__(self):
        super().__init__('br_cmd_vel_to_wheels')

        # Thong so lay tu model.sdf
        self.wheel_radius = 0.08       # m
        self.wheel_separation = 0.66   # m

        # Truc banh trong model.sdf (roll 90 do, truc 0 0 1) huong ve -y cua robot,
        # nen van toc banh duong lam robot chay lui. Dao dau de +linear.x = tien.
        self.wheel_sign = -1.0

        # Gioi han an toan khi thu nghiem
        self.max_linear = 0.25         # m/s
        self.max_angular = 0.5         # rad/s

        self.last_cmd_time = time.monotonic()

        self.wheel_topics = {
            'left_front': '/model/BR/joint/left_front_joint/cmd_vel',
            'left_rear':  '/model/BR/joint/left_rear_joint/cmd_vel',
            'right_front': '/model/BR/joint/right_front_joint/cmd_vel',
            'right_rear':  '/model/BR/joint/right_rear_joint/cmd_vel',
        }

        self.publishers_wheel = {
            name: self.create_publisher(Float64, topic, 10)
            for name, topic in self.wheel_topics.items()
        }

        self.sub = self.create_subscription(
            Twist, '/cmd_vel', self.cmd_callback, 10
        )

        # Neu mat lenh qua 0.5 giay thi dung banh
        self.watchdog = self.create_timer(0.1, self.watchdog_callback)

        self.get_logger().info(
            'BR cmd_vel -> 4 wheels ready. '
            'Wheel radius=0.08 m, separation=0.66 m'
        )

    def publish_wheels(self, left, right):
        values = {
            'left_front': left,
            'left_rear': left,
            'right_front': right,
            'right_rear': right,
        }

        for name, value in values.items():
            msg = Float64()
            msg.data = float(self.wheel_sign * value)
            self.publishers_wheel[name].publish(msg)

    def cmd_callback(self, msg):
        self.get_logger().info(f'Received msg: {msg}')

        self.last_cmd_time = time.monotonic()

        v = max(-self.max_linear, min(self.max_linear, msg.linear.x))
        wz = max(-self.max_angular, min(self.max_angular, msg.angular.z))

        half_track = self.wheel_separation / 2.0

        # Mo hinh vi sai: tinh van toc goc banh xe (rad/s)
        left = (v - wz * half_track) / self.wheel_radius
        right = (v + wz * half_track) / self.wheel_radius

        self.publish_wheels(left, right)

        self.get_logger().info(
            f'cmd_vel: v={v:.2f}, wz={wz:.2f} | '
            f'left={left:.2f}, right={right:.2f} rad/s'
        )

    def watchdog_callback(self):
        if time.monotonic() - self.last_cmd_time > 0.5:
            self.publish_wheels(0.0, 0.0)


def main():
    rclpy.init()
    node = BRCmdVelToWheels()

    try:
        rclpy.spin(node)
    except KeyboardInterrupt:
        pass
    finally:
        node.publish_wheels(0.0, 0.0)
        node.destroy_node()
        rclpy.shutdown()


if __name__ == '__main__':
    main()
