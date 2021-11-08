#ifndef UOT_CLIENT_TXRX_H
#define UOT_CLIENT_TXRX_H

#include <steam/steamnetworkingsockets.h>
#include <string>
#include <thread>
using namespace std;

class client_txrx
{
   public:
    virtual void send_reliable(const string& data) = 0;
    virtual void send_unreliable(const string& data) = 0;
    virtual void connect(const string& ipv4, unsigned short port_num) = 0;
    virtual void disconnect() = 0;
};

class net_client
{
   public:
    enum class net_status
    {
        ok,
        disconnect
    };
    virtual string get_name() = 0;
    virtual void handle_status_change(net_status status) = 0;
    virtual void handle_message(const string& data) = 0;
};

class sns_client_txrx : public client_txrx
{
   public:
    sns_client_txrx(net_client& served_client, unsigned long turn_dur);
    void send_reliable(const string& data) override;
    void send_unreliable(const string& data) override;
    void connect(const string& ipv4, unsigned short port_num) override;
    void disconnect() override;
    void set_as_handler();
    virtual ~sns_client_txrx();

   protected:
    enum class con_state
    {
        connected,
        connecting,
        accepted,
        disconnected
    };
    con_state state;
    thread read_thread;
    thread status_thread;
    net_client& client;
    HSteamNetConnection con;
    const static int reliable_flags;
    const static int unreliable_flags;
    static sns_client_txrx* cur_handler;
    static void handle_status_changed(SteamNetConnectionStatusChangedCallback_t* pInfo);
    void handle_status_changed_client(SteamNetConnectionStatusChangedCallback_t* pInfo);
    static void send_message(HSteamNetConnection con, int flags, const string& message);
    void rx_thread();
    void listen_thread();
    void disconnect_local();
    unsigned long sleep_dur;
};

#endif  // UOT_CLIENT_TXRX_H
