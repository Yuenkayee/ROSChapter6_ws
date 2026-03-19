#pragma once
#include "nav2_core/controller.hpp"
#include "nav2_core/exceptions.hpp"
#include "nav2_util/geometry_utils.hpp"
#include "nav2_util/node_utils.hpp"
#include "nav2_util/robot_utils.hpp"
#include "pluginlib/class_list_macros.hpp"
#include "rclcpp/rclcpp.hpp"
#include <algorithm>
#include <chrono>
#include <iostream>
#include <memory>
#include <string>
#include <thread>

namespace yuen_plugin_controller {
class yuenController : public nav2_core::Controller {
public:
  void configure(
      const rclcpp_lifecycle::LifecycleNode::WeakPtr &parent, std::string name,
      std::shared_ptr<tf2_ros::Buffer> tf,
      std::shared_ptr<nav2_costmap_2d::Costmap2DROS> costmap_ros) override;
  void cleanup() override;
  void activate() override;
  void deactivate() override;
  void setPlan(const nav_msgs::msg::Path &path) override;
  // 这是计算当前指令信号的模板函数，该函数接收机器人当前的位姿，速度，和是否达到目标点的信息
  // 需要注意的是，机器人当前的位姿不一定是在全局坐标系下描述的，有可能是在
  // odom或者别的坐标系中描述，因此，在函数中需要先将相应的坐标系转换过来（通过使用坐标转换服务
  // tf_ 即可）
  geometry_msgs::msg::TwistStamped
  computeVelocityCommands(const geometry_msgs::msg::PoseStamped &pose,
                          const geometry_msgs::msg::Twist &velocity,
                          nav2_core::GoalChecker *goal_checker) override;
  void setSpeedLimit(const double &speed_limit,
                     const bool &percentage) override;

protected:
  // 存储插件名称
  std::string plugin_name_;
  // 存储坐标变换缓存指针，可用于查询坐标关系
  std::shared_ptr<tf2_ros::Buffer> tf_;
  // 存储代价地图
  std::shared_ptr<nav2_costmap_2d::Costmap2DROS> costmap_ros_;
  // 存储节点指针
  nav2_util::LifecycleNode::SharedPtr node_;
  // 存储全局代价地图
  nav2_costmap_2d::Costmap2D *costmap_;
  // 存储 setPlan 提供的全局路径
  nav_msgs::msg::Path global_plan_;
  // 参数：最大线速度角速度
  double max_angular_speed_;
  double max_linear_speed_;

  // 获取路径中距离当前点最近的点
  geometry_msgs::msg::PoseStamped
  getNearestTargetPose(const geometry_msgs::msg::PoseStamped &current_pose);
  // 计算目标点方向和当前位置的角度差
  double
  calculateAngleDifference(const geometry_msgs::msg::PoseStamped &current_pose,
                           const geometry_msgs::msg::PoseStamped &target_pose);
};
} // namespace yuen_plugin_controller