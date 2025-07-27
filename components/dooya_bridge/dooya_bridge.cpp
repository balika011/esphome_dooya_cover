#include "dooya_bridge.h"
#include "esphome/core/log.h"

namespace esphome {
namespace dooya_bridge {

static const char *TAG = "dooya_bridge.component";

void DooyaBridge::setup()
{

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