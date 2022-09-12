///////////////////////////////////////////////////////////////////////////////
/**
 * @file
 * @license: Copyright 2022, Schoener-One
 *
 * @author: denis@schoener-one
 * @date:   2022-08-30
 */
///////////////////////////////////////////////////////////////////////////////

#include <signal.h>
#include <chrono>
#include <string>
#include <thread>

#include "IRequestHandler.hpp"
#include "Logger.hpp"
#include "RemoteControlProtocol.hpp"
#include "RemoteControlServer.hpp"

using namespace sugo;
using namespace remote_control;

namespace
{
// Handle interrupts, like Ctrl-C
static int  s_signo = 0;
static void signal_handler(int signo)
{
    s_signo = signo;
}
}  // namespace

class RequestHandler : public sugo::remote_control::IRequestHandler
{
public:
    static constexpr unsigned SpeedStep    = 10;
    static constexpr unsigned DefaultSpeed = SpeedStep * 10;
    static constexpr unsigned MaxSpeed     = SpeedStep * 30;

    bool receiveRequest(ClientId clientId, const Json& request, Json& response) override
    {
        const auto type = request.at(protocol::IdType);
        if (type.empty())
        {
            LOG(warning) << clientId << ": Invalid request - missing request type";
            response = {{protocol::IdType, protocol::IdTypeResponseRequest},
                        {protocol::IdResult, protocol::IdResultError},
                        {protocol::IdErrorReason, protocol::IdErrorTypeInvalid}};
            return true;
        }

        const auto typeValue = type.get<std::string>();
        if (typeValue == protocol::IdTypeRequestState)
        {
            response = createStateMessage(protocol::IdTypeResponseState);
        }
        else if (typeValue == protocol::IdTypeRequestCommand)
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
                        response       = {{protocol::IdType, protocol::IdTypeResponseCommand},
                                    {protocol::IdResult, protocol::IdResultSuccess}};
                    }
                    else
                    {
                        response = {{protocol::IdType, protocol::IdTypeResponseCommand},
                                    {protocol::IdResult, protocol::IdResultError},
                                    {protocol::IdErrorReason, "error-state-invalid"}};
                        return true;
                    }
                }
                else if (command == "stop")
                {
                    if ((m_currentState == State::Running) || (m_currentState == State::Error))
                    {
                        m_currentState = State::Off;
                        response       = {{protocol::IdType, protocol::IdTypeResponseCommand},
                                    {protocol::IdResult, protocol::IdResultSuccess}};
                    }
                    else
                    {
                        response = {{protocol::IdType, protocol::IdTypeResponseCommand},
                                    {protocol::IdResult, protocol::IdResultError},
                                    {protocol::IdErrorReason, "error-state-invalid"}};
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
                response = {{protocol::IdType, protocol::IdTypeResponseRequest},
                            {protocol::IdResult, protocol::IdResultError},
                            {protocol::IdErrorReason, "invalid-command"}};
            }
        }
        else
        {
            response = {{protocol::IdType, protocol::IdTypeResponseRequest},
                        {protocol::IdResult, protocol::IdResultError},
                        {protocol::IdErrorReason, "invalid-request"}};
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

    Json createStateMessage(const std::string& type)
    {
        return Json({{protocol::IdType, type},
                     {protocol::IdResult, protocol::IdResultSuccess},
                     {"state", convertToString(m_currentState)},
                     {protocol::IdSpeed, m_currentSpeed}});
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
            Logger::init();
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