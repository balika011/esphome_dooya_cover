#pragma once

#include <cinttypes>

#include "esphome/core/component.h"
#include "esphome/components/dooya/dooya_bridge.h"
#include "esphome/components/cover/cover.h"
#include "esphome/components/uart/uart.h"

namespace esphome {
namespace dooya_cover {

class DooyaCover : public Component, public cover::Cover, public Parented<DooyaBridge>
{
 public:
  void setup() override;
  void loop() override;
  void dump_config() override;
  cover::CoverTraits get_traits() override;
  void set_parent(dooya_bridge::DooyaBridge *parent) { this->parent_ = parent; }
  void set_address(std::string address) { address_ = address; }
  
 protected:
  void control(const cover::CoverCall &call) override;
  void parse_rx(std::string rx);

  dooya_bridge::DooyaBridge *parent_;
  std::string address_;
  optional<float> new_position_;
  optional<float> new_tilt_;
  bool polling_ = false;
};

}  // namespace dooya_cover
}  // namespace esphome