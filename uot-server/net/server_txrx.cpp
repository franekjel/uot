#include "server_txrx.h"
#include <sstream>
#include <stdexcept>

const int sns_server_txrx::reliable_flags = k_nSteamNetworkingSend_Reliable;
const int sns_server_txrx::unreliable_flags = k_nSteamNetworkingSend_Unreliable;

//  TODO : return full player list

sns_server_txrx* sns_server_txrx::cur_handler = nullptr;

static std::string ipv4_to_string(uint32 ipv4)
{
    std::stringstream ss;
    ss << (int)(ipv4 & 0xFF) << "." << (int)((ipv4 >> 8) & 0xFF) << "." << (int)((ipv4 >> 16) & 0xFF) << "."
       << (int)((ipv4 >> 24) & 0xFF);

    return ss.str();
}

sns_server_txrx::sns_server_txrx(net_server& served_server, unsigned long turn_dur)
    : server(served_server), accept_cons(false), socket(0), socket_opened(false), sleep_dur(turn_dur / 4)
{
}

void sns_server_txrx::send_reliable(const std::string& player_name, const std::string& message)
{
    auto con = name_to_player[player_name].con;
    send_message(con, reliable_flags, message);
}

void sns_server_txrx::send_unreliable(const std::string& player_name, const std::string& message)
{
    auto con = name_to_player[player_name].con;
    send_message(con, unreliable_flags, message);
}

void sns_server_txrx::disconnect_player(const std::string& player_name)
{
    //  TODO : mutex for maps
    auto& player = name_to_player[player_name];
    player.running = false;
    player.poller->join();
    delete player.poller;
    server.handle_status_change(player_name, net_server::net_status::disconnect);
    SteamNetworkingSockets()->CloseConnection(player.con, 0, nullptr, false);
    auto con = player.con;
    delete player.poller;
    name_to_player.erase(player_name);
    con_to_name.erase(con);
}

void sns_server_txrx::start_socket(unsigned short port)
{
    SteamDatagramErrMsg err_msg;
    if (!GameNetworkingSockets_Init(nullptr, err_msg))
        throw std::runtime_error("Could not init GNS");
    SteamNetworkingIPAddr addr{};
    addr.Clear();
    addr.m_port = port;
    SteamNetworkingConfigValue_t opt = {};
    accept_cons = true;
    opt.SetPtr(k_ESteamNetworkingConfig_Callback_ConnectionStatusChanged, (void*)handle_state_change);
    socket = SteamNetworkingSockets()->CreateListenSocketIP(addr, 1, &opt);

    if (socket == k_HSteamListenSocket_Invalid)
        throw std::runtime_error("Could not create a socket");

    socket_opened = true;
    listener = std::thread(&sns_server_txrx::listen_thread, this);
}

void sns_server_txrx::stop_accepting() { accept_cons = false; }

void sns_server_txrx::set_as_handler() { cur_handler = this; }

void sns_server_txrx::handle_state_change_server(SteamNetConnectionStatusChangedCallback_t* pInfo)
{
    if (pInfo->m_eOldState == k_ESteamNetworkingConnectionState_None &&
        pInfo->m_info.m_eState == k_ESteamNetworkingConnectionState_Connecting)
    {
        if (accept_cons)
        {
            SteamNetworkingSockets()->AcceptConnection(pInfo->m_hConn);
            unknown_cons.push(pInfo->m_hConn);
        }
        else
            SteamNetworkingSockets()->CloseConnection(pInfo->m_hConn, 0, nullptr, false);
    }
    else if (pInfo->m_info.m_eState == k_ESteamNetworkingConnectionState_ClosedByPeer ||
             pInfo->m_info.m_eState == k_ESteamNetworkingConnectionState_ProblemDetectedLocally)
    {
        if (con_to_name.count(pInfo->m_hConn) < 1)
            throw std::runtime_error("Unknown connection disconnected");

        std::string name = con_to_name[pInfo->m_hConn];
        server.handle_status_change(name, net_server::net_status::disconnect);
    }
}
void sns_server_txrx::handle_state_change(SteamNetConnectionStatusChangedCallback_t* pInfo)
{
    cur_handler->handle_state_change_server(pInfo);
}

//  Actually not only listens, so do not stop it until all connections are closed
void sns_server_txrx::listen_thread()
{
    while (socket_opened)
    {
        SteamNetworkingSockets()->RunCallbacks();
        if (!unknown_cons.empty())
        {
            auto con = unknown_cons.front();
            unknown_cons.pop();

            SteamNetworkingMessage_t* msg;
            int num_msg = SteamNetworkingSockets()->ReceiveMessagesOnConnection(con, &msg, 1);
            if (num_msg > 0)
            {
                std::string name((char*)msg->GetData(), msg->GetSize());
                SteamNetConnectionInfo_t con_info;
                if (!SteamNetworkingSockets()->GetConnectionInfo(con, &con_info))
                    throw std::runtime_error("Received message on invalid connection");

                std::string addr = ipv4_to_string(con_info.m_addrRemote.GetIPv4());
                if (!server.accept_player({name, addr}))
                {
                    SteamNetworkingSockets()->CloseConnection(con, 0, nullptr, false);
                    continue;
                }

                auto* t = new std::thread();
                sns_player_data data = {con, name, t, true};
                name_to_player.emplace(name, data);
                con_to_name.emplace(con, name);
                *t = std::thread(&sns_server_txrx::rx_thread, this, con);
                send_message(con, reliable_flags, "OK");
            }
            else if (num_msg < 0)
                throw std::runtime_error("unknown connection is invalid");
            else
            {
                unknown_cons.push(con);
            }
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(sleep_dur / 2));
    }
}

void sns_server_txrx::rx_thread(HSteamNetConnection con)
{
    while (name_to_player[con_to_name[con]].running)
    {
        SteamNetworkingMessage_t* message;
        int num_mes = SteamNetworkingSockets()->ReceiveMessagesOnConnection(con, &message, 1);
        if (num_mes < 0)
            throw std::runtime_error("Connection handle is invalid");
        if (num_mes)
        {
            server.handle_message(con_to_name[con], std::string((char*)message->GetData(), message->GetSize()));
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(sleep_dur));
    }
}

void sns_server_txrx::send_message(HSteamNetConnection con, int flags, const std::string& message)
{
    auto status = SteamNetworkingSockets()->SendMessageToConnection(con, (void*)message.c_str(),
                                                                    message.length() * sizeof(char), flags, nullptr);
    if (status != k_EResultOK)
        throw std::runtime_error("Sending not successful");
}

void sns_server_txrx::disconnect_all() { disconnect_all_local(); }

void sns_server_txrx::disconnect_all_local()
{
    stop_accepting();

    while (!name_to_player.empty())
    {
        std::string name = name_to_player.begin()->first;
        disconnect_player(name);
    }

    socket_opened = false;
    listener.join();
    SteamNetworkingSockets()->CloseListenSocket(socket);
}

sns_server_txrx::~sns_server_txrx()
{
    disconnect_all_local();
    GameNetworkingSockets_Kill();
}
