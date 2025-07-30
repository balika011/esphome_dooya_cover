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

  bool register_listener(std::string address, const std::function<void(std::string)> &func);

  bool start_pairing();

 protected:
  void process_packet();

  std::unordered_map<std::string, std::function<void(std::string)>> listeners_;
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

class DooyaComponent : public Component, public Parented<DooyaBridge>
{
public:
  DooyaComponent() { }
  
  virtual void process_packet(std::string rx) override;

  void set_address(std::string address) { address_ = address; }
  std::string get_address() { return address_; };

protected:
  std::string address_;
};

}  // namespace dooya
}  // namespace esphome