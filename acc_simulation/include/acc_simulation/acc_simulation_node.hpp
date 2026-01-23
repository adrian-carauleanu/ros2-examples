#ifndef ACC_SIMULATION__ACC_SIMULATION_NODE_HPP_
#define ACC_SIMULATION__ACC_SIMULATION_NODE_HPP_

#include "rclcpp/rclcpp.hpp"
#include "visualization_msgs/msg/marker.hpp"
#include "visualization_msgs/msg/marker_array.hpp"
#include "acc_simulation/vehicle.hpp"
#include "acc_simulation/acc_controller.hpp"
#include "acc_simulation/road.hpp"

/**
 * @brief ROS 2 node for adaptive cruise control simulation with RViz visualization
 * 
 * Manages the simulation lifecycle and publishes visualization markers
 */
class ACCSimulationNode : public rclcpp::Node {
private:
    rclcpp::Publisher<visualization_msgs::msg::MarkerArray>::SharedPtr marker_pub_;
    rclcpp::TimerBase::SharedPtr timer_;
    
    ACCSimulation simulation_;
    Road road_;
    double update_frequency_;

    /**
     * @brief Create visualization markers for vehicles and road
     * @return Array of markers for RViz
     */
    visualization_msgs::msg::MarkerArray createMarkers();

    /**
     * @brief Create road lane markers
     * @param markers Marker array to append to
     */
    void createRoadMarkers(visualization_msgs::msg::MarkerArray& markers);

    /**
     * @brief Create vehicle markers
     * @param markers Marker array to append to
     */
    void createVehicleMarkers(visualization_msgs::msg::MarkerArray& markers);

    /**
     * @brief Simulation update callback
     */
    void simulationCallback();

public:
    explicit ACCSimulationNode(const std::string& node_name = "acc_simulation");

    ~ACCSimulationNode() override = default;
};

#endif  // ACC_SIMULATION__ACC_SIMULATION_NODE_HPP_
