FROM ros:noetic-ros-base
WORKDIR /unilidar_sdk
COPY . .
RUN apt update;DEBIAN_FRONTEND=noninteractive apt install -y ros-noetic-pcl-conversions ros-noetic-tf ros-noetic-pcl-ros ros-noetic-eigen-conversions ros-noetic-move-base ros-noetic-pointcloud-to-laserscan
RUN bash -c "source /ros_entrypoint.sh;\
source /opt/ros/noetic/setup.bash;\
rm -r /unilidar_sdk/unitree_lidar_ros/build /unilidar_sdk/unitree_lidar_ros/devel;\
rm -r /unilidar_sdk/unitree_lidar_ros/catkin_point_lio_unilidar/build /unilidar_sdk/unitree_lidar_ros/catkin_point_lio_unilidar/devel;\
/opt/ros/noetic/bin/catkin_make -C /unilidar_sdk/untree_lidar_ros;\
/opt/ros/noetic/bin/catkin_make -C /unilidar_sdk/unitree_lidar_ros/catkin_point_lio_unilidar"
