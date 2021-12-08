#ifndef UOT_NET_SERVER_MOCK_H
#define UOT_NET_SERVER_MOCK_H

#include <functional>
#include <vector>
#include "../libuot/msg/messagetypes.h"
#include "server_txrx.h"

class net_server_uot : public net_server
{
   public:
    net_server_uot();
    bool accept_player(const server_txrx::net_player& player) override;
    void handle_status_change(const std::string& player_name, net_status status) override;
    void handle_message(const std::string& player_name, const std::string& data) override;
    void run();
    void set_accept_player_callback(std::function<bool(std::string player_name)> callback);
    void set_handle_actions_callback(
        std::function<void(std::string, std::shared_ptr<messageTypes::ActionsPayload>)> callback);
    void set_after_accept_player_callback(std::function<void()> callback);
    void send_new_tour_message(int tour_number, std::shared_ptr<Player>& player, std::string player_net_name);
    void send_game_begin_message(std::shared_ptr<Player>& player, std::string player_net_name);
    void after_accept_player();

   protected:
    bool running;
    std::thread read_thread;
    sns_server_txrx txrx;
    void read_input();
    std::vector<std::string> players;

   private:
    std::function<bool(std::string player_name)> uot_accept_player;
    std::function<void(std::string, std::shared_ptr<messageTypes::ActionsPayload>)> uot_handle_actions;
    std::function<void()> uot_after_accept_player;
};

#endif  // UOT_NET_SERVER_MOCK_H
