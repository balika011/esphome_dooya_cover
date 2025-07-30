#include <sstream>
#include <iomanip>

#include "esphome/core/log.h"
#include "dooya_cover.h"

namespace esphome {
namespace dooya {

#define DOOYA_MAX_POSITION 100
#define DOOYA_MAX_TILT 180

static const char *TAG = "dooya_cover.cover";

void DooyaCover::setup()
{
  // Get current position on setup
  parent_->write_str(("!" + address_ + "r?;").c_str());
}

void DooyaCover::loop()
{
  if (current_operation != cover::COVER_OPERATION_IDLE && !polling_)
  {
    polling_ = true;
    parent_->write_str(("!" + address_ + "r?;").c_str());
  }
}

void DooyaCover::dump_config()
{
  ESP_LOGCONFIG(TAG, "Dooya cover");
  ESP_LOGCONFIG(TAG, "Address: %s", address_.c_str());
  if (new_position_.has_value())
  {
    ESP_LOGCONFIG(TAG, "Destination position: %f", *new_position_);
  }
  if (new_tilt_.has_value())
  {
    ESP_LOGCONFIG(TAG, "Destination tilt: %f", *new_tilt_);
  }
}

cover::CoverTraits DooyaCover::get_traits()
{
  auto traits = cover::CoverTraits();
  traits.set_is_assumed_state(false);
  traits.set_supports_position(true);
  traits.set_supports_tilt(true);
  traits.set_supports_toggle(false);
  traits.set_supports_stop(true);

  return traits;
}

void DooyaCover::process_packet(std::vector<std::pair<DooyaPacketEntryTag, std::string>> params)
{
  optional<float> data_position;
  optional<float> data_tilt;

  for (auto param : params)
  {
    ESP_LOGD(TAG, "process_packet: tag: %c value: %s", param.first, param.second.c_str());

    switch(param.first)
    {
      case ROTATION: data_position = static_cast<float>(DOOYA_MAX_POSITION - std::stoi(param.second)) / DOOYA_MAX_POSITION; break;
      case TILT: data_tilt = static_cast<float>(DOOYA_MAX_TILT - std::stoi(param.second)) / DOOYA_MAX_TILT; break;
    }
  }

  // Data for the position poll arrived
  if (data_position.has_value() || data_tilt.has_value())
    polling_ = false;

  if (
    (new_position_.has_value() || new_tilt_.has_value()) &&
    (data_position.has_value() || data_tilt.has_value())
  )
  {
    bool stopped = false;
    if (new_position_.has_value() && data_position.has_value())
    {
      if (*new_position_ == *data_position)
        stopped = true;
    }

    if (new_tilt_.has_value() && data_tilt.has_value())
    {
      if (std::abs(static_cast<int>((*new_tilt_ - *data_tilt) * 100)) < 3) // 3% tolerance
        stopped = true;
    }

    if (!stopped)
    {
      if (data_position.has_value())
      {
        if (*data_position == position)
        {
          // If we arrive where we should set stuck flag, but that not supported by ESPHome yet.
          ESP_LOGI(TAG, "process_packet: The cover is stuck");
          stopped = true;
        }
      }
    }

    if (stopped)
      current_operation = cover::COVER_OPERATION_IDLE;
  }

  if (data_position.has_value())
    position = *data_position;

  if (data_tilt.has_value())
    tilt = *data_tilt;

  publish_state();
}

void DooyaCover::control(const cover::CoverCall &call)
{
  new_position_ = call.get_position();
  new_tilt_= call.get_tilt();

  std::string cmd_params;

  if (new_position_.has_value())
  {
    ESP_LOGD(TAG, "control::position old: %f new: %f", position, *new_position_);

    if (position > *new_position_)
      current_operation = cover::COVER_OPERATION_CLOSING;
    else
      current_operation = cover::COVER_OPERATION_OPENING;
    publish_state();

    std::ostringstream ss;
    ss << std::setw(3) << std::setfill('0') << static_cast<int>((1.0f - *new_position_) * DOOYA_MAX_POSITION);

    cmd_params += "m" + ss.str();
  }

  if (new_tilt_.has_value())
  {
    ESP_LOGD(TAG, "control::tilt old: %f new: %f", tilt, *new_tilt_);

    if (tilt > *new_tilt_)
      current_operation = cover::COVER_OPERATION_CLOSING;
    else
      current_operation = cover::COVER_OPERATION_OPENING;
    publish_state();

    std::ostringstream ss;
    ss << std::setw(3) << std::setfill('0') << static_cast<int>((1.0f - *new_tilt_) * DOOYA_MAX_TILT);

    cmd_params += "b" + ss.str();
  }

  if (call.get_stop())
  {
    current_operation = cover::COVER_OPERATION_IDLE;
    publish_state();

    ESP_LOGD(TAG, "control::stop");

    cmd_params += "s";
  }

  if (cmd_params.size() > 0)
    parent_->write_str(("!" + address_ + cmd_params + ";").c_str());
}

}  // namespace dooya
}  // namespace esphome
