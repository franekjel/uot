#pragma once
#include <string>
#include <thread>
#include "buildings.h"
#include "client_context.h"
#include "msg/messagestructs.h"
#include "net/client_txrx.h"

struct uot_net_client : net_client
{
    uot_net_client(client_context& cc);
    client_context& context;
    std::string name;
    sns_client_txrx txrx;
    bool getting_input;
    bool disconnected;
    // bool connected;
    std::thread player_payload;

    void input_thread();
    void run();
    void connect_to_server();
    void disconnect_from_server();
    void disconnect();
    std::string get_name() override;
    void handle_status_change(net_status status) override;
    void handle_message(const std::string& data) override;

    void send_payload();
    void updateFleet(const std::shared_ptr<Fleet> f, const messageTypes::MsgFleetParameters p);
};
