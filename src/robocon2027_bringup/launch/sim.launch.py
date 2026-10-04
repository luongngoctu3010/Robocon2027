from launch import LaunchDescription
from launch.actions import ExecuteProcess
from launch_ros.actions import Node
from launch.substitutions import PathJoinSubstitution
from launch_ros.substitutions import FindPackageShare


def generate_launch_description():

    world = PathJoinSubstitution([
        FindPackageShare("robocon2027_description"),
        "worlds",
        "robocon2027.sdf"
    ])

    return LaunchDescription([

        ExecuteProcess(
            cmd=[
                "gz",
                "sim",
                "-r",
                world
            ],
            output="screen"
        ),

    ])
