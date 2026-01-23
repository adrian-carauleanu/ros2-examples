#include "rclcpp/rclcpp.hpp"
#include "acc_simulation/acc_simulation_node.hpp"

int main(int argc, char* argv[]) {
    rclcpp::init(argc, argv);
    auto node = std::make_shared<ACCSimulationNode>();
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}
