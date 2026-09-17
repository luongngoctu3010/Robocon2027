#!/usr/bin/env python3

import rclpy
from rclpy.node import Node
from geometry_msgs.msg import Twist
from std_msgs.msg import Float64MultiArray


class MecanumController(Node):

    def __init__(self):
        super().__init__('mecanum_controller')

        self.wheel_radius = 0.075
        self.wheel_x = 0.25
        self.wheel_y = 0.25

        self.cmd_sub = self.create_subscription(
            Twist,
            '/cmd_vel',
            self.cmd_vel_callback,
            10
        )

        self.wheel_pub = self.create_publisher(
            Float64MultiArray,
            '/tr_wheel_velocity',
            10
        )

        self.get_logger().info('Mecanum controller started')

    def cmd_vel_callback(self, msg):
        vx = msg.linear.x
        vy = msg.linear.y
        wz = msg.angular.z

        k = self.wheel_x + self.wheel_y
        r = self.wheel_radius

        fl = (vx - vy - k * wz) / r
        fr = (vx + vy + k * wz) / r
        rl = (vx + vy - k * wz) / r
        rr = (vx - vy + k * wz) / r

        output = Float64MultiArray()
        output.data = [fl, fr, rl, rr]

        self.wheel_pub.publish(output)


def main(args=None):
    rclpy.init(args=args)

    node = MecanumController()

    try:
        rclpy.spin(node)
    except KeyboardInterrupt:
        pass

    node.destroy_node()
    rclpy.shutdown()


if __name__ == '__main__':
    main()
