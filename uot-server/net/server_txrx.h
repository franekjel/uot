#ifndef UOT_SERVER_TXRX_H
#define UOT_SERVER_TXRX_H

#include <steam/steamnetworkingsockets.h>
#include <map>
#include <queue>
#include <string>
#include <thread>
using namespace std;

class server_txrx
{
   public:
    struct net_player
    {
        string name;
        string IP;
    };
    virtual void send_reliable(const string& player_name, const string& message) = 0;
    virtual void send_unreliable(const string& player_name, const string& message) = 0;
    virtual void disconnect_player(const string& player_name) = 0;
    virtual void start_socket(unsigned short port) = 0;
    virtual void stop_accepting() = 0;
    virtual void disconnect_all() = 0;
};

class net_server
{
   public:
    enum class net_status
    {
        ok,
        disconnect
    };
    virtual bool accept_player(const server_txrx::net_player& player) = 0;
    virtual void handle_status_change(const string& player_name, net_status status) = 0;
    virtual void handle_message(const string& player_name, const string& data) = 0;
};

class sns_server_txrx : public server_txrx
{
   public:
    sns_server_txrx(net_server& served_server, unsigned long turn_dur);
    void send_reliable(const string& player_name, const string& message) override;
    void send_unreliable(const string& player_name, const string& message) override;
    void disconnect_player(const string& player_name) override;
    void start_socket(unsigned short port) override;
    void disconnect_all() override;
    void stop_accepting() override;
    void set_as_handler();
    virtual ~sns_server_txrx();

   protected:
    bool accept_cons;
    struct sns_player_data
    {
        HSteamNetConnection con;
        string name;
        thread* poller;
        bool running;
    };
    net_server& server;
    HSteamListenSocket socket;
    bool socket_opened;
    map<string, sns_player_data> name_to_player;
    map<HSteamNetConnection, string> con_to_name;
    thread listener;
    void handle_state_change_server(SteamNetConnectionStatusChangedCallback_t* pInfo);
    static sns_server_txrx* cur_handler;
    static void handle_state_change(SteamNetConnectionStatusChangedCallback_t* pInfo);
    void listen_thread();
    void rx_thread(HSteamNetConnection con);
    const static int reliable_flags;
    const static int unreliable_flags;
    static void send_message(HSteamNetConnection con, int flags, const string& message);
    void disconnect_all_local();
    unsigned long sleep_dur;
    queue<HSteamNetConnection> unknown_cons;
};

#endif  // UOT_SERVER_TXRX_H
