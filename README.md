0. Giải nén project
cd ~
unzip ~/Robocon2027-main.zip
mv ~/Robocon2027-main ~/robocon2027_gazebo
Nếu thư mục ~/Robocon2027_main đã tồn tại thì không chạy mv, mà kiểm tra:
ls ~/robocon2027_main
1. Build ROS 2 package

Mở Terminal:

cd ~/robocon2027_gazebo

source /opt/ros/lyrical/setup.bash

colcon build --symlink-install

Sau khi build xong:

source ~/robocon2027_gazebo/install/setup.bash

Kiểm tra:

ros2 pkg list | grep robocon2027

Phải có:

robocon2027_description
2. Terminal 1 — chạy Gazebo
source /opt/ros/lyrical/setup.bash
source ~/robocon2027_gazebo/install/setup.bash

export GZ_SIM_RESOURCE_PATH=~/robocon2027_gazebo/install/robocon2027_description/share:$GZ_SIM_RESOURCE_PATH
export GZ_SIM_SYSTEM_PLUGIN_PATH=/opt/ros/lyrical/lib:$GZ_SIM_SYSTEM_PLUGIN_PATH

gz sim ~/robocon2027_gazebo/src/robocon2027_description/worlds/robocon2027.sdf

Gazebo mở lên thì:

bấm Play đúng 1 lần.

Không Pause/Reset trong lúc mapping.

3. Terminal 2 — robot_state_publisher
source /opt/ros/lyrical/setup.bash
source ~/robocon2027_gazebo/install/setup.bash

/opt/ros/lyrical/bin/xacro \
~/robocon2027_gazebo/src/robocon2027_description/urdf/tr.xacro \
> /tmp/tr.urdf

Sau đó:

ros2 run robot_state_publisher robot_state_publisher \
--ros-args \
-p robot_description:="$(cat /tmp/tr.urdf)" \
-p use_sim_time:=true

Giữ Terminal này chạy.

4. Terminal 3 — spawn robot TR
source /opt/ros/lyrical/setup.bash
source ~/robocon2027_gazebo/install/setup.bash

ros2 run ros_gz_sim create \
-name tr \
-topic /robot_description \
-x -4 \
-y -4 \
-z 0.02

Robot sẽ xuất hiện tại:

x = -4
y = -4
z = 0.02
5. Terminal 4 — Clock
source /opt/ros/lyrical/setup.bash

ros2 run ros_gz_bridge parameter_bridge \
/world/robocon2027/clock@rosgraph_msgs/msg/Clock[gz.msgs.Clock \
--ros-args \
-r /world/robocon2027/clock:=/clock
6. Terminal 5 — Gazebo TF → ROS TF
source /opt/ros/lyrical/setup.bash
source ~/robocon2027_gazebo/install/setup.bash

ros2 run ros_gz_bridge parameter_bridge \
/model/tr/tf@tf2_msgs/msg/TFMessage[gz.msgs.Pose_V \
--ros-args \
-r /model/tr/tf:=/tf
7. Terminal 6 — Static TF
source /opt/ros/lyrical/setup.bash

ros2 run tf2_ros static_transform_publisher \
--x 0 \
--y 0 \
--z 0 \
--roll 0 \
--pitch 0 \
--yaw 0 \
--frame-id tr/base_link \
--child-frame-id base_link
8. Terminal 7 — LiDAR
source /opt/ros/lyrical/setup.bash
source ~/robocon2027_gazebo/install/setup.bash

ros2 run ros_gz_bridge parameter_bridge \
/scan@sensor_msgs/msg/LaserScan[gz.msgs.LaserScan
9. Terminal 8 — Odometry
source /opt/ros/lyrical/setup.bash
source ~/robocon2027_gazebo/install/setup.bash

ros2 run ros_gz_bridge parameter_bridge \
/model/tr/odometry@nav_msgs/msg/Odometry[gz.msgs.Odometry
10. Kiểm tra Odometry

Mở Terminal 9:

source /opt/ros/lyrical/setup.bash

ros2 topic hz /model/tr/odometry

Bạn trước đó đã đo khoảng:

49.2 Hz

→ như vậy là ổn.

11. Kiểm tra TF

Terminal mới:

source /opt/ros/lyrical/setup.bash

ros2 run tf2_ros tf2_echo tr/odom lidar_link

Phải thấy Translation/Rotation.

Không được có:

Invalid frame ID

hoặc:

two or more unconnected trees
12. Kiểm tra LiDAR
source /opt/ros/lyrical/setup.bash

ros2 topic hz /scan

Khoảng:

9.9 Hz

là đúng với cấu hình hiện tại.

13. Chạy SLAM Toolbox

Terminal mới:

source /opt/ros/lyrical/setup.bash
source ~/robocon2027_gazebo/install/setup.bash

ros2 run slam_toolbox async_slam_toolbox_node \
--ros-args \
-p use_sim_time:=true \
-p odom_frame:=tr/odom \
-p map_frame:=map \
-p base_frame:=tr/base_link \
-p scan_topic:=/scan

Giữ Terminal này chạy.

14. Configure SLAM

Terminal mới:

source /opt/ros/lyrical/setup.bash

ros2 lifecycle set /slam_toolbox configure

Phải hiện:

Transitioning successful

Kiểm tra:

ros2 lifecycle get /slam_toolbox

Phải:

inactive [2]
15. Activate SLAM
ros2 lifecycle set /slam_toolbox activate

Kiểm tra:

ros2 lifecycle get /slam_toolbox

Phải:

active [3]
16. Kiểm tra map
ros2 topic hz /map

Nếu SLAM hoạt động, /map bắt đầu publish.

17. Kiểm tra TF map
ros2 run tf2_ros tf2_echo map lidar_link

Cây TF mong muốn:

map
 ↓
tr/odom
 ↓
tr/base_link
 ↓
base_link
 ↓
lidar_link
18. Mở RViz

Terminal mới:

source /opt/ros/lyrical/setup.bash
source ~/robocon2027_gazebo/install/setup.bash

rviz2

Trong RViz:

Global Options
Fixed Frame = map
Add

Thêm:

Map
LaserScan
TF
RobotModel

Map:

Topic = /map

LaserScan:

Topic = /scan

Nếu thấy robot + LaserScan + map, phần SLAM cơ bản đã chạy.

19. Bridge /cmd_vel

Terminal mới:

source /opt/ros/lyrical/setup.bash
source ~/robocon2027_gazebo/install/setup.bash

ros2 run ros_gz_bridge parameter_bridge \
/model/tr/cmd_vel@geometry_msgs/msg/Twist]gz.msgs.Twist \
--ros-args \
-r /model/tr/cmd_vel:=/cmd_vel

⚠️ Chỉ chạy bridge này một lần.

20. Chạy tr_mapping.py

Cuối cùng:

source /opt/ros/lyrical/setup.bash
source ~/robocon2027_gazebo/install/setup.bash

python3 ~/robocon2027_gazebo/scripts/tr_mapping.py

Đây mới là node điều khiển mapping của TR.
