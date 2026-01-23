#include "rclcpp/rclcpp.hpp"
#include "acc_simulation/acc_simulation_node.hpp"
#include <signal.h>
#include <atomic>

std::atomic<bool> shutdown_requested(false);

void signal_handler(int signal) {
    if (signal == SIGINT) {
        shutdown_requested = true;
        rclcpp::shutdown();
    }
}

int main(int argc, char* argv[]) {
    rclcpp::init(argc, argv);
    
    // Register signal handler
    signal(SIGINT, signal_handler);
    
    auto node = std::make_shared<ACCSimulationNode>();
    
    try {
        rclcpp::spin(node);
    } catch (const std::exception& e) {
        RCLCPP_ERROR(node->get_logger(), "Exception in spin: %s", e.what());
    }
    
    rclcpp::shutdown();
    return 0;
}
