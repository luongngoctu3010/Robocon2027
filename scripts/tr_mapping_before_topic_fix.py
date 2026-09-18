import math
import rclpy
from rclpy.node import Node
from geometry_msgs.msg import Twist
from sensor_msgs.msg import LaserScan

class TRMapping(Node):

    def __init__(self):
        super().__init__("tr_mapping")

        self.cmd_pub = self.create_publisher(
            Twist,
            "/model/tr/cmd_vel",
            10
        )

        self.scan_sub = self.create_subscription(
            LaserScan,
            "/scan",
            self.scan_callback,
            10
        )

        self.scan = None
        self.state = "FORWARD"
        self.state_time = self.get_clock().now()
        self.turn_direction = 1.0

        self.timer = self.create_timer(
            0.05,
            self.control
        )

    def scan_callback(self, msg):
        self.scan = msg

    def sector_min(self, start_deg, end_deg):

        if self.scan is None:
            return float("inf")

        values = []

        start = int(
            (math.radians(start_deg) - self.scan.angle_min)
            / self.scan.angle_increment
        )

        end = int(
            (math.radians(end_deg) - self.scan.angle_min)
            / self.scan.angle_increment
        )

        for i in range(start, end + 1):

            if i < 0 or i >= len(self.scan.ranges):
                continue

            r = self.scan.ranges[i]

            if math.isfinite(r):
                values.append(r)

        if not values:
            return float("inf")

        return min(values)

    def publish(self, vx, wz):

        msg = Twist()

        msg.linear.x = vx
        msg.linear.y = 0.0
        msg.linear.z = 0.0

        msg.angular.x = 0.0
        msg.angular.y = 0.0
        msg.angular.z = wz

        self.cmd_pub.publish(msg)

    def elapsed(self):

        now = self.get_clock().now()

        return (
            now - self.state_time
        ).nanoseconds / 1e9

    def set_state(self, state):

        self.state = state

        self.state_time = self.get_clock().now()

    def control(self):

        if self.scan is None:
            self.publish(0.0, 0.0)
            return

        front = self.sector_min(-20, 20)
        left = self.sector_min(30, 80)
        right = self.sector_min(-80, -30)

        if self.state == "FORWARD":

            if front < 0.70:

                self.publish(0.0, 0.0)

                if left > right:
                    self.turn_direction = 1.0
                else:
                    self.turn_direction = -1.0

                self.set_state("TURN")

            else:

                self.publish(0.25, 0.0)

        elif self.state == "TURN":

            if self.elapsed() < 1.5:

                self.publish(
                    0.0,
                    0.8 * self.turn_direction
                )

            else:

                self.set_state("BYPASS")

        elif self.state == "BYPASS":

            front = self.sector_min(-25, 25)

            if front < 0.65:

                self.publish(
                    0.0,
                    0.6 * self.turn_direction
                )

            else:

                self.publish(
                    0.25,
                    0.0
                )

                if self.elapsed() > 2.0:

                    self.set_state("FORWARD")

def main():

    rclpy.init()

    node = TRMapping()

    try:
        rclpy.spin(node)

    except KeyboardInterrupt:
        pass

    node.publish(0.0, 0.0)

    node.destroy_node()

    rclpy.shutdown()

if __name__ == "__main__":
    main()
