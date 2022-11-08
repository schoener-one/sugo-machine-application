///////////////////////////////////////////////////////////////////////////////
/** @file
 * @license: Copyright 2022, Schoener-One
 *
 * @author: denis
 * @date:   16.10.2022
 */
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include "StreamBuffer.hpp"

namespace sugo::message
{
/**
 * Interface to handle received messages.
 */
class IMessageHandler
{
public:
    virtual ~IMessageHandler() = default;

    /**
     * @brief Called to process a new received message.
     *
     * @param      receivedMessage Input stream buffer as received message.
     * @param[out] responseMessage Output stream buffer to write the response to. Depending on the
     * received message the response could be not expected.
     * @return true if the message could be processed.
     */
    virtual bool processReceived(StreamBuffer& receivedMessage, StreamBuffer& responseMessage) = 0;

    /**
     * @brief Called after the message response has been sent.
     * This function could be used to do a second step processing.
     */
    virtual void processPost() = 0;
};

}  // namespace sugo::message