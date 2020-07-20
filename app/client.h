#pragma once

#include "modem.h"
#include "value.h"
#include "httplib.h"
#include "command.h"

#include <string>
#include <vector>

using namespace std;

class Client {
  public:
    Client(const string &server_name, const int port, const long long player_key)
        : client_(server_name, port), player_key_(player_key) {
            client_.set_connection_timeout(60);
            client_.set_read_timeout(10);
        }

    Value* join(const JoinParams& params) {
		string modulated = params.modulate();
		return send(2, modulated);
	}

	Value* start(const StartParams& params) {
		cout << "Engine=" << params.engine << endl;
		cout << "Armament=" << params.armament << endl;
		cout << "Reactor=" << params.reactor << endl;
		cout << "CoreHull=" << params.core << endl;
		string modulated = params.modulate();
		return send(3, modulated);
	}

	Value* command(const CommandParams& params) {
		string modulated = params.modulate();
		return send(4, modulated);
	}

  private:
    Value* send(long long command_id, const string &commands) {
		Modulator mod;
		mod.put_cell();
		mod.put_number(command_id);
		mod.put_cell();
		mod.put_number(player_key_);
		mod.put_cell();
		mod.put_raw(commands);
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

		if (response->body == "1101000") {
			cout << "battle has finished." << endl;
			exit(0);
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
