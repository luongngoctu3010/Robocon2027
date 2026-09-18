# Robocon 2027 Gazebo Simulation

Dự án mô phỏng sân đấu và Robot ABU Robocon 2027 trên môi trường ROS 2 Gazebo Sim.

---

## 🛠 Hướng dẫn cài đặt và Biên dịch (Colcon Build)

### 1. Tải dự án và giải nén
Sau khi tải file zip về, giải nén thư mục `robocon2027_gazebo` vào thư mục Home (`~`):
```bash
cd ~
# Đảm bảo thư mục dự án nằm tại đường dẫn: ~/robocon2027_gazebo
cd ~/robocon2027_gazebo

# Biến môi trường ROS 2
source /opt/ros/lyrical/setup.bash

# Biên dịch gói
colcon build

# Nạp môi trường sau khi build
source install/setup.bash
# 1. Source môi trường ROS 2 & Workspace
source /opt/ros/lyrical/setup.bash
source ~/robocon2027_gazebo/install/setup.bash

# 2. Cấu hình đường dẫn Resource & Plugin cho Gazebo
export GZ_SIM_RESOURCE_PATH=~/robocon2027_gazebo/install/robocon2027_description/share:$GZ_SIM_RESOURCE_PATH
export GZ_SIM_SYSTEM_PLUGIN_PATH=/opt/ros/lyrical/lib:$GZ_SIM_SYSTEM_PLUGIN_PATH

# 3. Khởi chạy sân đấu Gazebo
gz sim ~/robocon2027_gazebo/src/robocon2027_description/worlds/robocon2027.sdf
echo "source /opt/ros/lyrical/setup.bash" >> ~/.bashrc
echo "source ~/robocon2027_gazebo/install/setup.bash" >> ~/.bashrc
echo 'export GZ_SIM_RESOURCE_PATH=~/robocon2027_gazebo/install/robocon2027_description/share:$GZ_SIM_RESOURCE_PATH' >> ~/.bashrc
echo 'export GZ_SIM_SYSTEM_PLUGIN_PATH=/opt/ros/lyrical/lib:$GZ_SIM_SYSTEM_PLUGIN_PATH' >> ~/.bashrc
source ~/.bashrc
gz sim ~/robocon2027_gazebo/src/robocon2027_description/worlds/robocon2027.sdf
