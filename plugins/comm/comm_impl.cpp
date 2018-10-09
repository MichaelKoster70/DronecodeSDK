#include <functional>
#include "comm_impl.h"
#include "system.h"
#include "mavlink_include.h"
#include "global_include.h"

namespace dronecode_sdk {

   using namespace std::placeholders; // for `_1`

CommImpl::CommImpl(System &system) : PluginImplBase(system)
{
   _parent->register_plugin(this);
}

CommImpl::~CommImpl()
{
    _parent->unregister_plugin(this);
}

void CommImpl::init()
{
    _parent->register_mavlink_message_handler(
        MAVLINK_MSG_ID_HEARTBEAT, std::bind(&CommImpl::process_heartbeat, this, _1), this);

    _parent->register_mavlink_message_handler(
        MAVLINK_MSG_ID_MISSION_CURRENT,
        std::bind(&CommImpl::process_mission_current, this, _1),
        this);

    _parent->register_mavlink_message_handler(
        MAVLINK_MSG_ID_MISSION_ITEM_REACHED,
        std::bind(&CommImpl::process_mission_item_reached, this, _1),
        this);

    //_parent->register_mavlink_message_handler(
    //    MAVLINK_MSG_ID_MISSION_COUNT, std::bind(&CommImpl::process_mission_count, this, _1), this);

}

void CommImpl::deinit()
{
    _parent->unregister_all_mavlink_message_handlers(this);
}

void CommImpl::enable() {}

void CommImpl::disable() {}

bool CommImpl::set_param_int(const std::string &name, int32_t value)
{
    return _parent->set_param_int(name, value);
}

bool CommImpl::start_stream(uint8_t req_stream_id, uint16_t req_message_rate)
{
    auto sysid = _parent->get_system_id();
    auto compid = _parent->get_autopilot_id();
    mavlink_message_t message;
    mavlink_msg_request_data_stream_pack(GCSClient::system_id,
                                         GCSClient::component_id,
                                         &message,
                                         sysid,
                                         compid,
                                         req_stream_id,
                                         req_message_rate,
                                         1);

    return _parent->send_message(message);
}

bool CommImpl::stop_stream(uint8_t req_stream_id)
{
    auto sysid = _parent->get_system_id();
    auto compid = _parent->get_autopilot_id();
    mavlink_message_t message;
    mavlink_msg_request_data_stream_pack(GCSClient::system_id,
                                         GCSClient::component_id,
                                         &message,
                                         sysid,
                                         compid,
                                         req_stream_id,
                                         0,
                                         0);

    return _parent->send_message(message);
}

void CommImpl::subscribe_mission_progress(Comm::progress_callback_t callback)
{
   // mavlink_message_t message;
   // mavlink_msg_mission_request_list_pack(GCSClient::system_id,
   //                                       GCSClient::component_id,
   //                                       &message,
   //                                       _parent->get_system_id(),
   //                                       _parent->get_autopilot_id(),
   //                                       MAV_MISSION_TYPE_MISSION);

   //if (!_parent->send_message(message))
   //{
   //    return;
   //}

   std::lock_guard<std::recursive_mutex> lock(_mutex);
   _progress_callback = callback;
}

void CommImpl::process_mission_current(const mavlink_message_t &message)
{
    mavlink_mission_current_t mission_current;
    mavlink_msg_mission_current_decode(&message, &mission_current);

    {
        std::lock_guard<std::recursive_mutex> lock(_mutex);
        _mission_current = mission_current.seq;
    }

    report_mission_progress();
}

void CommImpl::process_mission_item_reached(const mavlink_message_t &message)
{
    mavlink_mission_item_reached_t mission_item_reached;
    mavlink_msg_mission_item_reached_decode(&message, &mission_item_reached);

    {
        std::lock_guard<std::recursive_mutex> lock(_mutex);
        _mission_reached = mission_item_reached.seq;
    }

    report_mission_progress();
}

void CommImpl::process_mission_count(const mavlink_message_t &message)
{
    mavlink_mission_count_t mission_count;
    mavlink_msg_mission_count_decode(&message, &mission_count);

    {
        std::lock_guard<std::recursive_mutex> lock(_mutex);
        _mission_total = mission_count.count;
    }
}

void CommImpl::report_mission_progress()
{
    if (_progress_callback == nullptr)
    {
        return;
    }

    int current = _mission_current;
    int reached = _mission_reached;

    bool should_report = false;

    {
        std::lock_guard<std::recursive_mutex> lock(_mutex);
        if (_mission_reported_current != current) {
            _mission_reported_current = current;
            should_report = true;
        }

        if (_mission_reported_reached != reached) {
            _mission_reported_reached = reached;
            should_report = true;
        }
    }

    if (_armed && should_report) {
        _parent->call_user_callback(
            [this, current, reached]() { _progress_callback(current, reached);
        });
    }
}

void CommImpl::send_statustext(const std::string &messageText) const
{
    std::string buffer = messageText;
    buffer.resize(50);

    mavlink_message_t message;
    mavlink_msg_statustext_pack(
        GCSClient::system_id, GCSClient::component_id, &message, MAV_SEVERITY_INFO, buffer.data());
    _parent->send_message(message);
}

void CommImpl::process_heartbeat(const mavlink_message_t &message)
{
   if (message.compid != MAV_COMP_ID_AUTOPILOT1)
   {
      return;
   }

   mavlink_heartbeat_t heartbeat;
   mavlink_msg_heartbeat_decode(&message, &heartbeat);

   _armed = (heartbeat.base_mode & MAV_MODE_FLAG_SAFETY_ARMED) ? true : false;
}

} // namespace dronecode_sdk