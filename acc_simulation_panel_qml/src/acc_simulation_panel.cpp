#include "acc_simulation_panel/acc_simulation_panel.hpp"
#include <ament_index_cpp/get_package_share_directory.hpp>
#include <qboxlayout.h>
#include <QQuickWindow>

namespace acc_simulation_panel_qml {

AccSimulationPanel::AccSimulationPanel(QWidget* parent): rviz_common::Panel(parent) 
{
    // This tells Qt not to use OpenGL for QML rendering
    QQuickWindow::setSceneGraphBackend("software");

    // Set this BEFORE creating the QQuickWidget
    this->setAttribute(Qt::WA_AlwaysStackOnTop);
    qml_widget_ = new QQuickWidget(this);
    // Then set the clear color to transparent so it doesn't "block" the background
    qml_widget_->setClearColor(Qt::transparent);

    // Connect C++ class to QML context
    qml_widget_->rootContext()->setContextProperty("panel", this);
    
    // Load the QML file (ensure this is in your install share folder)
    std::string package_share_directory = ament_index_cpp::get_package_share_directory("acc_simulation_panel_qml");
    std::string qml_path = package_share_directory + "/../../include/acc_simulation_panel/acc_simulation_panel.qml";
    RCLCPP_WARN(rclcpp::get_logger("panel"), "QML path: %s", qml_path.c_str());
    qml_widget_->setSource(QUrl::fromLocalFile(QString::fromStdString(qml_path)));    
    qml_widget_->setResizeMode(QQuickWidget::SizeRootObjectToView);

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(qml_widget_);
}

AccSimulationPanel::~AccSimulationPanel() = default;

void AccSimulationPanel::onInitialize() 
{
  // Create ROS node for service calls
  if (!rclcpp::ok()) {
    rclcpp::init(0, nullptr);
  }
  
  ros_node_ = std::make_shared<rclcpp::Node>("acc_simulation_panel_node");
  
  // Create service clients
  add_vehicle_client_ = ros_node_->create_client<std_srvs::srv::Empty>("add_vehicle");
  remove_vehicle_client_ = ros_node_->create_client<std_srvs::srv::Empty>("remove_vehicle");
  set_vehicle_distance_client_ = ros_node_->create_client<acc_simulation_interfaces::srv::SetAccVehicleDistance>("set_vehicle_distance");
  
  status_label_ = "Connected to ACC Simulation";
}

void AccSimulationPanel::onAddVehicleClicked() {
  callAddVehicleService();
}

void AccSimulationPanel::onRemoveVehicleClicked() {
  callRemoveVehicleService();
}

void AccSimulationPanel::callAddVehicleService() {
  if (!add_vehicle_client_) {
    status_label_ = "Service client not initialized";
    return;
  }
  
  if (!add_vehicle_client_->wait_for_service(std::chrono::seconds(1))) {
    status_label_ = "Add vehicle service unavailable";
    return;
  }
  
  auto request = std::make_shared<std_srvs::srv::Empty::Request>();
  auto future = add_vehicle_client_->async_send_request(request);

  status_label_ = "Adding vehicle...";
}

void AccSimulationPanel::callRemoveVehicleService() {
  if (!remove_vehicle_client_) {
    status_label_ = "Service client not initialized";
    return;
  }
  
  if (!remove_vehicle_client_->wait_for_service(std::chrono::seconds(1))) {
    status_label_ = "Remove vehicle service unavailable";
    return;
  }
  
  auto request = std::make_shared<std_srvs::srv::Empty::Request>();
  auto future = remove_vehicle_client_->async_send_request(request);

  status_label_ = "Removing vehicle...";
}

void AccSimulationPanel::callGetVehicleCountService() {
  vehicle_count_ = 0;
}

void AccSimulationPanel::onDistanceChanged(double value) 
{
  desired_distance_ = value;
  status_label_ = QString("Desired distance set to ") + 
                         QString::number(value, 'f', 1) + " m";
          
  if (set_vehicle_distance_client_) 
  {
    if (!set_vehicle_distance_client_->wait_for_service(std::chrono::seconds(1))) 
    {
      status_label_ = "Set vehicle distance service unavailable";
      return;
    }

    auto logger = rclcpp::get_logger("my_panel_logger");
    RCLCPP_WARN(logger, 
            "Calling the SetAccVehicleDistance service to set distance to: %f meters.",
            value);
    auto distance_request = std::make_shared<acc_simulation_interfaces::srv::SetAccVehicleDistance::Request>();
    distance_request->vehicle_distance = value;
    auto future = set_vehicle_distance_client_->async_send_request(distance_request);
  }
}
}  // namespace acc_simulation_panel_qml

#include <pluginlib/class_list_macros.hpp>
PLUGINLIB_EXPORT_CLASS(acc_simulation_panel_qml::AccSimulationPanel,
                       rviz_common::Panel)
