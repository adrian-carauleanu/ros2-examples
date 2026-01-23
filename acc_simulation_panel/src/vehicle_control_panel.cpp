#include "acc_simulation_panel/vehicle_control_panel.hpp"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QSpinBox>
#include <QGroupBox>

namespace acc_simulation_panel {

VehicleControlPanel::VehicleControlPanel(QWidget* parent)
    : rviz_common::Panel(parent) {
  auto layout = new QVBoxLayout();

  // Title
  auto title_label = new QLabel("Vehicle Control Panel");
  QFont title_font = title_label->font();
  title_font.setPointSize(12);
  title_font.setBold(true);
  title_label->setFont(title_font);
  layout->addWidget(title_label);

  // Status section
  auto status_group = new QGroupBox("Status");
  auto status_layout = new QVBoxLayout();
  vehicle_count_label_ = new QLabel("Connected Vehicles: --");
  status_label_ = new QLabel("Ready");
  status_layout->addWidget(vehicle_count_label_);
  status_layout->addWidget(status_label_);
  status_group->setLayout(status_layout);
  layout->addWidget(status_group);

  // Control section
  auto control_group = new QGroupBox("Vehicle Management");
  auto control_layout = new QVBoxLayout();

  // Add vehicle button
  auto add_button_layout = new QHBoxLayout();
  add_vehicle_button_ = new QPushButton("Add Vehicle");
  connect(add_vehicle_button_, SIGNAL(clicked()), this,
          SLOT(onAddVehicleClicked()));
  add_button_layout->addWidget(add_vehicle_button_);
  control_layout->addLayout(add_button_layout);

  // Remove vehicle button
  auto remove_button_layout = new QHBoxLayout();
  remove_vehicle_button_ = new QPushButton("Remove Vehicle");
  connect(remove_vehicle_button_, SIGNAL(clicked()), this,
          SLOT(onRemoveVehicleClicked()));
  remove_button_layout->addWidget(remove_vehicle_button_);
  control_layout->addLayout(remove_button_layout);

  // Info button
  auto info_button_layout = new QHBoxLayout();
  get_info_button_ = new QPushButton("Update Info");
  connect(get_info_button_, SIGNAL(clicked()), this,
          SLOT(onGetInfoClicked()));
  info_button_layout->addWidget(get_info_button_);
  control_layout->addLayout(info_button_layout);

  control_group->setLayout(control_layout);
  layout->addWidget(control_group);

  layout->addStretch();
  setLayout(layout);

  setWindowTitle("Vehicle Control");
}

VehicleControlPanel::~VehicleControlPanel() = default;

void VehicleControlPanel::onInitialize() {
  // Create ROS node for service calls
  if (!rclcpp::ok()) {
    rclcpp::init(0, nullptr);
  }
  
  ros_node_ = std::make_shared<rclcpp::Node>("vehicle_control_panel_node");
  
  // Create service clients
  add_vehicle_client_ = ros_node_->create_client<std_srvs::srv::Empty>("add_vehicle");
  remove_vehicle_client_ = ros_node_->create_client<std_srvs::srv::Empty>("remove_vehicle");
  
  status_label_->setText("Connected to ACC Simulation");
}

void VehicleControlPanel::onAddVehicleClicked() {
  callAddVehicleService();
}

void VehicleControlPanel::onRemoveVehicleClicked() {
  callRemoveVehicleService();
}

void VehicleControlPanel::onGetInfoClicked() {
  updateUI();
}

void VehicleControlPanel::updateUI() {
  status_label_->setText("Info updated");
}

void VehicleControlPanel::callAddVehicleService() {
  if (!add_vehicle_client_) {
    status_label_->setText("Service client not initialized");
    return;
  }
  
  if (!add_vehicle_client_->wait_for_service(std::chrono::seconds(1))) {
    status_label_->setText("Add vehicle service unavailable");
    return;
  }
  
  auto request = std::make_shared<std_srvs::srv::Empty::Request>();
  auto future = add_vehicle_client_->async_send_request(request);
  
  status_label_->setText("Adding vehicle...");
}

void VehicleControlPanel::callRemoveVehicleService() {
  if (!remove_vehicle_client_) {
    status_label_->setText("Service client not initialized");
    return;
  }
  
  if (!remove_vehicle_client_->wait_for_service(std::chrono::seconds(1))) {
    status_label_->setText("Remove vehicle service unavailable");
    return;
  }
  
  auto request = std::make_shared<std_srvs::srv::Empty::Request>();
  auto future = remove_vehicle_client_->async_send_request(request);
  
  status_label_->setText("Removing vehicle...");
}

void VehicleControlPanel::callGetVehicleCountService() {
  vehicle_count_label_->setText("Connected Vehicles: N/A");
}

}  // namespace acc_simulation_panel

#include <pluginlib/class_list_macros.hpp>
PLUGINLIB_EXPORT_CLASS(acc_simulation_panel::VehicleControlPanel,
                       rviz_common::Panel)
