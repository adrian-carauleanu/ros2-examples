#include <cinttypes>
#include <memory>

//#include "example_interfaces/srv/add_two_ints.hpp"
#include "tutorial_interfaces/srv/add_three_ints.hpp"
#include "rclcpp/rclcpp.hpp"

//using AddTwoInts = example_interfaces::srv::AddTwoInts;
//rclcpp::Node::SharedPtr g_node = nullptr;

// void handle_service(
//   const std::shared_ptr<rmw_request_id_t> request_header,
//   const std::shared_ptr<AddThreeInts::Request> request,
//   const std::shared_ptr<AddThreeInts::Response> response)
// {
//   (void)request_header;
//   RCLCPP_INFO(
//     g_node->get_logger(),
//     "request: %" PRId64 " + %" PRId64, request->a, request->b);
//   response->sum = request->a + request->b;
// }

void add(const std::shared_ptr<tutorial_interfaces::srv::AddThreeInts::Request> request,
          std::shared_ptr<tutorial_interfaces::srv::AddThreeInts::Response> response)
{
  response->sum = request->a + request->b + request->c;
  RCLCPP_INFO(rclcpp::get_logger("rclcpp"), "Incoming request\na: %ld" " b: %ld" " c: %ld",
                request->a, request->b, request->c);                                         
  RCLCPP_INFO(rclcpp::get_logger("rclcpp"), "sending back response: [%ld]", (long int)response->sum);
}

int main(int argc, char ** argv)
{
  rclcpp::init(argc, argv);
  //g_node = rclcpp::Node::make_shared("minimal_service");
  std::shared_ptr<rclcpp::Node> node = rclcpp::Node::make_shared("add_three_ints_server");
  //auto server = g_node->create_service<AddTwoInts>("add_two_ints", handle_service);
  rclcpp::Service<tutorial_interfaces::srv::AddThreeInts>::SharedPtr service =               // CHANGE
    node->create_service<tutorial_interfaces::srv::AddThreeInts>("add_three_ints", &add);
  RCLCPP_INFO(rclcpp::get_logger("rclcpp"), "Ready to add three ints.");
  //rclcpp::spin(g_node);
  rclcpp::spin(node);
  rclcpp::shutdown();
  //g_node = nullptr;
  return 0;
}
