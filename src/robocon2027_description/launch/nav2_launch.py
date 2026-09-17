from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument
from launch.substitutions import LaunchConfiguration
from launch_ros.actions import Node

def generate_launch_description():
    params_file = LaunchConfiguration("params_file")
    map_file = LaunchConfiguration("map")

    return LaunchDescription([
        DeclareLaunchArgument(
            "params_file",
            default_value="/home/luongngoctu/robocon2027_gazebo/src/robocon2027_description/config/nav2_params.yaml"
        ),

        DeclareLaunchArgument(
            "map",
            default_value="/home/luongngoctu/robocon2027_gazebo/maps/robocon2027_slam.yaml"
        ),

        Node(
            package="nav2_map_server",
            executable="map_server",
            name="map_server",
            output="screen",
            parameters=[
                params_file,
                {"yaml_filename": map_file}
            ]
        ),

        Node(
            package="nav2_amcl",
            executable="amcl",
            name="amcl",
            output="screen",
            parameters=[params_file]
        ),

        Node(
            package="nav2_planner",
            executable="planner_server",
            name="planner_server",
            output="screen",
            parameters=[params_file]
        ),

       Node(
           package="nav2_controller",
           executable="controller_server",
           name="controller_server",
           output="screen",
           parameters=[params_file],
           remappings=[
           ("/cmd_vel", "/diff_drive_controller/cmd_vel"),
           ],
        ),

        Node(
            package="nav2_behaviors",
            executable="behavior_server",
            name="behavior_server",
            output="screen",
            parameters=[params_file]
        ),

        Node(
            package="nav2_bt_navigator",
            executable="bt_navigator",
            name="bt_navigator",
            output="screen",
            parameters=[params_file]
        ),

        Node(
            package="nav2_lifecycle_manager",
            executable="lifecycle_manager",
            name="lifecycle_manager_navigation",
            output="screen",
            parameters=[
                params_file,
                {
                    "autostart": True,
                    "node_names": [
                        "map_server",
                        "amcl",
                        "planner_server",
                        "controller_server",
                        "behavior_server",
                        "bt_navigator"
                    ]
                }
            ]
        )
    ])
