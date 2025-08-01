#pragma once

#include <cinttypes>
#include <unordered_map>

#include "esphome/core/component.h"
#include "esphome/core/defines.h"
#include "esphome/components/uart/uart.h"

#ifdef USE_COVER
#include "esphome/components/cover/cover.h"
#endif

namespace esphome {
namespace dooya_bridge {

class DooyaBridge : public Component, public uart::UARTDevice
{
 public:
  void setup() override;
  void loop() override;
  void dump_config() override;

  void register_listener(std::string address, const std::function<void(std::string)> &func) { listeners_[address] = func; }

 protected:
  void parse_rx();

  std::unordered_map<std::string, std::function<void(std::string)>> listeners_;
  std::string rx_buf_;
};

}  // namespace dooya_bridge
}  // namespace esphome