#!/usr/bin/python
# coding:utf-8
 
import rospy
from sensor_msgs.msg import PointCloud2
from sensor_msgs import point_cloud2
 
def callback_pointcloud(data):
    assert isinstance(data, PointCloud2)
    gen = point_cloud2.read_points(data)
    # print(type(gen))
    for p in gen:
        print(type(p))
 
 
if __name__ == '__main__':
    rospy.init_node('pcl_subscriber', anonymous=True)
    #Subscriber函数第一个参数是topic的名称，第二个参数是接受的数据类型 第三个参数是回调函数的名称
    rospy.Subscriber('/cloud_registered', PointCloud2, callback_pointcloud)
    rospy.spin()