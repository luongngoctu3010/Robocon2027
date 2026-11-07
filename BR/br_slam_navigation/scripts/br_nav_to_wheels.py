#!/usr/bin/env python3

import math

import rclpy
from rclpy.node import Node
from geometry_msgs.msg import Twist
from std_msgs.msg import Float64


class BRNavToWheels(Node):
    def __init__(self):
        super().__init__('br_nav_to_wheels')

        # Thông số robot BR
        self.wheel_radius = 0.08         # Bán kính bánh xe (m)
        self.wheel_separation = 0.66     # Khoảng cách 2 vệt bánh (m)
        self.max_wheel_speed = 2.0       # Tốc độ góc tối đa của bánh (rad/s)

        self.last_cmd_time = self.get_clock().now()
        self.watchdog_timeout = 0.5      # Thời gian chờ tối đa (giây)
        self.is_stopped = False          # Cờ đánh dấu đã dừng để tránh spam topic

        # 4 topic điều khiển bánh xe
        self.wheel_publishers = {
            'left_front': self.create_publisher(
                Float64, '/model/BR/joint/left_front_joint/cmd_vel', 10),
            'left_rear': self.create_publisher(
                Float64, '/model/BR/joint/left_rear_joint/cmd_vel', 10),
            'right_front': self.create_publisher(
                Float64, '/model/BR/joint/right_front_joint/cmd_vel', 10),
            'right_rear': self.create_publisher(
                Float64, '/model/BR/joint/right_rear_joint/cmd_vel', 10),
        }

        self.subscription = self.create_subscription(
            Twist, '/cmd_vel', self.cmd_vel_callback, 10)

        # Watchdog kiểm tra định kỳ 10Hz
        self.watchdog = self.create_timer(0.1, self.watchdog_callback)

        self.get_logger().info('BR Nav2 -> 4 wheel controller started')
        self.get_logger().info('Subscribed to /cmd_vel')

    def publish_wheels(self, left: float, right: float):
        """Gửi vận tốc góc (rad/s) tới 4 bánh xe."""
        commands = {
            'left_front': left,
            'left_rear': left,
            'right_front': right,
            'right_rear': right,
        }

        for name, speed in commands.items():
            msg = Float64()
            msg.data = float(speed)
            self.wheel_publishers[name].publish(msg)

    def cmd_vel_callback(self, msg: Twist):
        self.last_cmd_time = self.get_clock().now()
        self.is_stopped = False

        v = msg.linear.x
        w = msg.angular.z

        # Tính toán vận tốc góc 2 bên bánh (Differential Drive Model)
        left = (v - w * self.wheel_separation / 2.0) / self.wheel_radius
        right = (v + w * self.wheel_separation / 2.0) / self.wheel_radius

        # Giữ nguyên tỷ lệ góc quay nếu 1 trong 2 bên vượt quá tốc độ tối đa
        peak = max(abs(left), abs(right))
        if peak > self.max_wheel_speed:
            scale = self.max_wheel_speed / peak
            left *= scale
            right *= scale

        self.publish_wheels(left, right)

    def watchdog_callback(self):
        """Nếu quá timeout không nhận được cmd_vel, phát lệnh dừng xe."""
        if self.is_stopped:
            return

        time_since_last_cmd = (
            self.get_clock().now() - self.last_cmd_time
        ).nanoseconds / 1e9

        if time_since_last_cmd > self.watchdog_timeout:
            self.publish_wheels(0.0, 0.0)
            self.is_stopped = True
            # ĐÃ SỬA: Đổi .warn thành .warning
            self.get_logger().warning(
                f'Watchdog triggered: No /cmd_vel received for {time_since_last_cmd:.2f}s. Vehicle stopped.'
            )
def main(args=None):
    rclpy.init(args=args)
    node = BRNavToWheels()

    try:
        rclpy.spin(node)
    except KeyboardInterrupt:
        pass
    finally:
        # Đảm bảo dừng bánh xe an toàn trước khi hủy node
        node.publish_wheels(0.0, 0.0)
        node.destroy_node()
        if rclpy.ok():
            rclpy.shutdown()


if __name__ == '__main__':
    main()
