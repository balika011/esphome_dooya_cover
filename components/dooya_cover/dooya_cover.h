#pragma once

#include <cinttypes>

#include "esphome/core/component.h"
#include "esphome/components/cover/cover.h"
#include "esphome/components/uart/uart.h"

namespace esphome {
namespace dooya_cover {

class DooyaCover : public Component, public cover::Cover, public uart::UARTDevice
{
 public:
  void setup() override;
  void loop() override;
  void dump_config() override;
  cover::CoverTraits get_traits() override;
  void set_address(std::string address) { address_ = address; }
  
 protected:
  void control(const cover::CoverCall &call) override;
  void parse_rx();

  std::string address_;
  std::string rx_buf_;
  optional<float> new_position_;
  optional<float> new_tilt_;
  bool polling_ = false;
};

}  // namespace dooya_cover
}  // namespace esphome