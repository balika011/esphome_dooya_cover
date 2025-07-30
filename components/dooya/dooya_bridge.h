#pragma once

#include <cinttypes>
#include <unordered_map>
#include <chrono>

#include "esphome/core/component.h"
#include "esphome/core/defines.h"
#include "esphome/core/helpers.h"

#include "esphome/components/uart/uart.h"

namespace esphome {
namespace dooya {

class DooyaComponent;

class DooyaBridge : public Component, public uart::UARTDevice
{
 public:
  void setup() override;
  void loop() override;
  void dump_config() override;

  void register_subcomponent(DooyaComponent *component) { subcomponents_.push_back(component); }

  bool start_pairing();

 protected:
  void parse_packet();

  std::string rx_buf_;
  std::string address_;
  std::vector<std::string> paired_addresses_;
  std::vector<DooyaComponent *> subcomponents_;
  struct 
  {
    bool in_progress = false;
    std::chrono::time_point<std::chrono::high_resolution_clock> start;
    bool req_sent = false;
  } pairing_;
};

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
  
  virtual void process_packet(std::string rx) = 0;

  void set_address(std::string address) { address_ = address; }
  std::string get_address() { return address_; };

protected:
  std::string address_;
};

}  // namespace dooya
}  // namespace esphome