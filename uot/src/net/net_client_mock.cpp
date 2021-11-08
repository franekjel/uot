#include "../../include/net/client_txrx.h"
#include <iostream>
using namespace std;
class net_client_mock : public net_client{
   private:
    string name;
    sns_client_txrx txrx;
    bool getting_input;
    thread input_reader;
    bool disconnected;

    void input_thread()
    {
        cout << "Trying to get input\n";
        while(getting_input){
            cout << "Trying to get input\n";
            char line[256];
            cin.getline(line, 256);
            if (line[0] == 'e') disconnected = true;
            cout << "Sending " << string(line) << "\n";
            if (getting_input) txrx.send_reliable(string(line));
        }
    }

   public:
    net_client_mock() : txrx(*this, 40), disconnected(true), getting_input(false){
        srand(time(NULL));
        int id = rand() % 1000 + 1;
        name = "player" + to_string(id);
        txrx.set_as_handler();
    }

    string get_name() override {
        cout << "Sending name: " << name << "\n";
        return name;
    }

    void handle_status_change(net_status status) override {
        if (status == net_client::net_status::ok)
        {
            cout << "Connection accepted :)\n";
            getting_input = true;
            input_reader = thread(&net_client_mock::input_thread, this);
        }
        else
        {
            cout << "Disconnected :(\n";
            getting_input = false;
            input_reader.join();
            disconnected = true;
        }
    }

    void handle_message(const string& data) override {
        cout << "Message :: " << data << "\n";
    }

    void run(){
        txrx.connect("127.0.0.1", 7645);
        disconnected = false;

        while (!disconnected)
        {
            this_thread::sleep_for(std::chrono::milliseconds(500));
        }

        txrx.disconnect();
    }

};

int main(void)
{
    net_client_mock mock;
    mock.run();
}