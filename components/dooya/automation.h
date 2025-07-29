#pragma once

#include "esphome/core/automation.h"
#include "esphome/core/helpers.h"

#include "dooya_bridge.h"

namespace esphome {
namespace dooya {

template<typename... Ts> class DooyaBridgePairingAction : public Action<Ts...>, public Parented<DooyaBridge> {
 public:
  void play(Ts... x) { this->parent_->start_pairing(); }
};

}  // namespace dooya
}  // namespace esphome
