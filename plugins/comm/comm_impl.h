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
   void subscribe_mission_progress(Comm::progress_callback_t callback);

   void send_statustext(const std::string &messageText) const;

private:
    void process_mission_current(const mavlink_message_t &message);
    void process_mission_count(const mavlink_message_t &message);
    void process_mission_item_reached(const mavlink_message_t &message);
    void process_heartbeat(const mavlink_message_t &message);

    void report_mission_progress();

private:
    mutable std::recursive_mutex _mutex{};
    Comm::progress_callback_t _progress_callback{nullptr};

    bool _armed{false};

    int _mission_total{-1};
    int _mission_current{-1};
    int _mission_reached{0};

    int _mission_reported_current{-1};
    int _mission_reported_reached{0};
};

} // namespace dronecode_sdk
