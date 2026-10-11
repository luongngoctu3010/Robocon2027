from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument
from launch.substitutions import LaunchConfiguration
from launch_ros.actions import Node
from ament_index_python.packages import get_package_share_directory
import os


def generate_launch_description():
    pkg = get_package_share_directory('br_navigation')

    params_file = os.path.join(pkg, 'config', 'nav2_params.yaml')
    keepout_yaml = os.path.join(pkg, 'maps', 'field_keepout.yaml')
    default_map = os.path.join(pkg, 'maps', 'robocon_map.yaml')

    return LaunchDescription([
        DeclareLaunchArgument(
            'map',
            default_value=default_map,
            description='Absolute path to the saved SLAM map YAML file',
        ),

        # Load the saved environment map used by Nav2.
        Node(
            package='nav2_map_server',
            executable='map_server',
            name='map_server',
            output='screen',
            parameters=[
                params_file,
                {
                    'use_sim_time': True,
                    'yaml_filename': LaunchConfiguration('map'),
                },
            ],
        ),

        # Localize the robot on the saved map. In RViz, use
        # "2D Pose Estimate" once the map is visible.
        Node(
            package='nav2_amcl',
            executable='amcl',
            name='amcl',
            output='screen',
            parameters=[params_file],
            remappings=[('scan', '/scan')],
        ),

        # Keep-out mask and metadata used by the global costmap filter.
        Node(
            package='nav2_map_server',
            executable='map_server',
            name='keepout_filter_mask_server',
            output='screen',
            parameters=[{
                'use_sim_time': True,
                'yaml_filename': keepout_yaml,
                'topic_name': '/keepout_filter_mask',
                'frame_id': 'map',
            }],
        ),
        Node(
            package='nav2_map_server',
            executable='costmap_filter_info_server',
            name='keepout_costmap_filter_info_server',
            output='screen',
            parameters=[{
                'use_sim_time': True,
                'type': 0,
                'filter_info_topic': '/keepout_costmap_filter_info',
                'mask_topic': '/keepout_filter_mask',
                'base': 0.0,
                'multiplier': 1.0,
            }],
        ),

        Node(
            package='nav2_controller',
            executable='controller_server',
            name='controller_server',
            output='screen',
            parameters=[params_file],
            remappings=[('cmd_vel', '/cmd_vel')],
        ),
        Node(
            package='nav2_planner',
            executable='planner_server',
            name='planner_server',
            output='screen',
            parameters=[params_file],
        ),
        Node(
            package='nav2_behaviors',
            executable='behavior_server',
            name='behavior_server',
            output='screen',
            parameters=[params_file],
        ),
        Node(
            package='nav2_bt_navigator',
            executable='bt_navigator',
            name='bt_navigator',
            output='screen',
            parameters=[params_file],
        ),

        Node(
            package='nav2_lifecycle_manager',
            executable='lifecycle_manager',
            name='lifecycle_manager_navigation',
            output='screen',
            parameters=[{
                'use_sim_time': True,
                'autostart': True,
                'node_names': [
                    'map_server',
                    'amcl',
                    'keepout_filter_mask_server',
                    'keepout_costmap_filter_info_server',
                    'controller_server',
                    'planner_server',
                    'behavior_server',
                    'bt_navigator',
                ],
            }],
        ),
    ])
