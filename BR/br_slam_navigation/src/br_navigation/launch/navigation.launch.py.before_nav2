from launch import LaunchDescription
from launch_ros.actions import Node
from ament_index_python.packages import get_package_share_directory
import os


def generate_launch_description():

    pkg = get_package_share_directory('br_navigation')

    params_file = os.path.join(
        pkg,
        'config',
        'nav2_params.yaml'
    )

    keepout_yaml = os.path.join(
        pkg,
        'maps',
        'field_keepout.yaml'
    )

    return LaunchDescription([

        # ============================================================
        # KEEP-OUT MASK
        # ============================================================

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

        # ============================================================
        # KEEP-OUT FILTER INFO
        # ============================================================

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

        # ============================================================
        # GLOBAL COSTMAP
        # ============================================================

        Node(
            package='nav2_controller',
            executable='controller_server',
            name='controller_server',
            output='screen',
            parameters=[params_file],
            remappings=[
                ('cmd_vel', '/cmd_vel'),
            ],
        ),

        # ============================================================
        # PLANNER
        # ============================================================

        Node(
            package='nav2_planner',
            executable='planner_server',
            name='planner_server',
            output='screen',
            parameters=[params_file],
        ),

        # ============================================================
        # BEHAVIOR
        # ============================================================

        Node(
            package='nav2_behaviors',
            executable='behavior_server',
            name='behavior_server',
            output='screen',
            parameters=[params_file],
        ),

        # ============================================================
        # BT NAVIGATOR
        # ============================================================

        Node(
            package='nav2_bt_navigator',
            executable='bt_navigator',
            name='bt_navigator',
            output='screen',
            parameters=[params_file],
        ),

        # ============================================================
        # LIFECYCLE MANAGER
        # ============================================================

        Node(
            package='nav2_lifecycle_manager',
            executable='lifecycle_manager',
            name='lifecycle_manager_navigation',
            output='screen',
            parameters=[{
                'use_sim_time': True,
                'autostart': True,

                'node_names': [
                    'keepout_filter_mask_server',
                    'keepout_costmap_filter_info_server',
                    'global_costmap/global_costmap',
                    'local_costmap/local_costmap',
                    'controller_server',
                    'planner_server',
                    'behavior_server',
                    'bt_navigator',
                ],
            }],
        ),
    ])
