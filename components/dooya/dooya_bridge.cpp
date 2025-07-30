#include <thread>

#include "esphome/core/log.h"
#include "esphome/components/dooya/dooya.h"

#include "dooya_bridge.h"

namespace esphome {
namespace dooya {

static const char *TAG = "dooya_bridge.component";

void DooyaBridge::setup()
{
  write_str(("!" + DOOYA_ADDRESS_GLOBAL + "v?;").c_str());

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

  for (DooyaComponent *subcomponent : subcomponents_)
  {
    if (std::find(paired_addresses_.begin(), paired_addresses_.end(), subcomponent->get_address()) == paired_addresses_.end())
    {
      ESP_LOGE(TAG, "No device paired by address: %s", subcomponent->get_address().c_str());
      subcomponent->mark_failed("Unknown address");
    }
  }
}

void DooyaBridge::loop()
{
  if (pairing_.in_progress)
  {
    if (!pairing_.req_sent)
      pairing_.in_progress = std::chrono::high_resolution_clock::now() - pairing_.start < std::chrono::seconds(30);

    if (pairing_.in_progress)
    {
      if (!pairing_.req_sent)
      {
        write_str("!" + DOOYA_ADDRESS_GLOBAL + "&;");
        pairing_.req_sent = true;
      }
    }
    else
    {
      ESP_LOGI(TAG, "30s passed, no new device found. Stopping pairing.");
    }
  }

  while (available())
  {
    uint8_t byte = read();
    if (byte == '!')
      rx_buf_.clear();
    rx_buf_ += byte;
    if (byte == ';')
      parse_packet();
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

bool DooyaBridge::start_pairing()
{
  ESP_LOGI(TAG, "Pairing...");

  pairing_.in_progress = true;
  pairing_.start = std::chrono::high_resolution_clock::now();

  return true;
}

std::unordered_map<DooyaPacketEntryTag, int> DooyaPacketEntryLen =
{
  { ERROR, 2},
  { VERSION, -1 },
  { NAME, -1 },
  { MOVE, 3 },
  { STOP, 0 },
  { ROTATION, 3 },
  { TILT, 3 },
  { ADDED, 0 }
};

std::string packet_params_to_string(std::vector<std::pair<DooyaPacketEntryTag, std::string>> params)
{
  std::string ret;
  for (auto param : params)
    ret += std::string((const char *) &param.first, 1) + "=" + param.second + " ";
  return ret;
}

void DooyaBridge::parse_packet()
{
  std::string rx = rx_buf_;
  rx_buf_.clear();
  if (rx[0] != '!' || rx[rx.length() - 1] != ';')
    return;

  rx = rx.substr(1, rx.length() - 2);

  ESP_LOGD(TAG, "parse_packet: rx: %s", rx.c_str());

  std::string address = rx.substr(0, 3);
  rx = rx.substr(3);

  ESP_LOGD(TAG, "parse_packet: address: %s", address.c_str());

  auto extra_index = rx.find(',');
  if (extra_index != std::string::npos)
  {
    std::string extra = rx.substr(extra_index + 1);
    ESP_LOGD(TAG, "parse_packet: extra: %s", extra.c_str());
    rx = rx.substr(0, extra_index);
  }

  ESP_LOGD(TAG, "parse_packet: data: %s", rx.c_str());

  std::vector<std::pair<DooyaPacketEntryTag, std::string>> params;

  while (rx.length() > 0)
  {
    DooyaPacketEntryTag tag = (DooyaPacketEntryTag) rx[0];
    rx = rx.substr(1);

    auto tag_len = DooyaPacketEntryLen.find(tag);
    if (tag_len == DooyaPacketEntryLen.end())
    {
      ESP_LOGE(TAG, "Unknown packet tag: %c", tag);
      return;
    }

    std::string value;
    
    if (tag_len->second == -1)
    {
      value = rx;
      rx.clear();
    }
    else if (tag_len->second > 0)
    {
      value = rx.substr(0, tag_len->second);
      rx = rx.substr(tag_len->second);
    }
	
    ESP_LOGD(TAG, "process_packet: tag: %c value: %s", tag, value.c_str());

    params.push_back(std::make_pair(tag, value));
  }

  if (pairing_.req_sent)
  {
    if (address == DOOYA_ADDRESS_GLOBAL && params.size() == 1 && params[0].first == ERROR && params[0].second == "pf")
    {
      ESP_LOGI(TAG, "Do device found (yet).");
      pairing_.req_sent = false;
      return;
    }
    else if(params.size() == 1 && params[0].first == ADDED)
    {
      ESP_LOGI(TAG, "Paired to new device, address: %s", address.c_str());
      pairing_.req_sent = false;
      pairing_.in_progress = false;
      paired_addresses_.push_back(address);
      return;
    }
  }

  if (address == DOOYA_ADDRESS_GLOBAL)
  {
    ESP_LOGE(TAG, "Unhandled global packet: %s", packet_params_to_string(params).c_str());
    return;
  }

  if (address == address_)
  {
    ESP_LOGE(TAG, "Unhandled bridge packet: %s", packet_params_to_string(params).c_str());
    return;
  }

  if (std::find(paired_addresses_.begin(), paired_addresses_.end(), address) == paired_addresses_.end())
  {
    ESP_LOGE(TAG, "Unknown address: %s", address.c_str());
    return;
  }

  auto subcomponent = std::find_if(subcomponents_.begin(), subcomponents_.end(), [address](DooyaComponent *subcomponent) { return subcomponent->get_address() == address; });
  if (subcomponent == subcomponents_.end())
  {
    ESP_LOGE(TAG, "Subcomponent for address: %s", address.c_str());
    return;
  }

  (*subcomponent)->process_packet(params);
}

} //namespace dooya
} //namespace esphome