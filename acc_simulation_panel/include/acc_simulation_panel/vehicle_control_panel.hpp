#ifndef ACC_SIMULATION_PANEL__VEHICLE_CONTROL_PANEL_HPP_
#define ACC_SIMULATION_PANEL__VEHICLE_CONTROL_PANEL_HPP_

#include <memory>
#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QSpinBox>
#include <rviz_common/panel.hpp>
#include <rclcpp/rclcpp.hpp>
#include <std_srvs/srv/empty.hpp>

class QSpinBox;
class QPushButton;
class QLabel;

namespace acc_simulation_panel {

class VehicleControlPanel : public rviz_common::Panel {
  Q_OBJECT

 public:
  explicit VehicleControlPanel(QWidget* parent = nullptr);
  virtual ~VehicleControlPanel();

  void onInitialize() override;

 private Q_SLOTS:
  void onAddVehicleClicked();
  void onRemoveVehicleClicked();
  void onGetInfoClicked();

 private:
  void updateUI();
  void callAddVehicleService();
  void callRemoveVehicleService();
  void callGetVehicleCountService();

  QPushButton* add_vehicle_button_;
  QPushButton* remove_vehicle_button_;
  QPushButton* get_info_button_;
  QLabel* status_label_;
  QLabel* vehicle_count_label_;
  QSpinBox* vehicle_id_spinbox_;

  // ROS node and service clients
  std::shared_ptr<rclcpp::Node> ros_node_;
  rclcpp::Client<std_srvs::srv::Empty>::SharedPtr add_vehicle_client_;
  rclcpp::Client<std_srvs::srv::Empty>::SharedPtr remove_vehicle_client_;
};

}  // namespace acc_simulation_panel

#endif  // ACC_SIMULATION_PANEL__VEHICLE_CONTROL_PANEL_HPP_
