#ifndef ACC_SIMULATION__ACC_SIMULATION_NODE_HPP_
#define ACC_SIMULATION__ACC_SIMULATION_NODE_HPP_

#include "rclcpp/rclcpp.hpp"
#include "visualization_msgs/msg/marker.hpp"
#include "visualization_msgs/msg/marker_array.hpp"
#include "std_srvs/srv/empty.hpp"
#include "acc_simulation/vehicle.hpp"
#include "acc_simulation/acc_controller.hpp"
#include "acc_simulation/road.hpp"
#include "acc_simulation_interfaces/srv/set_acc_vehicle_distance.hpp"
/**
 * @brief ROS 2 node for adaptive cruise control simulation with RViz visualization
 */
class ACCSimulationNode : public rclcpp::Node {
private:
    rclcpp::Publisher<visualization_msgs::msg::MarkerArray>::SharedPtr marker_pub_;
    rclcpp::TimerBase::SharedPtr timer_;
    
    ACCSimulation simulation_;
    Road road_;
    double update_frequency_;
    size_t last_vehicle_count_ = 0;  // Track for marker deletion

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

    /**
     * @brief Add a new vehicle to the simulation
     */
    void handleAddVehicle(const std::shared_ptr<std_srvs::srv::Empty::Request> request,
                          std::shared_ptr<std_srvs::srv::Empty::Response> response);

    /**
     * @brief Remove the last vehicle from the simulation
     */
    void handleRemoveVehicle(const std::shared_ptr<std_srvs::srv::Empty::Request> request,
                             std::shared_ptr<std_srvs::srv::Empty::Response> response);

    /**
     * @brief Set the desired distance for vehicles
     */
    void handleSetVehicleDistance(const std::shared_ptr<acc_simulation_interfaces::srv::SetAccVehicleDistance::Request> request,
                                  std::shared_ptr<acc_simulation_interfaces::srv::SetAccVehicleDistance::Response> response);

    rclcpp::Service<std_srvs::srv::Empty>::SharedPtr add_vehicle_service_;
    rclcpp::Service<std_srvs::srv::Empty>::SharedPtr remove_vehicle_service_;
    rclcpp::Service<acc_simulation_interfaces::srv::SetAccVehicleDistance>::SharedPtr set_vehicle_distance_service_;
public:
    explicit ACCSimulationNode(const std::string& node_name = "acc_simulation");

    ~ACCSimulationNode() override;
};

#endif  // ACC_SIMULATION__ACC_SIMULATION_NODE_HPP_
