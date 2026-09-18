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

        now = self.get_clock().now()

        self.state_time = now
        self.mapping_start_time = now

        self.turn_direction = 1.0

        self.turn_start_yaw = 0.0
        self.turn_target_angle = math.radians(70.0)

        self.field_limit = 4.70

        self.obstacle_stop_distance = 0.85
        self.emergency_distance = 0.45
        self.clear_distance = 0.95

        self.backup_speed = -0.15
        self.forward_speed = 0.20
        self.turn_speed = 0.55

        self.stop_delay = 0.35
        self.backup_time = 0.80
        self.bypass_time = 1.20

        self.mapping_time = 120.0
        self.return_distance = 0.20

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
            q.w * q.z +
            q.x * q.y
        )

        cosy_cosp = 1.0 - 2.0 * (
            q.y * q.y +
            q.z * q.z
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

        start = max(
            0,
            min(start, len(self.scan.ranges) - 1)
        )

        end = max(
            0,
            min(end, len(self.scan.ranges) - 1)
        )

        if start > end:
            start, end = end, start

        for i in range(start, end + 1):
            r = self.scan.ranges[i]

            if (
                math.isfinite(r)
                and r >= self.scan.range_min
                and r <= self.scan.range_max
            ):
                values.append(r)

        if not values:
            return float("inf")

        return min(values)

    def publish(self, vx, wz):
        msg = Twist()

        msg.linear.x = vx
        msg.angular.z = wz

        self.cmd_pub.publish(msg)

    def elapsed(self):
        now = self.get_clock().now()

        return (
            now - self.state_time
        ).nanoseconds / 1e9

    def mapping_elapsed(self):
        now = self.get_clock().now()

        return (
            now - self.mapping_start_time
        ).nanoseconds / 1e9

    def set_state(self, state):
        if self.state != state:
            self.get_logger().info(
                f"{self.state} -> {state}"
            )

        self.state = state
        self.state_time = self.get_clock().now()

    def normalize_angle(self, angle):
        while angle > math.pi:
            angle -= 2.0 * math.pi

        while angle < -math.pi:
            angle += 2.0 * math.pi

        return angle

    def angle_turned(self):
        return abs(
            self.normalize_angle(
                self.yaw - self.turn_start_yaw
            )
        )

    def distance_to_home(self):
        dx = self.home_x - self.x
        dy = self.home_y - self.y

        return math.hypot(
            dx,
            dy
        )

    def near_field_edge(self):
        return (
            abs(self.x) > self.field_limit
            or abs(self.y) > self.field_limit
        )

    def choose_turn_direction(self):
        left = self.sector_min(
            25,
            100
        )

        right = self.sector_min(
            -100,
            -25
        )

        if not math.isfinite(left):
            left = self.scan.range_max

        if not math.isfinite(right):
            right = self.scan.range_max

        if left >= right:
            self.turn_direction = 1.0
        else:
            self.turn_direction = -1.0

    def start_obstacle_avoidance(self):
        self.publish(
            0.0,
            0.0
        )

        self.choose_turn_direction()

        self.set_state(
            "OBSTACLE_STOP"
        )

    def start_edge_avoidance(self):
        self.publish(
            0.0,
            0.0
        )

        center_dx = -self.x
        center_dy = -self.y

        desired_yaw = math.atan2(
            center_dy,
            center_dx
        )

        yaw_error = self.normalize_angle(
            desired_yaw - self.yaw
        )

        if yaw_error >= 0.0:
            self.turn_direction = 1.0
        else:
            self.turn_direction = -1.0

        self.set_state(
            "EDGE_BACKUP"
        )

    def go_home(self):
        distance = self.distance_to_home()

        if distance < self.return_distance:
            self.publish(
                0.0,
                0.0
            )

            self.set_state(
                "STOP"
            )

            return

        front = self.sector_min(
            -25,
            25
        )

        if front < self.obstacle_stop_distance:
            self.start_obstacle_avoidance()
            return

        dx = self.home_x - self.x
        dy = self.home_y - self.y

        target_yaw = math.atan2(
            dy,
            dx
        )

        yaw_error = self.normalize_angle(
            target_yaw - self.yaw
        )

        if abs(yaw_error) > 0.15:
            wz = max(
                -0.50,
                min(
                    0.50,
                    1.2 * yaw_error
                )
            )

            self.publish(
                0.0,
                wz
            )

            return

        speed = 0.18

        if distance < 0.60:
            speed = 0.10

        self.publish(
            speed,
            0.0
        )

    def control(self):
        if self.scan is None:
            self.publish(
                0.0,
                0.0
            )
            return

        if self.state == "STOP":
            self.publish(
                0.0,
                0.0
            )
            return

        front = self.sector_min(
            -20,
            20
        )

        rear = min(
            self.sector_min(
                160,
                179
            ),
            self.sector_min(
                -179,
                -160
            )
        )

        if self.state == "MAPPING":
            if self.mapping_elapsed() >= self.mapping_time:
                self.publish(
                    0.0,
                    0.0
                )

                self.set_state(
                    "RETURN_HOME"
                )

                return

            if self.near_field_edge():
                self.start_edge_avoidance()
                return

            if front <= self.obstacle_stop_distance:
                self.start_obstacle_avoidance()
                return

            self.publish(
                self.forward_speed,
                0.0
            )

            return

        if self.state == "OBSTACLE_STOP":
            self.publish(
                0.0,
                0.0
            )

            if self.elapsed() >= self.stop_delay:
                self.set_state(
                    "OBSTACLE_BACKUP"
                )

            return

        if self.state == "OBSTACLE_BACKUP":
            if rear <= self.emergency_distance:
                self.publish(
                    0.0,
                    0.0
                )

                self.turn_start_yaw = self.yaw

                self.set_state(
                    "OBSTACLE_TURN"
                )

                return

            if self.elapsed() < self.backup_time:
                self.publish(
                    self.backup_speed,
                    0.0
                )

                return

            self.publish(
                0.0,
                0.0
            )

            self.turn_start_yaw = self.yaw

            self.set_state(
                "OBSTACLE_TURN"
            )

            return

        if self.state == "OBSTACLE_TURN":
            if self.angle_turned() < self.turn_target_angle:
                self.publish(
                    0.0,
                    self.turn_speed
                    * self.turn_direction
                )

                return

            self.publish(
                0.0,
                0.0
            )

            if front > self.clear_distance:
                self.set_state(
                    "OBSTACLE_BYPASS"
                )
            else:
                self.choose_turn_direction()
                self.turn_start_yaw = self.yaw

            return

        if self.state == "OBSTACLE_BYPASS":
            if front <= self.obstacle_stop_distance:
                self.start_obstacle_avoidance()
                return

            if self.near_field_edge():
                self.start_edge_avoidance()
                return

            self.publish(
                0.16,
                0.0
            )

            if self.elapsed() >= self.bypass_time:
                self.set_state(
                    "MAPPING"
                )

            return

        if self.state == "EDGE_BACKUP":
            if rear <= self.emergency_distance:
                self.publish(
                    0.0,
                    0.0
                )

                self.turn_start_yaw = self.yaw

                self.set_state(
                    "EDGE_TURN"
                )

                return

            if self.elapsed() < 0.70:
                self.publish(
                    -0.12,
                    0.0
                )

                return

            self.publish(
                0.0,
                0.0
            )

            self.turn_start_yaw = self.yaw

            self.set_state(
                "EDGE_TURN"
            )

            return

        if self.state == "EDGE_TURN":
            if self.angle_turned() < math.radians(90.0):
                self.publish(
                    0.0,
                    self.turn_speed
                    * self.turn_direction
                )

                return

            self.publish(
                0.0,
                0.0
            )

            self.set_state(
                "MAPPING"
            )

            return

        if self.state == "RETURN_HOME":
            self.go_home()
            return

        self.publish(
            0.0,
            0.0
        )


def main():
    rclpy.init()

    node = TRMapping()

    try:
        rclpy.spin(node)

    except KeyboardInterrupt:
        pass

    finally:
        node.publish(
            0.0,
            0.0
        )

        node.destroy_node()

        rclpy.shutdown()


if __name__ == "__main__":
    main()
