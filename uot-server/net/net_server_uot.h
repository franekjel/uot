#ifndef UOT_NET_SERVER_MOCK_H
#define UOT_NET_SERVER_MOCK_H

#include <vector>
#include <functional>
#include "server_txrx.h"

class net_server_uot : public net_server
{
   public:
    net_server_uot();
    bool accept_player(const server_txrx::net_player& player) override;
    void handle_status_change(const std::string& player_name, net_status status) override;
    void handle_message(const std::string& player_name, const std::string& data) override;
    void run();
    void set_accept_player_callback(std::function<void()> callback);
    void send_new_tour_message(int tour_number);
    void send_game_begin_message(/*TBD*/);
   protected:
    bool running;
    std::thread read_thread;
    sns_server_txrx txrx;
    void read_input();
    std::vector<std::string> players;

   private:
    std::function<void()> uot_accept_player;
};

#endif  // UOT_NET_SERVER_MOCK_H
