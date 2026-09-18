1. Terminal 1 — Gazebo
source /opt/ros/lyrical/setup.bash
source ~/robocon2027_gazebo/install/setup.bash
export GZ_SIM_RESOURCE_PATH=~/robocon2027_gazebo/install/robocon2027_description/share:$GZ_SIM_RESOURCE_PATH
export GZ_SIM_SYSTEM_PLUGIN_PATH=/opt/ros/lyrical/lib:$GZ_SIM_SYSTEM_PLUGIN_PATH
gz sim ~/robocon2027_gazebo/src/robocon2027_description/worlds/robocon2027.sdf
Sau khi Gazebo mở:
bấm Play một lần duy nhất.
Sau đó không Pause, không Reset trong suốt phiên SLAM.

2. Terminal 2 — robot_state_publisher
source /opt/ros/lyrical/setup.bash
source ~/robocon2027_gazebo/install/setup.bash
/opt/ros/lyrical/bin/xacro ~/robocon2027_gazebo/src/robocon2027_description/urdf/tr.xacro > /tmp/tr.urdf
ros2 run robot_state_publisher robot_state_publisher --ros-args -p robot_description:="$(cat /tmp/tr.urdf)" -p use_sim_time:=true
Giữ terminal này chạy.

3. Terminal 3 — Spawn TR
source /opt/ros/lyrical/setup.bash
source ~/robocon2027_gazebo/install/setup.bash
ros2 run ros_gz_sim create -name tr -topic /robot_description -x -4 -y -4 -z 0.02
Robot ở:
(-4, -4)

4. Terminal 4 — Clock
source /opt/ros/lyrical/setup.bash
ros2 run ros_gz_bridge parameter_bridge /world/robocon2027/clock@rosgraph_msgs/msg/Clock[gz.msgs.Clock --ros-args -r /world/robocon2027/clock:=/clock

5. Terminal 5 — TF Gazebo → ROS
source /opt/ros/lyrical/setup.bash
source ~/robocon2027_gazebo/install/setup.bash
ros2 run ros_gz_bridge parameter_bridge /model/tr/tf@tf2_msgs/msg/TFMessage[gz.msgs.Pose_V --ros-args -r /model/tr/tf:=/tf

6. Terminal 6 — Static TF
source /opt/ros/lyrical/setup.bash
ros2 run tf2_ros static_transform_publisher --x 0 --y 0 --z 0 --roll 0 --pitch 0 --yaw 0 --frame-id tr/base_link --child-frame-id base_link

7. Terminal 7 — LiDAR
source /opt/ros/lyrical/setup.bash
source ~/robocon2027_gazebo/install/setup.bash
ros2 run ros_gz_bridge parameter_bridge /scan@sensor_msgs/msg/LaserScan[gz.msgs.LaserScan

8. Terminal 8 — Odometry
source /opt/ros/lyrical/setup.bash
source ~/robocon2027_gazebo/install/setup.bash
ros2 run ros_gz_bridge parameter_bridge /model/tr/odometry@nav_msgs/msg/Odometry[gz.msgs.Odometry

9. Kiểm tra Odometry trước khi chạy SLAM
Terminal mới:
source /opt/ros/lyrical/setup.bash
ros2 topic hz /model/tr/odometry
Phải thấy khoảng:
49 Hz
Bạn đã kiểm tra được ~49.2 Hz nên bước này dự kiến OK.

10. Kiểm tra TF robot → LiDAR
Terminal mới:
source /opt/ros/lyrical/setup.bash
ros2 run tf2_ros tf2_echo tr/odom lidar_link
Phải có Translation/Rotation.
Không được có:
Invalid frame ID
hoặc:
two or more unconnected trees

11. Kiểm tra LiDAR
Terminal mới:
source /opt/ros/lyrical/setup.bash
ros2 topic hz /scan
Phải khoảng:
9.9 Hz

12. Terminal 9 — SLAM
Sau khi 9–11 đều OK:
source /opt/ros/lyrical/setup.bash
source ~/robocon2027_gazebo/install/setup.bash
ros2 run slam_toolbox async_slam_toolbox_node --ros-args -p use_sim_time:=true -p odom_frame:=tr/odom -p map_frame:=map -p base_frame:=tr/base_link -p scan_topic:=/scan
Giữ terminal này chạy.

13. Configure SLAM
Terminal mới:
source /opt/ros/lyrical/setup.bash
ros2 lifecycle set /slam_toolbox configure
Kết quả:
Transitioning successful
Kiểm tra:
ros2 lifecycle get /slam_toolbox
Phải là:
inactive [2]

14. Activate SLAM
ros2 lifecycle set /slam_toolbox activate
Sau đó:
ros2 lifecycle get /slam_toolbox
Phải là:
active [3]

15. Kiểm tra /map
ros2 topic hz /map
Nếu SLAM hoạt động, /map sẽ bắt đầu được publish.

16. Kiểm tra TF map → lidar_link
ros2 run tf2_ros tf2_echo map lidar_link
Lúc này cây TF mong muốn:
map
 ↓
tr/odom
 ↓
tr/base_link
 ↓
base_link
 ↓
lidar_link

17. Terminal 10 — RViz
source /opt/ros/lyrical/setup.bash
rviz2
Trong RViz:
Global Options
Fixed Frame = map
Add:
Map
LaserScan
TF
RobotModel
LaserScan:
Topic = /scan
Map:
Topic = /map

18. Terminal 11 — /cmd_vel bridge
Chỉ có đúng một bridge này.
source /opt/ros/lyrical/setup.bash
source ~/robocon2027_gazebo/install/setup.bash
ros2 run ros_gz_bridge parameter_bridge /model/tr/cmd_vel@geometry_msgs/msg/Twist]gz.msgs.Twist --ros-args -r /model/tr/cmd_vel:=/cmd_vel
Không chạy dòng này lần thứ hai.

19. Cuối cùng mới chạy mapping
Khi RViz đã thấy:
LaserScan
Map
thì mới chạy:
source /opt/ros/lyrical/setup.bash
source ~/robocon2027_gazebo/install/setup.bash
python3 ~/robocon2027_gazebo/scripts/tr_mapping.py

