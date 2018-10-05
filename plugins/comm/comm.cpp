#include "plugins/comm/comm.h"
#include "comm_impl.h"

namespace dronecode_sdk {

Comm::Comm(System &system) : PluginBase(), _impl{new CommImpl(system)} {}

Comm::~Comm() {}

bool Comm::set_param_int(const std::string &name, int32_t value)
{
    return _impl->set_param_int(name, value);
}

bool Comm::start_stream(uint8_t req_stream_id, uint16_t req_message_rate)
{
    return _impl->start_stream(req_stream_id, req_message_rate);
}

bool Comm::stop_stream(uint8_t req_stream_id)
{
    return _impl->stop_stream(req_stream_id);
}

void Comm::send_statustext(const std::string &messageText) const
{
    _impl->send_statustext(messageText);
}

} // namespace dronecode_sdk
