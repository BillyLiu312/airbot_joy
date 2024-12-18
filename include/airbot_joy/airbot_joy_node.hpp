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

#ifndef AUDIO_INTERACTION__AUDIO_INTERACTION_NODE_HPP_
#define AUDIO_INTERACTION__AUDIO_INTERACTION_NODE_HPP_

#include <memory>
#include <string>
#include <vector>

#include "rcl_interfaces/srv/get_parameters.hpp"
#include "rclcpp/rclcpp.hpp"
#include "sensor_msgs/msg/joy.hpp"
#include "tita_utils/topic_names.hpp"

namespace airbot_joy
{
class AirbotJoyNode : public rclcpp::Node
{
public:
  explicit AirbotJoyNode(const rclcpp::NodeOptions & option);
  ~AirbotJoyNode() = default;

private:
  void get_param(const std::string & param_name);
  void joy_cb(const sensor_msgs::msg::Joy::SharedPtr msg);
  rclcpp::Subscription<sensor_msgs::msg::Joy>::SharedPtr joy_sub_;
  rclcpp::Client<rcl_interfaces::srv::GetParameters>::SharedPtr client_;
  rclcpp::Publisher<sensor_msgs::msg::Joy>::SharedPtr joy_publisher_;

  std::shared_ptr<rclcpp::ParameterEventHandler> param_subscriber_;
  std::shared_ptr<rclcpp::ParameterCallbackHandle> cb_handle_;
  bool use_sdk_{false};
};
}  // namespace airbot_joy
#endif  // AUDIO_INTERACTION__AUDIO_INTERACTION_NODE_HPP_
