#ifndef ACC_SIMULATION_PANEL__ACC_SIMULATION_PANEL_HPP_
#define ACC_SIMULATION_PANEL__ACC_SIMULATION_PANEL_HPP_

#include <memory>
#include <QQuickWidget> // The bridge to QML
#include <QQmlContext>
#include <rviz_common/panel.hpp>
#include <rclcpp/rclcpp.hpp>
#include "acc_simulation_interfaces/srv/set_acc_vehicle_distance.hpp"
#include <std_srvs/srv/empty.hpp>

namespace acc_simulation_panel_qml {

class AccSimulationPanel : public rviz_common::Panel {
  Q_OBJECT
  // Example property to update the UI from ROS
  Q_PROPERTY(int vehicleCount READ getVehicleCount NOTIFY vehicleCountChanged)
  Q_PROPERTY(double desiredDistance READ getDesiredDistance NOTIFY desiredDistanceChanged)
  Q_PROPERTY(QString statusLabel READ getStatusLabel NOTIFY statusLabelChanged)
  
 public:
  explicit AccSimulationPanel(QWidget* parent = nullptr);
  virtual ~AccSimulationPanel();

  void onInitialize() override;
  int getVehicleCount() const { return vehicle_count_; }
  double getDesiredDistance() const { return desired_distance_; }
  QString getStatusLabel() const { return status_label_; }

  Q_SIGNALS:
  void vehicleCountChanged();
  void desiredDistanceChanged();
  void statusLabelChanged();

 public Q_SLOTS:
  void onAddVehicleClicked();
  void onRemoveVehicleClicked();
  void onDistanceChanged(double value);

 private:
  void callAddVehicleService();
  void callRemoveVehicleService();
  void callGetVehicleCountService();

  QQuickWidget* qml_widget_;
  int vehicle_count_ = 0;
  double desired_distance_ = 0.0;
  QString status_label_ = "";

  // ROS node and service clients
  std::shared_ptr<rclcpp::Node> ros_node_;
  rclcpp::Client<std_srvs::srv::Empty>::SharedPtr add_vehicle_client_;
  rclcpp::Client<std_srvs::srv::Empty>::SharedPtr remove_vehicle_client_;
  rclcpp::Client<acc_simulation_interfaces::srv::SetAccVehicleDistance>::SharedPtr set_vehicle_distance_client_;
};

}  // namespace acc_simulation_panel_qml

#endif  // ACC_SIMULATION_PANEL__ACC_SIMULATION_PANEL_HPP_
