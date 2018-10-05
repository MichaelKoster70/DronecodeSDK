#pragma once

#include <cstdint>
#include <string>
#include <memory>
#include "plugin_base.h"

namespace dronecode_sdk {

class System;
class CommImpl;

/**
 * @brief The Comm class provides the ability to
* - send statustext messages to the connected vecicle.
* - set parameter values
* - start/stop data streams
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

    /**
     * @brief Sets a MAVLINK 32bit integer parameter
     *
     * @param name The parameter name
     * @param value The parameter value
     * @return bool true if successfull, else false
     */
    bool set_param_int(const std::string &name, int32_t value);

     /**
     * @brief Request a given data stream from the connected system 
     * using the MAVLINK REQUEST_DATA_STREAM message.
     *
     * @param req_stream_id The stream ID to start (one of the MAV_DATA_STREAM values)
     * @param req_message_rate The message rate in Hz (1..10)
     * @return bool true if successfull, else false
     */
    bool start_stream(uint8_t req_stream_id, uint16_t req_message_rate);

    /**
     * @brief Instructs the connected system to stop sending steam data.
     *
     * @param req_stream_id The stream ID to stop (one of the MAV_DATA_STREAM values)
     * @return bool true if successfull, else false
     */
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