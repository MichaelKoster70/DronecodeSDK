#include <functional>
#include "comm_impl.h"
#include "system.h"
#include "mavlink_include.h"
#include "global_include.h"

namespace dronecode_sdk {

CommImpl::CommImpl(System &system) : PluginImplBase(system)
{
   _parent->register_plugin(this);
}

CommImpl::~CommImpl()
{
    _parent->unregister_plugin(this);
}

void CommImpl::init() { }

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
   UNUSED(message);
}
} // namespace dronecode_sdk