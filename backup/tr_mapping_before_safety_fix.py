import math
import rclpy
from rclpy.node import Node
from geometry_msgs.msg import Twist
from sensor_msgs.msg import LaserScan
from nav_msgs.msg import Odometry


class TRMapping(Node):

    def __init__(self):
        super().__init__("tr_mapping")

        self.cmd_pub = self.create_publisher(
            Twist,
            "/cmd_vel",
            10
        )

        self.scan_sub = self.create_subscription(
            LaserScan,
            "/scan",
            self.scan_callback,
            10
        )

        self.odom_sub = self.create_subscription(
            Odometry,
            "/model/tr/odometry",
            self.odom_callback,
            10
        )

        self.scan = None

        self.x = -4.0
        self.y = -4.0
        self.yaw = 0.0

        self.home_x = -4.0
        self.home_y = -4.0

        self.state = "MAPPING"
        self.state_time = self.get_clock().now()

        self.turn_direction = 1.0

        self.field_limit = 5.0
        self.edge_distance = 0.45

        self.obstacle_distance = 0.70
        self.bypass_distance = 0.65

        self.mapping_time = 120.0
        self.return_distance = 0.18

        self.timer = self.create_timer(
            0.05,
            self.control
        )

    def scan_callback(self, msg):
        self.scan = msg

    def odom_callback(self, msg):
        self.x = msg.pose.pose.position.x
        self.y = msg.pose.pose.position.y

        q = msg.pose.pose.orientation

        siny_cosp = 2.0 * (
            q.w * q.z + q.x * q.y
        )

        cosy_cosp = 1.0 - 2.0 * (
            q.y * q.y + q.z * q.z
        )

        self.yaw = math.atan2(
            siny_cosp,
            cosy_cosp
        )

    def sector_min(self, start_deg, end_deg):

        if self.scan is None:
            return float("inf")

        values = []

        start = int(
            (
                math.radians(start_deg)
                - self.scan.angle_min
            )
            / self.scan.angle_increment
        )

        end = int(
            (
                math.radians(end_deg)
                - self.scan.angle_min
            )
            / self.scan.angle_increment
        )

        start = max(0, start)
        end = min(
            len(self.scan.ranges) - 1,
            end
        )

        for i in range(start, end + 1):

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

    def distance_to_home(self):

        dx = self.home_x - self.x
        dy = self.home_y - self.y

        return math.sqrt(
            dx * dx + dy * dy
        )

    def normalize_angle(self, angle):

        while angle > math.pi:
            angle -= 2.0 * math.pi

        while angle < -math.pi:
            angle += 2.0 * math.pi

        return angle

    def edge_detected(self):

        if self.scan is None:
            return True

        front = self.sector_min(-15, 15)
        left = self.sector_min(70, 110)
        right = self.sector_min(-110, -70)

        if not math.isfinite(front):
            front = self.scan.range_max

        if not math.isfinite(left):
            left = self.scan.range_max

        if not math.isfinite(right):
            right = self.scan.range_max

        near_left_edge = left > 2.0
        near_right_edge = right > 2.0

        x_edge = (
            self.x < -self.field_limit
            or self.x > self.field_limit
        )

        y_edge = (
            self.y < -self.field_limit
            or self.y > self.field_limit
        )

        if x_edge or y_edge:
            return True

        if (
            front > 2.0
            and (
                near_left_edge
                or near_right_edge
            )
        ):
            return True

        return False

    def choose_turn_direction(self):

        left = self.sector_min(30, 80)
        right = self.sector_min(-80, -30)

        if not math.isfinite(left):
            left = self.scan.range_max

        if not math.isfinite(right):
            right = self.scan.range_max

        if left > right:
            self.turn_direction = 1.0
        else:
            self.turn_direction = -1.0

    def go_home(self):

        dx = self.home_x - self.x
        dy = self.home_y - self.y

        distance = math.sqrt(
            dx * dx + dy * dy
        )

        if distance < self.return_distance:

            self.publish(0.0, 0.0)
            self.set_state("STOP")
            return

        target_yaw = math.atan2(
            dy,
            dx
        )

        yaw_error = self.normalize_angle(
            target_yaw - self.yaw
        )

        front = self.sector_min(-20, 20)

        if not math.isfinite(front):
            front = self.scan.range_max

        if front < 0.60:

            self.choose_turn_direction()

            self.publish(
                0.0,
                0.6 * self.turn_direction
            )

            return

        if abs(yaw_error) > 0.20:

            wz = 0.8

            if yaw_error < 0.0:
                wz = -0.8

            self.publish(
                0.0,
                wz
            )

            return

        speed = 0.25

        if distance < 0.60:
            speed = 0.15

        self.publish(
            speed,
            0.0
        )

    def control(self):

        if self.state == "STOP":

            self.publish(0.0, 0.0)
            return

        if self.scan is None:

            self.publish(0.0, 0.0)
            return

        if self.state == "RETURN_HOME":

            self.go_home()
            return

        if self.state == "EDGE_TURN":

            if self.elapsed() < 1.8:

                self.publish(
                    0.0,
                    0.8 * self.turn_direction
                )

            else:

                self.set_state("MAPPING")

            return

        if self.state == "OBSTACLE_TURN":

            if self.elapsed() < 1.4:

                self.publish(
                    0.0,
                    0.8 * self.turn_direction
                )

            else:

                self.set_state("OBSTACLE_BYPASS")

            return

        if self.state == "OBSTACLE_BYPASS":

            front = self.sector_min(-25, 25)

            if not math.isfinite(front):
                front = self.scan.range_max

            if front < self.bypass_distance:

                self.publish(
                    0.0,
                    0.6 * self.turn_direction
                )

                return

            self.publish(
                0.22,
                0.0
            )

            if self.elapsed() > 2.5:

                self.set_state("MAPPING")

            return

        if self.state == "MAPPING":

            if self.elapsed() > self.mapping_time:

                self.publish(0.0, 0.0)
                self.set_state("RETURN_HOME")
                return

            if self.edge_detected():

                self.publish(0.0, 0.0)

                self.turn_direction = 1.0

                if self.x > 0.0:
                    self.turn_direction = -1.0

                self.set_state("EDGE_TURN")
                return

            front = self.sector_min(-20, 20)

            if not math.isfinite(front):
                front = self.scan.range_max

            if front < self.obstacle_distance:

                self.publish(0.0, 0.0)

                self.choose_turn_direction()

                self.set_state("OBSTACLE_TURN")
                return

            self.publish(
                0.25,
                0.0
            )


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
