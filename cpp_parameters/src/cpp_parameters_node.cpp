#include <chrono>
#include <memory>
#include <string>

#include "rclcpp/rclcpp.hpp"
using namespace std::chrono_literals;

class ParameterNode : public rclcpp::Node
{
public:
    ParameterNode()
    : Node("cpp_parameters_node")
    {
        auto param_desc = rcl_interfaces::msg::ParameterDescriptor{};
        param_desc.description = "Declared 'my_parameter' with value 'parameter_default_value' in C++ code";
        this->declare_parameter("my_parameter", "parameter_default_value", param_desc);

        auto timer_callback = [this](){
        std::string my_param = this->get_parameter("my_parameter").as_string();

        RCLCPP_INFO(this->get_logger(), "Parameter 'my_parameter' has value '%s'", my_param.c_str());

        std::vector<rclcpp::Parameter> all_new_parameters{rclcpp::Parameter("my_parameter", "parameter_default_value_updated")};
        this->set_parameters(all_new_parameters);
        };
        timer_ = this->create_wall_timer(1000ms, timer_callback);
    }
private:
    rclcpp::TimerBase::SharedPtr timer_;
};
int main(int argc, char * argv[])
{
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<ParameterNode>());
    rclcpp::shutdown();
    return 0;
}