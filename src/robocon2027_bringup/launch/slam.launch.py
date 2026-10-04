from launch import LaunchDescription
from launch_ros.actions import Node
from launch_ros.substitutions import FindPackageShare
from launch.substitutions import PathJoinSubstitution


def generate_launch_description():

    params = PathJoinSubstitution([
        FindPackageShare("robocon2027_bringup"),
        "config",
        "slam.yaml"
    ])

    return LaunchDescription([

        Node(
            package="slam_toolbox",
            executable="async_slam_toolbox_node",
            name="slam_toolbox",
            output="screen",
            parameters=[
                params
            ]
        )

    ])
