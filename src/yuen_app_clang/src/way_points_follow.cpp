#include <memory>

#include "nav2_msgs/action/follow_waypoints.hpp" // 导入导航动作消息的头文件
#include "rclcpp/rclcpp.hpp"               // 导入ROS 2的C++客户端库
#include "rclcpp_action/rclcpp_action.hpp" // 导入ROS 2的C++ Action客户端库

using FAction = nav2_msgs::action::FollowWaypoints;
using FActionClientPtr = rclcpp_action::Client<FAction>::SharedPtr;
using poseStamped = geometry_msgs::msg::PoseStamped;
using goalHandlePtr = rclcpp_action::Client<FAction>::GoalHandle::SharedPtr;
using goalHandleRst = rclcpp_action::ClientGoalHandle<FAction>::WrappedResult;
using feedBackPtr =
    std::shared_ptr<const nav2_msgs::action::FollowWaypoints_Feedback>;

class waypointsFollower : public rclcpp::Node {
private:
  FActionClientPtr client_;

public:
  waypointsFollower(const std::string &node_name) : Node(node_name) {
    client_ = rclcpp_action::create_client<FAction>(this, "follow_waypoints");
    handleProcess();
  }

  void handleProcess() {
    // 等待服务端上线
    while (!client_->wait_for_action_server(std::chrono::seconds(5))) {
      RCLCPP_INFO(get_logger(), "waiting for the serve online......");
    }

    // 设置 waypoints
    auto goal_msgs = FAction::Goal();
    {
      poseStamped goal_pose;
      goal_pose.header.frame_id = "map";
      goal_pose.header.stamp = this->get_clock()->now();
      goal_pose.pose.position.x = 8.0;
      goal_pose.pose.position.y = 0.0;
      goal_pose.pose.orientation.w = 1.0;
      goal_msgs.poses.push_back(goal_pose);
    }
    {
      poseStamped goal_pose;
      goal_pose.header.frame_id = "map";
      goal_pose.header.stamp = this->get_clock()->now();
      goal_pose.pose.position.x = 8.0;
      goal_pose.pose.position.y = 2.0;
      goal_pose.pose.orientation.w = 1.0;
      goal_msgs.poses.push_back(goal_pose);
    }
    {
      poseStamped goal_pose;
      goal_pose.header.frame_id = "map";
      goal_pose.header.stamp = this->get_clock()->now();
      goal_pose.pose.position.x = 2.0;
      goal_pose.pose.position.y = 2.0;
      goal_pose.pose.orientation.w = 1.0;
      goal_msgs.poses.push_back(goal_pose);
    }

    // 设置预设项
    auto goal_option = rclcpp_action::Client<FAction>::SendGoalOptions();
    goal_option.feedback_callback = [&](goalHandlePtr gh,
                                        feedBackPtr fd) -> void {
      (void)gh;
      RCLCPP_INFO(this->get_logger(), "current number of wayPoints is : %u",
                  fd->current_waypoint);
    };
    goal_option.goal_response_callback = [&](goalHandlePtr gh) -> void {
      if (gh) {
        RCLCPP_INFO(this->get_logger(), "target is received by server");
      }
    };
    goal_option.result_callback = [&](const goalHandleRst &rst) -> void {
      if (rst.code == rclcpp_action::ResultCode::SUCCEEDED) {
        RCLCPP_INFO(get_logger(), "handle succeed !!");
      }
    };
    client_->async_send_goal(goal_msgs, goal_option);
  }
};

int main(int argc, char **argv) {
  rclcpp::init(argc, argv);
  auto nodePtr = std::make_shared<waypointsFollower>("waypoints_follower");
  rclcpp::spin(nodePtr);
  rclcpp::shutdown();
  return 0;
}