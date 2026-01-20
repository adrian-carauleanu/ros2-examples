// Copyright 2016 Open Source Robotics Foundation, Inc.
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

#include <chrono>
#include <memory>
#include <string>

#include "rclcpp/rclcpp.hpp"
#include <rviz_point2d_plugin_msgs/msg/point2_d.hpp>


using namespace std::chrono_literals;

/* This example creates a subclass of Node and uses a fancy C++11 lambda
 * function to shorten the callback syntax, at the expense of making the
 * code somewhat more difficult to understand at first glance. */

class Point2DPublisher : public rclcpp::Node
{
public:
  Point2DPublisher()
  : Node("point2d_publisher"), x_(0),y_(0)
  {
    publisher_ = this->create_publisher<rviz_point2d_plugin_msgs::msg::Point2D>("point2d", 10);
    auto timer_callback =
      [this]() -> void {
        auto message = rviz_point2d_plugin_msgs::msg::Point2D();
        message.header.frame_id = "map";
        constexpr float threshold = 5.0;
        if (x_ > threshold)
        {
          x_ = 0;
          y_++;
        }
        else
          x_++;

        if (y_ > threshold)
          y_ = 0;
            
        message.x = x_;
        message.y = y_;
        RCLCPP_INFO(this->get_logger(), "Publishing point: '(%lf, %lf)'", message.x, message.y);
        this->publisher_->publish(message);
      };
    timer_ = this->create_wall_timer(500ms, timer_callback);
  }

private:
  rclcpp::TimerBase::SharedPtr timer_;
  rclcpp::Publisher<rviz_point2d_plugin_msgs::msg::Point2D>::SharedPtr publisher_;
  float x_, y_;
};

int main(int argc, char * argv[])
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<Point2DPublisher>());
  rclcpp::shutdown();
  return 0;
}
