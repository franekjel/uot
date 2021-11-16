#ifndef UOT_NET_SERVER_MOCK_H
#define UOT_NET_SERVER_MOCK_H

#include <vector>
#include "server_txrx.h"

class net_server_mock : public net_server
{
   public:
    net_server_mock();
    bool accept_player(const server_txrx::net_player& player) override;
    void handle_status_change(const std::string& player_name, net_status status) override;
    void handle_message(const std::string& player_name, const std::string& data) override;
    void run();

   protected:
    bool running;
    std::thread read_thread;
    sns_server_txrx txrx;
    void read_input();
    std::vector<std::string> players;
};

#endif  // UOT_NET_SERVER_MOCK_H
