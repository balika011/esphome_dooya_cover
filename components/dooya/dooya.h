#pragma once

#include <cinttypes>

#include "esphome/core/component.h"
#include "esphome/core/defines.h"
#include "esphome/core/helpers.h"

#include "esphome/components/dooya/dooya_bridge.h"

namespace esphome {
namespace dooya {

#define DOOYA_ADDRESS_GLOBAL std::string("000")

enum DooyaPacketEntryTag : char
{
  ERROR = 'E',
  VERSION = 'v',
  NAME = 'N',
  MOVE = 'm',
  STOP = 's',
  ROTATION = 'r',
  TILT = 'b',
  ADDED = 'A'
};

class DooyaComponent : public Component, public Parented<DooyaBridge>
{
public:
  DooyaComponent() { }
  
  virtual void process_packet(std::vector<std::pair<DooyaPacketEntryTag, std::string>> params) = 0;

  void set_address(std::string address) { address_ = address; }
  std::string get_address() { return address_; };

protected:
  std::string address_;
};

}  // namespace dooya
}  // namespace esphome