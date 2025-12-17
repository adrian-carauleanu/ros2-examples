#include <chrono>
#include <cinttypes>
#include <memory>

//#include "example_interfaces/srv/add_two_ints.hpp"
#include "tutorial_interfaces/srv/add_three_ints.hpp"
#include "rclcpp/rclcpp.hpp"

//using AddTwoInts = example_interfaces::srv::AddTwoInts;

int main(int argc, char * argv[])
{
  rclcpp::init(argc, argv);
  //auto node = rclcpp::Node::make_shared("minimal_client");
  std::shared_ptr<rclcpp::Node> node = rclcpp::Node::make_shared("add_three_ints_client");
  //auto client = node->create_client<AddTwoInts>("add_two_ints");
  rclcpp::Client<tutorial_interfaces::srv::AddThreeInts>::SharedPtr client = 
            node->create_client<tutorial_interfaces::srv::AddThreeInts>("add_three_ints");
            
  while (!client->wait_for_service(std::chrono::seconds(1))) {
    if (!rclcpp::ok()) {
      RCLCPP_ERROR(node->get_logger(), "client interrupted while waiting for service to appear.");
      return 1;
    }
    RCLCPP_INFO(node->get_logger(), "waiting for service to appear...");
  }

  while(true)
  {
    //auto request = std::make_shared<AddTwoInts::Request>();
    auto request = std::make_shared<tutorial_interfaces::srv::AddThreeInts::Request>();
    //std::cout << "Enter two integers to add (enter 'q' to exit)" << std::endl;
    std::cout << "Enter three integers to add (enter 'q' to exit)" << std::endl;
    std::string input;
    
    std::cout << "First number: ";
    std::cin >> input;
    if(input == "q") {
      break;
    }
    request->a = std::stoll(input);
    
    std::cout << "Second number: ";
    std::cin >> input;
    if(input == "q") {
      break;
    }
    request->b = std::stoll(input);

    std::cout << "Third number: ";
    std::cin >> input;
    if(input == "q") {
      break;
    }
    request->c = std::stoll(input); 

    auto result_future = client->async_send_request(request);
  //   if (rclcpp::spin_until_future_complete(node, result_future) !=
  //       rclcpp::FutureReturnCode::SUCCESS)
  //   {
  //       RCLCPP_ERROR(node->get_logger(), "service call failed :(");
  //       client->remove_pending_request(result_future);
  //       return 1;
  //   }
  //   auto result = result_future.get();
  //   RCLCPP_INFO(
  //       node->get_logger(), "result of %" PRId64 " + %" PRId64 " = %" PRId64,
  //       request->a, request->b, result->sum);
  // }
    if (rclcpp::spin_until_future_complete(node, result_future) ==
      rclcpp::FutureReturnCode::SUCCESS)
    {
      RCLCPP_INFO(rclcpp::get_logger("rclcpp"), "Sum: %ld", result_future.get()->sum);
    } else {
      RCLCPP_ERROR(rclcpp::get_logger("rclcpp"), "Failed to call service add_three_ints");
    }
  }
  rclcpp::shutdown();
  return 0;
}
