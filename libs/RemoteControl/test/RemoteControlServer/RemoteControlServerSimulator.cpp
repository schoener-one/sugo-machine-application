///////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * @file
 *
 * @author: Denis Schoener (denis@schoener-one.de)
 * @date:   22.09.2020
 *
 * @license: Copyright (C) 2020 by Denis Schoener
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
 * for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <https://www.gnu.org/licenses/>.
 */
///////////////////////////////////////////////////////////////////////////////////////////////////

#include <signal.h>
#include <chrono>
#include <string>
#include <thread>

#include "Common/Logger.hpp"
#include "Common/Types.hpp"
#include "RemoteControl/IClientRequestHandler.hpp"
#include "RemoteControl/Protocol.hpp"
#include "RemoteControl/RemoteControlServer.hpp"

using namespace sugo;
using namespace sugo::remote_control;

namespace
{
// Handle interrupts, like Ctrl-C
static int  s_signo = 0;
static void signal_handler(int signo)
{
    s_signo = signo;
}
}  // namespace

class RequestHandler : public IClientRequestHandler
{
public:
    static constexpr unsigned SpeedStep    = 10;
    static constexpr unsigned DefaultSpeed = SpeedStep * 10;
    static constexpr unsigned MaxSpeed     = SpeedStep * 30;

    bool receiveRequest(ClientId clientId, const common::Json& request,
                        common::Json& response) override
    {
        const auto type = request.at(id::Type);
        if (type.empty())
        {
            LOG(warning) << clientId << ": Invalid request - missing request type";
            response = {{id::Type, id::TypeResponseRequest},
                        {id::Result, id::ResultError},
                        {id::ErrorReason, id::ErrorTypeInvalid}};
            return true;
        }

        const auto typeValue = type.get<std::string>();
        if (typeValue == id::TypeRequestState)
        {
            response = createStateMessage(id::TypeResponseState);
        }
        else if (typeValue == id::TypeRequestCommand)
        {
            if (!request.at("command").empty())
            {
                const auto command = request.at("command").get<std::string>();
                LOG(debug) << clientId << ": Received command request: " << command;
                if (command == "start")
                {
                    if (m_currentState == State::Off)
                    {
                        m_currentState = State::Starting;
                        response       = {{id::Type, id::TypeResponseCommand},
                                    {id::Result, id::ResultSuccess}};
                    }
                    else
                    {
                        response = {{id::Type, id::TypeResponseCommand},
                                    {id::Result, id::ResultError},
                                    {id::ErrorReason, "error-state-invalid"}};
                        return true;
                    }
                }
                else if (command == "stop")
                {
                    if ((m_currentState == State::Running) || (m_currentState == State::Error))
                    {
                        m_currentState = State::Off;
                        response       = {{id::Type, id::TypeResponseCommand},
                                    {id::Result, id::ResultSuccess}};
                    }
                    else
                    {
                        response = {{id::Type, id::TypeResponseCommand},
                                    {id::Result, id::ResultError},
                                    {id::ErrorReason, "error-state-invalid"}};
                        return true;
                    }
                }
                else if (command == "increase-speed")
                {
                    m_currentSpeed = (m_currentSpeed + SpeedStep) % MaxSpeed;
                }
                else if (command == "decrease-speed")
                {
                    m_currentSpeed = (m_currentSpeed - SpeedStep) % MaxSpeed;
                }
                setUpdateTimer(std::chrono::seconds(1));
            }
            else
            {
                response = {{id::Type, id::TypeResponseRequest},
                            {id::Result, id::ResultError},
                            {id::ErrorReason, "invalid-command"}};
            }
        }
        else
        {
            response = {{id::Type, id::TypeResponseRequest},
                        {id::Result, id::ResultError},
                        {id::ErrorReason, "invalid-request"}};
        }
        return true;
    }

    void registerSendNotification(SendNotificationCallback callback) override
    {
        m_cbSendNotification = callback;
    }

private:
    enum State
    {
        Off,
        Starting,
        Running,
        Error
    };

    static const char* convertToString(State state)
    {
        switch (state)
        {
            case State::Off:
                return "off";
            case State::Starting:
                return "starting";
            case State::Running:
                return "running";
            case State::Error:
                return "error";
        }
        return "";
    }

    common::Json createStateMessage(const std::string& type)
    {
        return common::Json({{id::Type, type},
                             {id::Result, id::ResultSuccess},
                             {"state", convertToString(m_currentState)},
                             {id::Speed, m_currentSpeed}});
    }

    void updateMachineState()
    {
        if (m_cbSendNotification == nullptr)
        {
            LOG(warning) << "No callback for send notification available";
            return;
        }
        m_cbSendNotification(createStateMessage("state-notification"));
        if (m_currentState == State::Starting)
        {
            m_currentState = State::Running;
            setUpdateTimer(std::chrono::seconds(3));
        }
    }

    template <typename _Rep, typename _Period>
    void setUpdateTimer(const std::chrono::duration<_Rep, _Period>& duration)
    {
        std::thread([this, duration]() {
            common::Logger::init();
            std::this_thread::sleep_for(duration);
            this->updateMachineState();
        }).detach();
    }

    SendNotificationCallback m_cbSendNotification = nullptr;
    State                    m_currentState       = State::Off;
    unsigned                 m_currentSpeed       = DefaultSpeed;
};

int main(int argc, char const** argv)
{
    if (argc < 2)
    {
        return 1;
    }
    const std::string docRoot(argv[1]);
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);
    sigset_t signals;
    sigemptyset(&signals);
    sigaddset(&signals, SIGINT);
    sigaddset(&signals, SIGTERM);

    RequestHandler      requestHandler;
    RemoteControlServer server("localhost", 4321, docRoot, requestHandler);
    if (!server.start())
    {
        return 2;
    }

    while (s_signo == 0)
    {
        int sig = 0;
        sigwait(&signals, &sig);
    }
    server.stop();
    return 0;
}