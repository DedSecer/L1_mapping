#include <ros/ros.h>
#include <sensor_msgs/PointCloud.h>
#include <sensor_msgs/PointCloud2.h>
#include <sensor_msgs/point_cloud_conversion.h>
#include <nav_msgs/OccupancyGrid.h>

#define RESOLUTION 0.01
#define Z_TOP 1.0
#define Z_BUTTOM 0.0
#define MAP_WIDTH 5000 // x length
#define MAP_HEIGHT 5000 // y length
 
using namespace std;

class OccupancyGrid
{
private:
    ros::Publisher map_publisher_;          // 声明一个Publisher
    ros::Publisher map_publisher_metadata_; // 声明一个Publisher
    nav_msgs::OccupancyGrid map_;           //用来发布map的实体对象

public:
    // OccupancyGrid();
    void init(ros::NodeHandle nh);
    void PublishMap();
    void UpdateMap(float x, float y);
};

// 构造函数
void OccupancyGrid::init(ros::NodeHandle nh)
{
    // \033[1;32m，\033[0m 终端显示成绿色
    ROS_INFO_STREAM("\033[1;32m----> Make Occupancy Grid Map by no move started.\033[0m");

    map_publisher_ = nh.advertise<nav_msgs::OccupancyGrid>("map", 1, true);
    map_publisher_metadata_ = nh.advertise<nav_msgs::MapMetaData>("map_metadata", 1, true);

    // 对map_进行初始化
    map_.header.frame_id = "camera_init";

    // 地图的分辨率为1m,代表一个格子的距离是1m
    map_.info.resolution = RESOLUTION;

    // 地图图片像素的大小, width为地图的宽度是多少个像素
    map_.info.width = MAP_WIDTH;
    map_.info.height = MAP_HEIGHT;

    // 如果要表示地图图片为多少米的话,就需要用实际长度除以分辨率,得到像素值
    // map_.info.width = 100 / map_.info.resolution;
    // map_.info.height = 100 / map_.info.resolution;

    // 地图左下角的点对应的物理坐标
    map_.info.origin.position.x = -(MAP_WIDTH * RESOLUTION / 2);
    map_.info.origin.position.y = -(MAP_HEIGHT * RESOLUTION / 2);

    // 对数组进行初始化, 数组的大小为实际像素的个数
    map_.data.resize(map_.info.width * map_.info.height);
}


// 构造map并进行发布
void OccupancyGrid::PublishMap()
{
    // 设置这一帧地图数据的时间戳
    map_.header.stamp = ros::Time::now();
    // 发布map和map_metadata话题
    map_publisher_.publish(map_);
    map_publisher_metadata_.publish(map_.info);
}

void OccupancyGrid::UpdateMap(float x, float y){
            // 二维坐标转成一维坐标
            int index = (int) (x/RESOLUTION+0.5 + MAP_WIDTH/2) + (int) (y/RESOLUTION+0.5 + MAP_HEIGHT/2) * map_.info.width;
            if (index >= 0 && index < map_.info.width * map_.info.height){
                map_.data[index] = 100;
            }else{
                ROS_WARN("out of map");
                cout << (int) (x/RESOLUTION+0.5 + MAP_WIDTH*RESOLUTION/2) << "," << (int) (y/RESOLUTION+0.5 + MAP_HEIGHT*RESOLUTION/2) << endl;
            }

}



void pointCloud2ToZ(const sensor_msgs::PointCloud2 &msg)
{
	sensor_msgs::PointCloud out_pointcloud;
	sensor_msgs::convertPointCloud2ToPointCloud(msg, out_pointcloud);
    extern OccupancyGrid occupancy_grid;

	for (int i = 0; i < out_pointcloud.points.size(); i++) {
        if (out_pointcloud.points[i].z > Z_BUTTOM && out_pointcloud.points[i].z < Z_TOP) {
            occupancy_grid.UpdateMap(out_pointcloud.points[i].x, out_pointcloud.points[i].y);

        }
	}
}


OccupancyGrid occupancy_grid;
int main(int argc, char *argv[])
{
    ros::init(argc, argv, "cloud_subscriber");
    ros::NodeHandle nh;
    extern OccupancyGrid occupancy_grid;
    occupancy_grid.init(nh);
    ros::Subscriber sub =  nh.subscribe("/cloud_registered", 10, pointCloud2ToZ);

    ros::Rate rate(1);
    while(ros::ok()){
        // publish map
        occupancy_grid.PublishMap();
        ros::spinOnce();
        rate.sleep();
    }
    return 0;
}
