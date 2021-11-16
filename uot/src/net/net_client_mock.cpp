#include <array>
#include <iostream>

#include "net/client_txrx.h"

class net_client_mock : public net_client
{
   private:
    std::string name;
    sns_client_txrx txrx;
    bool getting_input;
    std::thread input_reader;
    bool disconnected;

    void input_thread()
    {
        std::cout << "Trying to get input\n";
        while (getting_input)
        {
            std::cout << "Trying to get input\n";
            std::array<char, 256> line;
            std::cin.getline(line.data(), 256);
            if (line[0] == 'e')
                disconnected = true;
            std::cout << "Sending " << std::string(line.data()) << "\n";
            if (getting_input)
                txrx.send_reliable(std::string(line.data()));
        }
    }

   public:
    net_client_mock() : txrx(*this, 40), disconnected(true), getting_input(false)
    {
        srand(time(nullptr));
        int id = rand() % 1000 + 1;
        name = "player" + std::to_string(id);
        txrx.set_as_handler();
    }

    std::string get_name() override
    {
        std::cout << "Sending name: " << name << "\n";
        return name;
    }

    void handle_status_change(net_status status) override
    {
        if (status == net_client::net_status::ok)
        {
            std::cout << "Connection accepted :)\n";
            getting_input = true;
            input_reader = std::thread(&net_client_mock::input_thread, this);
        }
        else
        {
            std::cout << "Disconnected :(\n";
            getting_input = false;
            input_reader.join();
            disconnected = true;
        }
    }

    void handle_message(const std::string& data) override { std::cout << "Message :: " << data << "\n"; }

    void run()
    {
        txrx.connect("127.0.0.1", 7645);
        disconnected = false;

        while (!disconnected)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }

        txrx.disconnect();
    }
};

int main(void)
{
    net_client_mock mock;
    mock.run();
}
