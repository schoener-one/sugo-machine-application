///////////////////////////////////////////////////////////////////////////////
/**
 * @file
 * @license: Copyright 2022, Schoener-One
 *
 * @author: denis@schoener-one
 * @date:   2022-07-25
 */
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include "IOContext.hpp"

#include <boost/asio.hpp>

namespace sugo
{
/**
 * Class representing IO context.
 */
class AsioContext : public IOContext
{
public:
    using IOContext::IOContext;
    
    ~AsioContext() override
    {
        if (isRunning())
        {
            stop();
        }
    }

    bool start() override;

    void stop() override;

    boost::asio::io_context& getContext()
    {
        return m_asioContext;
    }

private:
    boost::asio::io_context m_asioContext;
};

}  // namespace sugo