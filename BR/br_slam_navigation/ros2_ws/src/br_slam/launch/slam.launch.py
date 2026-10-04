from launch import LaunchDescription
from launch_ros.actions import Node
from ament_index_python.packages import get_package_share_directory
import os


def generate_launch_description():

    config_file = os.path.join(
        get_package_share_directory('br_slam'),
        'config',
        'mapper_params_online_async.yaml'
    )

    slam_node = Node(
        package='slam_toolbox',
        executable='async_slam_toolbox_node',
        name='slam_toolbox',
        output='screen',
        parameters=[
            config_file,
            {
                'use_sim_time': True,
                'scan_topic': '/scan',
                'base_frame': 'base_link',
                'odom_frame': 'odom',
                'map_frame': 'map',
                'mode': 'mapping',
            }
        ]
    )

    return LaunchDescription([
        slam_node
    ])
