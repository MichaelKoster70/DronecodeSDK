#pragma once

#include <cstdint>
#include <string>
#include <memory>
#include "plugin_base.h"

namespace dronecode_sdk {

class System;
class CommImpl;

/**
 * @brief The StatusText class provides the ability to send MAVlink statustext messages to the connected vecicle.
 */
class Comm : public PluginBase {
public:
    /**
     * @brief Constructor. Creates the plugin for a specific System.
     *
     * The plugin is typically created as shown below:
     *
     *     ```cpp
     *     auto info = std::make_shared<Info>(system);
     *     ```
     *
     * @param system The specific system associated with this plugin.
     */
    explicit Comm(System &system);

    /**
     * @brief Destructor (internal use only).
     */
    ~Comm();

   bool set_param_int(const std::string &name, int32_t value);

   bool start_stream(uint8_t req_stream_id, uint16_t req_message_rate);
   bool stop_stream(uint8_t req_stream_id);

    /**
     * @brief Sends a status text message.
     *
     * @param messageText The message (at max 50 characters) to send.
     * @return none
     */
    void send_statustext(const std::string &messageText) const;

    // Non-copyable
    /**
     * @brief Copy Constructor (object is not copyable).
     */
    Comm(const Comm &) = delete;
    /**
     * @brief Equality operator (object is not copyable).
     */
    const Comm &operator=(Comm &) = delete;

private:
    /** @private Underlying implementation, set at instantiation */
    std::unique_ptr<CommImpl> _impl;
};

} // namespace dronecode_sdk