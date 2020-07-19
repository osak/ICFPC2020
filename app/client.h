#pragma once

#include "modem.h"
#include "value.h"
#include "httplib.h"

#include <string>
#include <vector>

using namespace std;

class Client {
  public:
    Client(const string &server_name, const int port, const long long player_key)
        : client_(server_name, port), player_key_(player_key) {}

    Value* join() {
		return send(2, vector<long long>());
	}

	Value* start() {
		return send(3, vector<long long>{10, 10, 10, 10});
	}

	Value* command() {
		return send(4, vector<long long>());
	}

  private:
    Value* send(long long command_id, const vector<long long> &commands) {
		Modulator mod;
		mod.put_cell();
		mod.put_number(command_id);
		mod.put_cell();
		mod.put_number(player_key_);
		mod.put_cell();
		mod.put_list(commands);
		mod.put_nil();

		cout << "command: " << "id=" << command_id << ", commands=" << commands << endl;
		cout << "sending: " << mod.to_string() << endl;
		auto response = client_.Post("/aliens/send?apiKey=decffdda9f2d431792a37fbfb770f825", mod.to_string(), "text/plain");
		if (!response) {
			cout << "Server didn't respond" << endl;
			return nullptr;
		}

		if (response->status != 200) {
			cout << "Server returned unexpected response code: " << response->status << " " << response->body << endl;
			return nullptr;
		}

		cout << "received (raw):" << response->body << endl;
		Value *stat = Demodulator().demodulate(response->body);
		cout << "received (cons):" << *stat << endl;
		cout << "received (list):" << *as_galaxy(stat) << endl;
		return stat;
	}

    httplib::Client client_;
    const long long player_key_;
};
