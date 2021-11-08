#include "../../include/net/client_txrx.h"
#include <steam/isteamnetworkingutils.h>
#include <sstream>
//  TODO : disconnect in destructor
sns_client_txrx* sns_client_txrx::cur_handler = nullptr;

void sns_client_txrx::set_as_handler() { cur_handler = this; }

const int sns_client_txrx::reliable_flags = k_nSteamNetworkingSend_Reliable;
const int sns_client_txrx::unreliable_flags = k_nSteamNetworkingSend_Unreliable;

sns_client_txrx::sns_client_txrx(net_client& served_client, unsigned long turn_dur)
    : client(served_client), state(con_state::disconnected), sleep_dur(turn_dur / 4), con(k_HSteamNetConnection_Invalid)
{
}

void sns_client_txrx::send_reliable(const string& data) { send_message(con, reliable_flags, data); }
void sns_client_txrx::send_unreliable(const string& data) { send_message(con, unreliable_flags, data); }

void sns_client_txrx::connect(const string& ipv4, unsigned short port_num)
{
    SteamDatagramErrMsg err_msg;
    if (!GameNetworkingSockets_Init(nullptr, err_msg))
        throw runtime_error("Could not init GNS");
    SteamNetworkingConfigValue_t opt{};
    SteamNetworkingIPAddr addrServer{};
    addrServer.Clear();
    if (!addrServer.ParseString(ipv4.c_str()))
        throw runtime_error("invalid ip address format");
    addrServer.m_port = port_num;
    opt.SetPtr(k_ESteamNetworkingConfig_Callback_ConnectionStatusChanged, (void*)handle_status_changed);
    con = SteamNetworkingSockets()->ConnectByIPAddress(addrServer, 1, &opt);
    if (con == k_HSteamNetConnection_Invalid)
        throw runtime_error("Could not connect to server");
    state = con_state::connecting;
    status_thread = thread(&sns_client_txrx::listen_thread, this);
}

void sns_client_txrx::disconnect() { disconnect_local(); }

void sns_client_txrx::handle_status_changed(SteamNetConnectionStatusChangedCallback_t* pInfo)
{
    cur_handler->handle_status_changed_client(pInfo);
}

void sns_client_txrx::handle_status_changed_client(SteamNetConnectionStatusChangedCallback_t* pInfo)
{
    if (pInfo->m_eOldState == k_ESteamNetworkingConnectionState_Connecting &&
        pInfo->m_info.m_eState == k_ESteamNetworkingConnectionState_Connected)
    {
        if (state == con_state::connecting)
        {
            send_message(pInfo->m_hConn, reliable_flags, client.get_name());
            state = con_state::connected;
            read_thread = thread(&sns_client_txrx::rx_thread, this);
        }
        else
            throw runtime_error("Should not be connecting if state is not disconnected");
    }
    else if (pInfo->m_info.m_eState == k_ESteamNetworkingConnectionState_ProblemDetectedLocally ||
             pInfo->m_info.m_eState == k_ESteamNetworkingConnectionState_ClosedByPeer ||
             pInfo->m_info.m_eState == k_ESteamNetworkingConnectionState_Dead ||
             pInfo->m_info.m_eState == k_ESteamNetworkingConnectionState_None)
    {
        client.handle_status_change(net_client::net_status::disconnect);
    }
}

void sns_client_txrx::send_message(HSteamNetConnection con, int flags, const string& message)
{
    auto status = SteamNetworkingSockets()->SendMessageToConnection(con, (void*)message.c_str(),
                                                                    message.length() * sizeof(char), flags, nullptr);
    if (status != k_EResultOK)
        throw runtime_error("Sending not successful");
}

void sns_client_txrx::rx_thread()
{
    while (state != con_state::disconnected)
    {
        SteamNetworkingMessage_t* message;
        int num_mes = SteamNetworkingSockets()->ReceiveMessagesOnConnection(con, &message, 1);
        if (num_mes < 0)
            throw runtime_error("Connection handle is invalid");
        if (num_mes)
        {
            if (state == con_state::connected)
            {
                state = con_state::accepted;
                client.handle_status_change(net_client::net_status::ok);
                continue;
            }
            client.handle_message(string((char*)message->GetData(), message->GetSize()));
        }
    }
}

void sns_client_txrx::listen_thread()
{
    while (state != con_state::disconnected)
    {
        SteamNetworkingSockets()->RunCallbacks();
    }
}

void sns_client_txrx::disconnect_local()
{
    state = con_state::disconnected;
    client.handle_status_change(net_client::net_status::disconnect);
    read_thread.join();
    status_thread.join();
    SteamNetworkingSockets()->CloseConnection(con, 0, nullptr, false);
    GameNetworkingSockets_Kill();
}

sns_client_txrx::~sns_client_txrx()
{
    if (state != con_state::disconnected)
    {
        disconnect_local();
    }
}
