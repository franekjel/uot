#ifndef UOT_NET_CLIENT_H
#define UOT_NET_CLIENT_H
#include <string>
#include <thread>
#include "client_context.h"
#include "net/client_txrx.h"

struct uot_net_client : net_client
{
    uot_net_client(client_context& cc);
    client_context& context;
    std::string name;
    std::map<int, int> population_data;
    std::map<Resource, float> resource_data;
    sns_client_txrx txrx;
    bool getting_input;
    bool disconnected;
    // bool connected;
    std::thread main_net_thread;

    void input_thread();
    void run();
    void connect_to_server();
    void disconnect_from_server();
    void disconnect();
    std::string get_name() override;
    void handle_status_change(net_status status) override;
    void handle_message(const std::string& data) override;
};

#endif  // UOT_NET_CLIENT_H
