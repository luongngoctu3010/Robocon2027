source /opt/ros/lyrical/setup.bash
source ~/robocon2027_gazebo/install/setup.bash
export GZ_SIM_RESOURCE_PATH=~/robocon2027_gazebo/install/robocon2027_description/share:$GZ_SIM_RESOURCE_PATH
export GZ_SIM_SYSTEM_PLUGIN_PATH=/opt/ros/lyrical/lib:$GZ_SIM_SYSTEM_PLUGIN_PATH
gz sim ~/robocon2027_gazebo/src/robocon2027_description/worlds/robocon2027.sdf
