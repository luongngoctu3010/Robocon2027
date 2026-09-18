#!/usr/bin/env bash
set -Eeo pipefail

# ============================================================
# Robocon 2027 - One-command launcher
# Starts:
#   1) Gazebo
#   2) robot_state_publisher
#   3) Spawn TR
#   4) /clock bridge
#   5) Gazebo TF bridge
#   6) Static TF: tr/base_link -> base_link
#   7) LiDAR bridge
#   8) Odometry bridge
#   9) SLAM Toolbox + lifecycle configure/activate
#  10) RViz2
#  11) /cmd_vel bridge
#  12) tr_mapping.py
#
# Usage:
#   cd ~/robocon2027_gazebo
#   chmod +x run_all.sh
#   ./run_all.sh
# ============================================================

PROJECT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROS_DISTRO="lyrical"
ROS_SETUP="/opt/ros/${ROS_DISTRO}/setup.bash"
INSTALL_SETUP="${PROJECT_DIR}/install/setup.bash"
WORLD="${PROJECT_DIR}/src/robocon2027_description/worlds/robocon2027.sdf"
XACRO="${PROJECT_DIR}/src/robocon2027_description/urdf/tr.xacro"
SLAM_PARAMS="${PROJECT_DIR}/src/robocon2027_description/config/slam_params.yaml"
MAPPING_SCRIPT="${PROJECT_DIR}/scripts/tr_mapping.py"

# Stop all processes started by this script.
if [[ "${1:-}" == "stop" ]]; then
    if command -v tmux >/dev/null 2>&1; then
        tmux kill-session -t "${SESSION}" 2>/dev/null || true
    fi
    if [[ -f "${PROJECT_DIR}/.run_all_pids" ]]; then
        while read -r pid; do
            kill "${pid}" 2>/dev/null || true
        done < "${PROJECT_DIR}/.run_all_pids"
        rm -f "${PROJECT_DIR}/.run_all_pids"
    fi
    echo "Đã gửi lệnh dừng toàn bộ node của run_all.sh."
    exit 0
fi

# ---------- process launcher ----------
# Prefer tmux because many Ubuntu/VS Code environments do not have
# a GUI terminal emulator installed. If tmux is unavailable, fall
# back to background processes with per-node log files.
USE_TMUX=0
if command -v tmux >/dev/null 2>&1; then
    USE_TMUX=1
fi

SESSION="robocon2027"

cleanup() {
    echo
    echo "Stopping Robocon 2027 processes..."
    if [[ "${USE_TMUX}" -eq 1 ]]; then
        tmux kill-session -t "${SESSION}" 2>/dev/null || true
    else
        if [[ -f "${PROJECT_DIR}/.run_all_pids" ]]; then
            while read -r pid; do
                kill "${pid}" 2>/dev/null || true
            done < "${PROJECT_DIR}/.run_all_pids"
            rm -f "${PROJECT_DIR}/.run_all_pids"
        fi
    fi
}
launch_node() {
    local name="$1"
    local command="$2"

    if [[ "${USE_TMUX}" -eq 1 ]]; then
        tmux new-window -t "${SESSION}" -n "${name}" \
            "bash -lc 'source \"${ROS_SETUP}\"; source \"${INSTALL_SETUP}\"; echo \"=== ${name} ===\"; ${command}; rc=\$?; echo; echo \"[${name}] exited: \$rc\"; exec bash'"
    else
        mkdir -p "${PROJECT_DIR}/logs"
        local logfile="${PROJECT_DIR}/logs/${name// /_}.log"
        echo "[$(date '+%F %T')] START ${name}" >> "${logfile}"
        bash -lc "source '${ROS_SETUP}'; source '${INSTALL_SETUP}'; ${command}" \
            >> "${logfile}" 2>&1 &
        echo $! >> "${PROJECT_DIR}/.run_all_pids"
        echo "  ${name} -> ${logfile}"
    fi
}

# ---------- checks ----------
[[ -f "${ROS_SETUP}" ]] || { echo "ERROR: Không tìm thấy ${ROS_SETUP}"; exit 1; }
[[ -f "${INSTALL_SETUP}" ]] || {
    echo "ERROR: Chưa có install/setup.bash."
    echo "Hãy chạy:"
    echo "  cd ${PROJECT_DIR}"
    echo "  source ${ROS_SETUP}"
    echo "  colcon build --symlink-install"
    exit 1
}
[[ -f "${WORLD}" ]] || { echo "ERROR: Không tìm thấy world: ${WORLD}"; exit 1; }
[[ -f "${XACRO}" ]] || { echo "ERROR: Không tìm thấy xacro: ${XACRO}"; exit 1; }
[[ -f "${SLAM_PARAMS}" ]] || { echo "ERROR: Không tìm thấy: ${SLAM_PARAMS}"; exit 1; }
[[ -f "${MAPPING_SCRIPT}" ]] || { echo "ERROR: Không tìm thấy: ${MAPPING_SCRIPT}"; exit 1; }

source "${ROS_SETUP}"
source "${INSTALL_SETUP}"

# Verify the package is visible.
if ! ros2 pkg prefix robocon2027_description >/dev/null 2>&1; then
    echo "ERROR: package robocon2027_description chưa được ROS 2 nhận."
    echo "Chạy lại colcon build:"
    echo "  cd ${PROJECT_DIR}"
    echo "  source ${ROS_SETUP}"
    echo "  colcon build --symlink-install"
    exit 1
fi

# ---------- helper ----------
launch_gnome() {
    local title="$1"
    local command="$2"

    if [[ "${TERMINAL}" == "gnome-terminal" ]]; then
        gnome-terminal --title="${title}" -- bash -lc "
            source '${ROS_SETUP}'
            source '${INSTALL_SETUP}'
            echo '=================================================='
            echo ' ${title}'
            echo '=================================================='
            ${command}
            rc=\$?
            echo
            echo '[${title}] process exited with code '\$rc
            read -r -p 'Press Enter to close...'
            exit \$rc
        "
    elif [[ "${TERMINAL}" == "xfce4-terminal" ]]; then
        xfce4-terminal --title="${title}" --command="bash -lc \"source '${ROS_SETUP}'; source '${INSTALL_SETUP}'; ${command}; echo; read -r -p 'Press Enter to close...'\""
    elif [[ "${TERMINAL}" == "konsole" ]]; then
        konsole --new-tab -p tabtitle="${title}" -e bash -lc "source '${ROS_SETUP}'; source '${INSTALL_SETUP}'; ${command}; echo; read -r -p 'Press Enter to close...'"
    else
        xterm -T "${title}" -hold -e bash -lc "source '${ROS_SETUP}'; source '${INSTALL_SETUP}'; ${command}"
    fi
}

echo
echo "============================================================"
echo " ROBocon 2027 - STARTING FULL SIMULATION"
echo " Project : ${PROJECT_DIR}"
if [[ "${USE_TMUX}" -eq 1 ]]; then
    echo " Mode    : tmux session '${SESSION}'"
else
    echo " Mode    : background processes + logs/"
fi
echo "============================================================"
echo

# Generate URDF once before opening the terminals.
echo "[1/4] Checking xacro..."
# Create the URDF once before launching the dependent nodes.
"${ROS_SETUP}" >/dev/null 2>&1 || true
source "${ROS_SETUP}"
source "${INSTALL_SETUP}"
if ! /opt/ros/${ROS_DISTRO}/bin/xacro "${XACRO}" > /tmp/tr.urdf; then
    echo "ERROR: xacro lỗi. Không mở simulation."
    exit 1
fi
echo "      OK: /tmp/tr.urdf"

if [[ "${USE_TMUX}" -eq 1 ]]; then
    tmux kill-session -t "${SESSION}" 2>/dev/null || true
    tmux new-session -d -s "${SESSION}" -n "00 launcher"
fi

echo "[2/4] Opening Gazebo..."
launch_node "01 Gazebo" \
    "export GZ_SIM_RESOURCE_PATH='${PROJECT_DIR}/install/robocon2027_description/share':\\\$GZ_SIM_RESOURCE_PATH; \
     export GZ_SIM_SYSTEM_PLUGIN_PATH='/opt/ros/${ROS_DISTRO}/lib':\\\$GZ_SIM_SYSTEM_PLUGIN_PATH; \
     gz sim -r '${WORLD}'"

sleep 5

echo "[3/4] Starting ROS/Gazebo bridges and SLAM..."
launch_node "02 robot_state_publisher" \
    "ros2 run robot_state_publisher robot_state_publisher \
     --ros-args -p robot_description:=\"\\\$(cat /tmp/tr.urdf)\" -p use_sim_time:=true"

launch_node "03 Spawn TR" \
    "sleep 2; \
     ros2 run ros_gz_sim create \
     -name tr -topic /robot_description -x -4 -y -4 -z 0.02"

launch_node "04 Clock bridge" \
    "ros2 run ros_gz_bridge parameter_bridge \
     '/world/robocon2027/clock@rosgraph_msgs/msg/Clock[gz.msgs.Clock' \
     --ros-args -r /world/robocon2027/clock:=/clock"

launch_node "05 TF bridge" \
    "ros2 run ros_gz_bridge parameter_bridge \
     '/model/tr/tf@tf2_msgs/msg/TFMessage[gz.msgs.Pose_V' \
     --ros-args -r /model/tr/tf:=/tf"

launch_node "06 Static TF" \
    "ros2 run tf2_ros static_transform_publisher \
     --x 0 --y 0 --z 0 --roll 0 --pitch 0 --yaw 0 \
     --frame-id tr/base_link --child-frame-id base_link"

launch_node "07 LiDAR bridge" \
    "ros2 run ros_gz_bridge parameter_bridge \
     '/scan@sensor_msgs/msg/LaserScan[gz.msgs.LaserScan'"

launch_node "08 Odometry bridge" \
    "ros2 run ros_gz_bridge parameter_bridge \
     '/model/tr/odometry@nav_msgs/msg/Odometry[gz.msgs.Odometry'"

# Give the publishers a moment before SLAM starts.
sleep 5

launch_node "09 SLAM Toolbox" \
    "ros2 run slam_toolbox async_slam_toolbox_node \
     --ros-args --params-file '${SLAM_PARAMS}'"

sleep 6

# Configure and activate SLAM automatically.
launch_node "10 SLAM lifecycle" \
    "sleep 2; \
     echo '[SLAM] configure'; \
     ros2 lifecycle set /slam_toolbox configure; \
     sleep 2; \
     echo '[SLAM] state:'; \
     ros2 lifecycle get /slam_toolbox; \
     echo '[SLAM] activate'; \
     ros2 lifecycle set /slam_toolbox activate; \
     sleep 2; \
     echo '[SLAM] state:'; \
     ros2 lifecycle get /slam_toolbox; \
     echo; \
     echo 'SLAM lifecycle terminal will stay open for diagnostics.'; \
     while true; do sleep 3600; done"

sleep 4

echo "[4/4] Opening RViz, cmd_vel bridge and mapping..."
launch_node "11 RViz2" \
    "rviz2"

launch_node "12 cmd_vel bridge" \
    "ros2 run ros_gz_bridge parameter_bridge \
     '/model/tr/cmd_vel@geometry_msgs/msg/Twist]gz.msgs.Twist' \
     --ros-args -r /model/tr/cmd_vel:=/cmd_vel"

sleep 4

launch_node "13 TR Mapping" \
    "python3 '${MAPPING_SCRIPT}'"

echo
echo "============================================================"
echo " ĐÃ KHỞI ĐỘNG XONG"
echo "============================================================"
echo
if [[ "${USE_TMUX}" -eq 1 ]]; then
    echo "Các cửa sổ đang nằm trong tmux session: ${SESSION}"
    echo
    echo "Xem session:"
    echo "  tmux attach -t ${SESSION}"
    echo
    echo "Tách khỏi tmux mà không dừng node:"
    echo "  Ctrl+B rồi D"
else
    echo "Các node đang chạy nền."
    echo "Log nằm tại:"
    echo "  ${PROJECT_DIR}/logs/"
    echo
    echo "PID file:"
    echo "  ${PROJECT_DIR}/.run_all_pids"
    echo
    echo "Dừng toàn bộ:"
    echo "  ./run_all.sh stop"
fi
echo
echo "RViz:"
echo "  Fixed Frame = map"
echo "  Add: Map (/map), LaserScan (/scan), TF, RobotModel"
echo
echo "LƯU Ý:"
echo "  - Không chạy thêm một /cmd_vel bridge."
echo "  - Không chạy thêm một tr_mapping.py."
echo "  - Không chạy Nav2 cùng lúc với tr_mapping.py."
echo "  - Khi mapping xong, lưu map bằng map_saver_cli."
echo
echo "Kiểm tra nhanh:"
echo "  ros2 topic hz /scan"
echo "  ros2 topic hz /model/tr/odometry"
echo "  ros2 topic hz /map"
echo
echo "============================================================"

# Internal helper mode used only by this script's self-check.
exit 0
