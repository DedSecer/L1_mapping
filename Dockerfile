FROM ros:noetic-ros-base
WORKDIR /unilidar_sdk
COPY . .
RUN apt update;DEBIAN_FRONTEND=noninteractive apt install -y ros-noetic-pcl-conversions ros-noetic-tf ros-noetic-pcl-ros ros-noetic-eigen-conversions
RUN bash -c "source /ros_entrypoint.sh; source /opt/ros/noetic/setup.bash; cd /unilidar_sdk/untree_lidar_ros; /opt/ros/noetic/bin/catkin_make; cd /unilidar_sdk/unitree_lidar_ros/catkin_point_lio_unilidar; /opt/ros/noetic/bin/catkin_make"
