#pragma once

#include "esphome/core/automation.h"
#include "esphome/core/helpers.h"

#include "dooya_bridge.h"

namespace esphome {
namespace dooya_bridge {

template<typename... Ts> class DooyaBridgePairingAction : public Action<Ts...>, public Parented<DooyaBridge> {
 public:
  void play(Ts... x) { this->parent_->start_pairing(); }
};

}  // namespace dooya_bridge
}  // namespace esphome
