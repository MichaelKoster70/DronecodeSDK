#pragma once

#include "plugins/comm/comm.h"
#include "plugin_impl_base.h"
#include <mutex>

namespace dronecode_sdk {

class CommImpl : public PluginImplBase
{
public:
    CommImpl(System &system);
    ~CommImpl();

   void init() override;
   void deinit() override;

   void enable() override;
   void disable() override;

   bool set_param_int(const std::string &name, int32_t value);
   
   bool start_stream(uint8_t req_stream_id, uint16_t req_message_rate);
   bool stop_stream(uint8_t req_stream_id);

   void send_statustext(const std::string &messageText) const;

private:
    void process_heartbeat(const mavlink_message_t &message);
};

} // namespace dronecode_sdk
