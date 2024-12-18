// Copyright (c) 2023 Direct Drive Technology Co., Ltd. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "airbot_joy/airbot_joy_node.hpp"

using namespace std::chrono_literals;

namespace airbot_joy
{
AirbotJoyNode::AirbotJoyNode(const rclcpp::NodeOptions & options) : Node("airbot_joy_node", options)
{
  std::string log_level;
  this->get_parameter("log_level", log_level);
  if (log_level == "DEBUG") {
    this->get_logger().set_level(rclcpp::Logger::Level::Debug);
  } else if (log_level == "INFO") {
    this->get_logger().set_level(rclcpp::Logger::Level::Info);
  } else if (log_level == "WARN") {
    this->get_logger().set_level(rclcpp::Logger::Level::Warn);
  } else if (log_level == "ERROR") {
    this->get_logger().set_level(rclcpp::Logger::Level::Error);
  } else if (log_level == "FATAL") {
    this->get_logger().set_level(rclcpp::Logger::Level::Fatal);
  } else {
    RCLCPP_WARN(this->get_logger(), "Unknown log level: %s, defaulting to INFO", log_level.c_str());
    this->get_logger().set_level(rclcpp::Logger::Level::Info);
  }

  RCLCPP_DEBUG(this->get_logger(), "Audio interaction node is started");
  joy_sub_ = this->create_subscription<sensor_msgs::msg::Joy>(
    tita_topic::joy, 10, std::bind(&AirbotJoyNode::joy_cb, this, std::placeholders::_1));
  param_subscriber_ = std::make_shared<rclcpp::ParameterEventHandler>(this);
  joy_publisher_ = this->create_publisher<sensor_msgs::msg::Joy>("/airbot_play/joy", 10);

  client_ =
    this->create_client<rcl_interfaces::srv::GetParameters>("active_command_node/get_parameters");
  while (!client_->wait_for_service(std::chrono::seconds(1))) {
    RCLCPP_INFO(this->get_logger(), "Waiting for parameter service...");
  }

  // RCLCPP_INFO(this->get_logger(), "Parameter service is available.");

  // // 调用服务获取参数
  // get_parameter("use_sdk");
  auto cb = [this](const rclcpp::Parameter & p) {
    RCLCPP_INFO(
      this->get_logger(), "cb2: Received an update to parameter \"%s\" of type: %s: \"%d\"",
      p.get_name().c_str(), p.get_type_name().c_str(), p.as_bool());
    use_sdk_ = p.as_bool();
  };
  cb_handle_ = param_subscriber_->add_parameter_callback("use_sdk", cb, "active_command_node");
}
void AirbotJoyNode::joy_cb(const sensor_msgs::msg::Joy::SharedPtr msg)
{
  // get_param("use_sdk");
  if (use_sdk_) {
    auto new_msg = std::make_shared<sensor_msgs::msg::Joy>();
    new_msg->header = msg->header;
    new_msg->axes.resize(6);
    new_msg->buttons.resize(12);
    new_msg->axes[0] = msg->axes[3];
    new_msg->axes[1] = -msg->axes[2];
    new_msg->axes[2] = msg->axes[0];
    new_msg->axes[3] = -msg->axes[1];
    if(msg->axes[6] == -1){
      new_msg->buttons[5] = 1;
    }
    else{
      new_msg->buttons[5] = 0;
    }

    joy_publisher_->publish(*new_msg);
    RCLCPP_DEBUG(this->get_logger(), "Received joy message");
  }
}
// not use 
void AirbotJoyNode::get_param(const std::string & param_name)
{
  // 创建请求对象
  auto request = std::make_shared<rcl_interfaces::srv::GetParameters::Request>();
  request->names.push_back(param_name);

  // 异步调用服务
  auto future = client_->async_send_request(request);

  // 等待结果
  if (
    rclcpp::spin_until_future_complete(this->shared_from_this(), future) ==
    rclcpp::FutureReturnCode::SUCCESS) {
    auto response = future.get();
    if (
      !response->values.empty() &&
      response->values[0].type == rcl_interfaces::msg::ParameterType::PARAMETER_BOOL) {
      bool use_sdk = response->values[0].bool_value;
      RCLCPP_INFO(this->get_logger(), "参数 use_sdk 的值: %s", use_sdk ? "true" : "false");
    } else {
      RCLCPP_WARN(this->get_logger(), "未找到参数或参数类型不匹配");
    }
  } else {
    RCLCPP_ERROR(this->get_logger(), "获取参数失败");
  }
}

}  // namespace airbot_joy
