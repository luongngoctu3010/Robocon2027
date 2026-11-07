from launch import LaunchDescription
from launch_ros.actions import Node


def generate_launch_description():

    lidar_tf = Node(
        package='tf2_ros',
        executable='static_transform_publisher',
        name='br_lidar_tf',
        arguments=[
            '--x', '0',
            '--y', '0',
            '--z', '0.32',
            '--roll', '0',
            '--pitch', '0',
            '--yaw', '0',
            '--frame-id', 'base_link',
            '--child-frame-id', 'BR/lidar_link/lidar'
        ],
        output='screen'
    )

    return LaunchDescription([
        lidar_tf
    ])
