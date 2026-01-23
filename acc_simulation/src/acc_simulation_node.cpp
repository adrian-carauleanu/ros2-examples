#include "acc_simulation/acc_simulation_node.hpp"
#include <cmath>
#include <memory>

ACCSimulationNode::ACCSimulationNode(const std::string& node_name)
    : rclcpp::Node(node_name),
      simulation_(0.05),  // 50ms time step
      road_(500.0, 3.5, 2.0, 10.0),
      update_frequency_(20.0)  // 20 Hz
{
    // Declare parameters
    this->declare_parameter("desired_speed", 20.0);
    this->declare_parameter("desired_distance", 30.0);
    this->declare_parameter("kp", 0.5);
    this->declare_parameter("kv", 0.3);
    this->declare_parameter("update_frequency", 20.0);

    // Get parameters
    double desired_speed = this->get_parameter("desired_speed").as_double();
    double desired_distance = this->get_parameter("desired_distance").as_double();
    double kp = this->get_parameter("kp").as_double();
    double kv = this->get_parameter("kv").as_double();
    update_frequency_ = this->get_parameter("update_frequency").as_double();

    // Create publisher
    marker_pub_ = this->create_publisher<visualization_msgs::msg::MarkerArray>(
        "visualization_marker_array", 10);

    // Create two vehicles: lead and ego (running vertically along Y-axis)
    auto lead_vehicle = std::make_unique<Vehicle>(
        0, road_.getLaneYPosition(0), 100.0, 4.5, 2.0);
    lead_vehicle->setVelocity(15.0);  // Constant velocity
    lead_vehicle->setHeading(M_PI / 2.0);  // Point upward (90 degrees)

    auto ego_vehicle = std::make_unique<Vehicle>(
        1, road_.getLaneYPosition(0), 50.0, 4.5, 2.0);
    ego_vehicle->setVelocity(10.0);  // Starting slower
    ego_vehicle->setHeading(M_PI / 2.0);  // Point upward (90 degrees)

    simulation_.addVehicle(std::move(lead_vehicle));
    simulation_.addVehicle(std::move(ego_vehicle));

    // Create and set ACC controller
    auto controller = std::make_unique<SimpleACCController>(
        desired_speed, desired_distance, kp, kv);
    simulation_.setController(std::move(controller));

    // Create timer for simulation updates
    int period_ms = static_cast<int>(1000.0 / update_frequency_);
    timer_ = this->create_wall_timer(
        std::chrono::milliseconds(period_ms),
        std::bind(&ACCSimulationNode::simulationCallback, this));

    RCLCPP_INFO(this->get_logger(), "ACC Simulation Node initialized");
}

visualization_msgs::msg::MarkerArray ACCSimulationNode::createMarkers() {
    visualization_msgs::msg::MarkerArray marker_array;
    
    createRoadMarkers(marker_array);
    createVehicleMarkers(marker_array);

    return marker_array;
}

void ACCSimulationNode::createRoadMarkers(
    visualization_msgs::msg::MarkerArray& markers) {
    // Create road surface (vertical along Y-axis)
    visualization_msgs::msg::Marker road_marker;
    road_marker.header.frame_id = "map";
    road_marker.header.stamp = this->now();
    road_marker.ns = "road";
    road_marker.id = 0;
    road_marker.type = visualization_msgs::msg::Marker::CUBE;
    road_marker.action = visualization_msgs::msg::Marker::ADD;
    
    road_marker.pose.position.x = 0.0;
    road_marker.pose.position.y = road_.getLength() / 2.0;
    road_marker.pose.position.z = -0.1;
    road_marker.pose.orientation.w = 1.0;
    
    road_marker.scale.x = road_.getWidth() * road_.getNumLanes();
    road_marker.scale.y = road_.getLength();
    road_marker.scale.z = 0.05;
    
    road_marker.color.r = 0.3;
    road_marker.color.g = 0.3;
    road_marker.color.b = 0.3;
    road_marker.color.a = 1.0;
    
    markers.markers.push_back(road_marker);

    // Create lane markings (vertical along Y-axis)
    int marking_id = 1;
    for (double y = 0.0; y < road_.getLength(); 
         y += road_.getLaneMarkingDistance()) {
        visualization_msgs::msg::Marker marking;
        marking.header.frame_id = "map";
        marking.header.stamp = this->now();
        marking.ns = "lane_markings";
        marking.id = marking_id++;
        marking.type = visualization_msgs::msg::Marker::CUBE;
        marking.action = visualization_msgs::msg::Marker::ADD;
        
        marking.pose.position.x = 0.0;
        marking.pose.position.y = y;
        marking.pose.position.z = 0.01;
        marking.pose.orientation.w = 1.0;
        
        marking.scale.x = 0.15;
        marking.scale.y = 0.5;
        marking.scale.z = 0.02;
        
        marking.color.r = 1.0;
        marking.color.g = 1.0;
        marking.color.b = 1.0;
        marking.color.a = 1.0;
        
        markers.markers.push_back(marking);
    }
}

void ACCSimulationNode::createVehicleMarkers(
    visualization_msgs::msg::MarkerArray& markers) {
    const auto& vehicles = simulation_.getVehicles();
    
    for (size_t i = 0; i < vehicles.size(); ++i) {
        const auto& vehicle = vehicles[i];
        const auto& state = vehicle->getState();
        
        visualization_msgs::msg::Marker vehicle_marker;
        vehicle_marker.header.frame_id = "map";
        vehicle_marker.header.stamp = this->now();
        vehicle_marker.ns = "vehicles";
        vehicle_marker.id = i;
        vehicle_marker.type = visualization_msgs::msg::Marker::CUBE;
        vehicle_marker.action = visualization_msgs::msg::Marker::ADD;
        
        vehicle_marker.pose.position.x = state.position_x;
        vehicle_marker.pose.position.y = state.position_y;
        vehicle_marker.pose.position.z = 1.0;
        
        // Set orientation based on heading
        double cy = std::cos(state.heading * 0.5);
        double sy = std::sin(state.heading * 0.5);
        vehicle_marker.pose.orientation.w = cy;
        vehicle_marker.pose.orientation.z = sy;
        
        vehicle_marker.scale.x = vehicle->getLength();
        vehicle_marker.scale.y = vehicle->getWidth();
        vehicle_marker.scale.z = 1.5;
        
        // Color: lead vehicle in green, ego in red
        if (i == 0) {  // Lead vehicle
            vehicle_marker.color.r = 0.0;
            vehicle_marker.color.g = 1.0;
            vehicle_marker.color.b = 0.0;
        } else {  // Ego vehicle
            vehicle_marker.color.r = 1.0;
            vehicle_marker.color.g = 0.0;
            vehicle_marker.color.b = 0.0;
        }
        vehicle_marker.color.a = 1.0;
        
        markers.markers.push_back(vehicle_marker);
        
        // Add velocity vector
        visualization_msgs::msg::Marker velocity_marker;
        velocity_marker.header.frame_id = "map";
        velocity_marker.header.stamp = this->now();
        velocity_marker.ns = "velocity_vectors";
        velocity_marker.id = i;
        velocity_marker.type = visualization_msgs::msg::Marker::ARROW;
        velocity_marker.action = visualization_msgs::msg::Marker::ADD;
        
        velocity_marker.points.resize(2);
        velocity_marker.points[0].x = state.position_x;
        velocity_marker.points[0].y = state.position_y;
        velocity_marker.points[0].z = 1.75;
        
        velocity_marker.points[1].x = state.position_x + 
                                      state.velocity * std::cos(state.heading) * 0.5;
        velocity_marker.points[1].y = state.position_y + 
                                      state.velocity * std::sin(state.heading) * 0.5;
        velocity_marker.points[1].z = 1.75;
        
        velocity_marker.scale.x = 0.3;  // arrow shaft radius
        velocity_marker.scale.y = 0.6;  // arrow head radius
        velocity_marker.scale.z = 0.0;
        
        velocity_marker.color.r = 0.0;
        velocity_marker.color.g = 0.0;
        velocity_marker.color.b = 1.0;
        velocity_marker.color.a = 0.8;
        
        markers.markers.push_back(velocity_marker);
    }
}

void ACCSimulationNode::simulationCallback() {
    // Update simulation
    simulation_.update();
    
    // Wrap vehicles around when they exceed road length (vertical Y-axis)
    const auto& vehicles = simulation_.getVehicles();
    double road_length = road_.getLength();
    for (auto& vehicle : vehicles) {
        // Wrap position back to start if it exceeds road length
        if (vehicle->getPositionY() > road_length) {
            vehicle->setPosition(
                vehicle->getState().position_x,
                std::fmod(vehicle->getPositionY(), road_length)
            );
        }
    }
    
    // Create and publish markers
    auto marker_array = createMarkers();
    marker_pub_->publish(marker_array);
    
    // Log info every 100 updates
    static int update_count = 0;
    if (++update_count % 100 == 0) {
        const auto& vehicles = simulation_.getVehicles();
        if (vehicles.size() >= 2) {
            RCLCPP_INFO(
                this->get_logger(),
                "Time: %.2fs | Lead: y=%.1f, v=%.1f m/s | "
                "Ego: y=%.1f, v=%.1f m/s, a=%.2f m/s²",
                simulation_.getSimulationTime(),
                vehicles[0]->getPositionY(),
                vehicles[0]->getVelocity(),
                vehicles[1]->getPositionY(),
                vehicles[1]->getVelocity(),
                vehicles[1]->getState().acceleration);
        }
    }
}
