#include <chrono>
#include <thread>

#include "dooya_bridge.h"
#include "esphome/core/log.h"

namespace esphome {
namespace dooya_bridge {

static const char *TAG = "dooya_bridge.component";

void DooyaBridge::setup()
{
  write_str("!000v?;");

  // Wait for response...
  while (!available());

  while (available())
  {
    uint8_t byte = read();
    if (byte == '!')
      rx_buf_.clear();
    rx_buf_ += byte;
    if (byte == ';')
    {
      ESP_LOGD(TAG, "rx_buf_: %s", rx_buf_.c_str());
      std::string address = rx_buf_.substr(1, 3);
      if (!address_.size())
        address_ = address;
      else
       paired_addresses_.push_back(address);
      rx_buf_.clear();
      std::this_thread::sleep_for(std::chrono::seconds(1));
    }
  }
}

void DooyaBridge::loop()
{
  while (available())
  {
    uint8_t byte = read();
    if (byte == '!')
      rx_buf_.clear();
    rx_buf_ += byte;
    if (byte == ';')
      parse_rx();
  }
}

void DooyaBridge::dump_config()
{
  ESP_LOGCONFIG(TAG, "Dooya bridge");
  ESP_LOGCONFIG(TAG, "Address: %s", address_.c_str());
  ESP_LOGCONFIG(TAG, "Paired devices:");
  for (std::string address : paired_addresses_)
    ESP_LOGCONFIG(TAG, "%s", address.c_str());
}

void DooyaBridge::register_listener(std::string address, const std::function<void(std::string)> &func)
{
  if (std::find(paired_addresses_.begin(), paired_addresses_.end(), address) == paired_addresses_.end())
  {
    ESP_LOGE(TAG, "No device paired by address: %s", address.c_str());
    return;
  }

  if (listeners_.find(address) != listeners_.end())
  {
    ESP_LOGE(TAG, "Listener already registered for address: %s", address.c_str());
    return;
  }
  
  listeners_[address] = func;
}

bool DooyaBridge::start_pairing()
{
  ESP_LOGI(TAG, "Pairing...");

  return true;
}

void DooyaBridge::parse_rx()
{
  std::string rx = rx_buf_;
  rx_buf_.clear();
  if (rx[0] != '!' || rx[rx.length() - 1] != ';')
    return;

  rx = rx.substr(1, rx.length() - 2);

  ESP_LOGD(TAG, "parse_rx: rx: %s", rx.c_str());

  std::string address = rx.substr(0, 3);
  rx = rx.substr(3);

  ESP_LOGD(TAG, "parse_rx: address: %s", address.c_str());

  auto extra_index = rx.find(',');
  if (extra_index != std::string::npos)
  {
    std::string extra = rx.substr(extra_index + 1);
    ESP_LOGD(TAG, "parse_rx: extra: %s", extra.c_str());
    rx = rx.substr(0, extra_index);
  }

  ESP_LOGD(TAG, "parse_rx: data: %s", rx.c_str());

  if (auto search = listeners_.find(address); search != listeners_.end())
    search->second(rx);
  else
    ESP_LOGE(TAG, "Unknown address: %s", address.c_str());
}

} //namespace dooya_bridge
} //namespace esphome