#include <ros/ros.h>
#include <tf/transform_listener.h>
#include <nav_msgs/Odometry.h>
#include <string.h>
#include <move_base_msgs/MoveBaseAction.h>
#include <actionlib/client/simple_action_client.h>
#include <cmath>

int main(int argc, char** argv)
{
    ros::init(argc, argv, "nav_move_base");
    ros::NodeHandle node;
    ros::Duration(5.0).sleep();
    actionlib::SimpleActionClient<move_base_msgs::MoveBaseAction> ac("move_base", true); // 订阅move_base操作服务器

    double forward_distance = 3.0;
    node.param("/forward_and_back/forward_distance", forward_distance, forward_distance);

    tf::TransformListener listener;
    tf::StampedTransform transform;

    try {
        // 等待并获取机器人当前位姿
        listener.waitForTransform("/map", "/base_link", ros::Time(0), ros::Duration(10.0));
        listener.lookupTransform("/map", "/base_link", ros::Time(0), transform);
    } catch (tf::TransformException &ex) {
        ROS_ERROR("%s", ex.what());
        return 1;
    }

    // 获取机器人当前位姿
    double x = transform.getOrigin().x();
    double y = transform.getOrigin().y();
    double theta = tf::getYaw(transform.getRotation());

    // 计算前进后的目标点
    geometry_msgs::Point forward_point;
    forward_point.x = x + forward_distance * cos(theta);
    forward_point.y = y + forward_distance * sin(theta);
    forward_point.z = 0.0;

    // 设置目标点的方向（保持与当前方向一致）
    geometry_msgs::Quaternion forward_quaternion = tf::createQuaternionMsgFromYaw(theta);

    geometry_msgs::Pose forward_pose;
    forward_pose.position = forward_point;
    forward_pose.orientation = forward_quaternion;

    ROS_INFO("waiting movebase server.");
    if (!ac.waitForServer(ros::Duration(30))) // 等待操作服务器可用
    {
        ROS_INFO("not connect movebase server.");
        return 1;
    }
    ROS_INFO("connected movebase server success.");
    ROS_INFO("start to navigation.");

    move_base_msgs::MoveBaseGoal forward_goal;
    forward_goal.target_pose.header.frame_id = "map";
    forward_goal.target_pose.header.stamp = ros::Time::now();
    forward_goal.target_pose.pose = forward_pose;
    ac.sendGoal(forward_goal); // 将目标位姿发送到MoveBaseAction服务器

    bool finished_within_time = ac.waitForResult(ros::Duration(1000)); // 要求机器人在1000s时间内到达目标点
    if (!finished_within_time) // 没有及时到达，放弃目标
    {
        ac.cancelGoal();
        ROS_INFO("time out for arrival goal.");
    }
    else
    {
        if (ac.getState() == actionlib::SimpleClientGoalState::SUCCEEDED)
        {
            ROS_INFO("success to arrival goal!");
        }
        else
        {
            ROS_INFO("other reason to failed to arrival goal.");
        }
    }

    // 等待3秒钟
    ros::Duration(3.0).sleep();

    // 设置返回初始位置的目标点
    geometry_msgs::Pose initial_pose;
    initial_pose.position.x = x;
    initial_pose.position.y = y;
    initial_pose.position.z = 0.0;
    initial_pose.orientation = tf::createQuaternionMsgFromYaw(theta);

    move_base_msgs::MoveBaseGoal return_goal;
    return_goal.target_pose.header.frame_id = "map";
    return_goal.target_pose.header.stamp = ros::Time::now();
    return_goal.target_pose.pose = initial_pose;
    ac.sendGoal(return_goal); // 将目标位姿发送到MoveBaseAction服务器

    finished_within_time = ac.waitForResult(ros::Duration(1000)); // 要求机器人在1000s时间内返回初始位置
    if (!finished_within_time) // 没有及时到达，放弃目标
    {
        ac.cancelGoal();
        ROS_INFO("time out for arrival goal.");
    }
    else
    {
        if (ac.getState() == actionlib::SimpleClientGoalState::SUCCEEDED)
        {
            ROS_INFO("success to return to initial position!");
        }
        else
        {
            ROS_INFO("other reason to failed to return to initial position.");
        }
    }

    ros::spin();
    ROS_INFO("movebase kill...");
    return 0;
}
