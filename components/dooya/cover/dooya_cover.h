#pragma once

#include <cinttypes>

#include "esphome/core/component.h"

#include "esphome/components/dooya/dooya.h"
#include "esphome/components/cover/cover.h"

namespace esphome {
namespace dooya {

class DooyaCover : public DooyaComponent, public cover::Cover
{
 public:
  void setup() override;
  void loop() override;
  void dump_config() override;
  cover::CoverTraits get_traits() override;

  void process_packet(std::vector<std::pair<DooyaPacketEntryTag, std::string>> params) override;
  
 protected:
  void control(const cover::CoverCall &call) override;

  optional<float> new_position_;
  optional<float> new_tilt_;
  bool polling_ = false;
};

}  // namespace dooya
}  // namespace esphome