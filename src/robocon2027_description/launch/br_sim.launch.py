from launch import LaunchDescription
from launch.actions import ExecuteProcess
from launch_ros.actions import Node


def generate_launch_description():

    world = (
        "/home/luongngoctu/Downloads/Robocon2027-main/"
        "install/robocon2027_description/share/"
        "robocon2027_description/worlds/robocon2027.sdf"
    )

    return LaunchDescription([

        # Gazebo
        ExecuteProcess(
            cmd=["gz", "sim", "-r", world],
            output="screen"
        ),

        # ROS <-> Gazebo bridge
        Node(
            package="ros_gz_bridge",
            executable="parameter_bridge",
            arguments=[
                "/clock@rosgraph_msgs/msg/Clock[gz.msgs.Clock",
                "/scan@sensor_msgs/msg/LaserScan[gz.msgs.LaserScan",
                "/odom@nav_msgs/msg/Odometry[gz.msgs.Odometry",
                "/cmd_vel@geometry_msgs/msg/Twist]gz.msgs.Twist",
                "/tf@tf2_msgs/msg/TFMessage[gz.msgs.Pose_V",
            ],
            output="screen"
        ),

        # User BR frame:
        # +Z = hướng tiến chính của BR
        #
        # ROS base_link:
        # +X = hướng tiến chính
        #
        # br_base +Z <=> base_link +X
        Node(
            package="tf2_ros",
            executable="static_transform_publisher",
            arguments=[
                "0", "0", "0",
                "0", "1.57079632679", "0",
                "base_link",
                "br_base"
            ],
            output="screen"
        ),
    ])
