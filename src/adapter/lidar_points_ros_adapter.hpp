/******************************************************************************
 * Copyright 2020 RoboSense All rights reserved.
 * Suteng Innovation Technology Co., Ltd. www.robosense.ai

 * This software is provided to you directly by RoboSense and might
 * only be used to access RoboSense LiDAR. Any compilation,
 * modification, exploration, reproduction and redistribution are
 * restricted without RoboSense's prior consent.

 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESSED OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL ROBOSENSE BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *****************************************************************************/
#pragma once

/*****************************************************
 *
 *            ROS LiDAR packets adapter
 *
 ****************************************************/
#ifdef ROS_FOUND
#include "adapter/lidar_adapter_base.h"
#include "msg/ros_msg_translator.h"
#include <ros/ros.h>
#include <ros/publisher.h>

namespace robosense
{
namespace lidar
{
class LidarPointsRosAdapter : virtual public LidarAdapterBase
{
public:
  LidarPointsRosAdapter() = default;

  ~LidarPointsRosAdapter()
  {
    stop();
  }

  void init(const YAML::Node& config)
  {
    bool send_points_ros;
    YAML::Node ros_config = yamlSubNodeAbort(config, "ros");
    nh_ = std::unique_ptr<ros::NodeHandle>(new ros::NodeHandle());
    std::string ros_send_topic;
    yamlRead<std::string>(ros_config, "ros_send_points_topic", ros_send_topic, "rslidar_points");
    yamlRead<bool>(config, "send_points_ros", send_points_ros, false);
    if (send_points_ros)
    {
      lidar_points_pub_ = nh_->advertise<sensor_msgs::PointCloud2>(ros_send_topic, 10);
    }
  }

  inline void start()
  {
    return;
  }

  inline void stop()
  {
    return;
  }

  void regRecvCallback(const std::function<void(const LidarPointsMsg&)> callBack)
  {
    lidarPointscbs_.emplace_back(callBack);
  }

  void send(const LidarPointsMsg& msg)
  {
    lidar_points_pub_.publish(toRosMsg(msg));
  }

private:
  std::shared_ptr<ros::NodeHandle> nh_;
  std::vector<std::function<void(const LidarPointsMsg&)>> lidarPointscbs_;
  ros::Publisher lidar_points_pub_;
};
}  // namespace lidar
}  // namespace robosense
#endif  // ROS_FOUND

/*****************************************************
 *
 *            ROS2 LiDAR packets adapter
 *
 ****************************************************/
#ifdef ROS2_FOUND
#include "adapter/lidar_adapter_base.h"
#include "msg/ros_msg_translator.h"
#include "rclcpp/rclcpp.hpp"
namespace robosense
{
namespace lidar
{
class LidarPointsRosAdapter : virtual public LidarAdapterBase
{
public:
  LidarPointsRosAdapter() = default;

  ~LidarPointsRosAdapter()
  {
    stop();
  }

  void init(const YAML::Node& config)
  {
    bool send_points_ros;
    node_ptr_.reset(new rclcpp::Node("rslidar_points_adapter"));
    YAML::Node ros_config = yamlSubNodeAbort(config, "ros");
    std::string ros_send_topic;
    yamlRead<std::string>(ros_config, "ros_send_points_topic", ros_send_topic, "rslidar_points");
    yamlRead<bool>(config, "send_points_ros", send_points_ros, false);
    if (send_points_ros)
    {
      lidar_points_pub_ = node_ptr_->create_publisher<sensor_msgs::msg::PointCloud2>(ros_send_topic, 1);
    }
  }

  inline void start()
  {
    return;
  }

  inline void stop()
  {
    return;
  }

  void send(const LidarPointsMsg& msg)
  {
    lidar_points_pub_->publish(toRosMsg(msg));
  }

private:
  std::shared_ptr<rclcpp::Node> node_ptr_;
  rclcpp::Publisher<sensor_msgs::msg::PointCloud2>::SharedPtr lidar_points_pub_;
};
}  // namespace lidar
}  // namespace robosense
#endif  // ROS2_FOUND