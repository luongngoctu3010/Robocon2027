#!/usr/bin/env python3

import rclpy
from rclpy.node import Node

from sensor_msgs.msg import PointCloud2
from sensor_msgs_py import point_cloud2


class PointCloudDistance(Node):

    def __init__(self):
        super().__init__('pointcloud_distance')

        self.subscription = self.create_subscription(
            PointCloud2,
            '/camera/depth/points',
            self.pointcloud_callback,
            10
        )

        self.count = 0

        self.get_logger().info(
            'BR Astra Pro PointCloud node started'
        )

    def pointcloud_callback(self, msg):

        self.count += 1

        # Đọc các điểm XYZ
        points = point_cloud2.read_points(
            msg,
            field_names=('x', 'y', 'z'),
            skip_nans=True
        )

        min_distance = None
        closest_point = None

        for p in points:

            x = float(p[0])
            y = float(p[1])
            z = float(p[2])

            # Bỏ các điểm không hợp lệ
            if z <= 0.05 or z > 10.0:
                continue

            distance = (x*x + y*y + z*z) ** 0.5

            if min_distance is None or distance < min_distance:
                min_distance = distance
                closest_point = (x, y, z)

        if closest_point is not None:

            x, y, z = closest_point

            self.get_logger().info(
                f'Closest XYZ: '
                f'X={x:.3f} m, '
                f'Y={y:.3f} m, '
                f'Z={z:.3f} m | '
                f'D={min_distance:.3f} m'
            )


def main(args=None):

    rclpy.init(args=args)

    node = PointCloudDistance()

    try:
        rclpy.spin(node)

    except KeyboardInterrupt:
        pass

    node.destroy_node()
    rclpy.shutdown()


if __name__ == '__main__':
    main()
