#include "httplib.h"
#include <iostream>
#include <regex>
#include <string>

using namespace std;

class Modulator {
  public:
    void put_number(long long num) {
        if (num >= 0) {
            buffer_ += "01";
        } else {
            buffer_ += "10";
            num = -num;
        }

        assert(num >= 0);
        int bits = 0;
        while ((1ULL << bits) <= num && bits < 64)
            bits += 4;
        buffer_ += string(bits / 4, '1');
        buffer_ += "0";

		cout << "mod: " << num << " " << bits << endl;
        if (bits > 0) {
            unsigned long long mask = (1ULL << (bits - 1));
            while (mask > 0) {
                buffer_ += (num & mask) ? "1" : "0";
                mask >>= 1;
            }
        }
    }

	void put_cell() {
		buffer_ += "11";
	}

	void put_nil() {
		buffer_ += "00";
	}

	void put_list(const vector<long long> &nums) {
		for (auto n : nums) {
			put_cell();
			put_number(n);
		}
		put_nil();
	}

	string to_string() const {
		return buffer_;
	}

  private:
    string buffer_;
};

struct ConsCell;
struct Value {
	enum Type {
		NUMBER,
		PTR
	};

	Type type;
	long long value;
	ConsCell *ptr;

	Value(long long v) : type(NUMBER), value(v) {}
	Value(ConsCell *ptr) : type(PTR), ptr(ptr) {}
};

struct ConsCell {
	Value *car, *cdr;

	ConsCell(Value *car, Value *cdr) : car(car), cdr(cdr) {}
};

ostream& operator <<(ostream&, const ConsCell&);
ostream& operator << (ostream &os, const Value &v) {
	if (v.type == Value::NUMBER) {
		os << v.value;
	} else if (v.ptr == nullptr) {
		os << "nil";
	} else {
		os << *v.ptr;
	}
	return os;
}

ostream& operator << (ostream &os, const ConsCell &v) {
	os << "(" << *v.car << "," << *v.cdr << ")";
	return os;
}

class Demodulator {
	public:
	Value* demodulate(const string &str) {
		int pos = 0;
		return demodulate_top(str, pos);
	}

	private:
	Value* demodulate_top(const string &str, int &pos) {
		const string tag = str.substr(pos, 2);
		pos += 2;
		if (tag == "00") {
			return new Value(nullptr);
		} else if (tag == "01") {
			return demodulate_number(str, pos, 1);
		} else if (tag == "10") {
			return demodulate_number(str, pos, -1);
		} else if (tag == "11") {
			return demodulate_cell(str, pos);
		}
		assert(false);
	}

	Value* demodulate_number(const string &str, int &pos, int sign) {
		int bits = 0;
		while(str[pos] == '1') {
			bits += 4;
			pos++;
		}
		pos++;

		long long value = 0;
		for (int i = 0; i < bits; ++i) {
			value <<= 1;
			if (str[pos] == '1') value += 1;
			pos++;
		}
		return new Value(value * sign);
	}

	Value* demodulate_cell(const string &str, int &pos) {
		Value *car = demodulate_top(str, pos);
		Value *cdr = demodulate_top(str, pos);
		return new Value(new ConsCell(car, cdr));
	}
};

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

		cout << "sending: " << mod.to_string() << endl;
		auto response = client_.Post("/alians/send", mod.to_string(), "text/plain");
		if (!response) {
			cout << "Server didn't respond" << endl;
			return nullptr;
		}

		if (response->status != 200) {
			cout << "Server returned unexpected response code: " << response->status << endl;
			return nullptr;
		}

		cout << response->body << endl;
		Value *stat = Demodulator().demodulate(response->body);
		cout << *stat << endl;
		return stat;
	}

    httplib::Client client_;
    const long long player_key_;
};

Client *init_client(char **argv) {
    const std::string serverUrl(argv[1]);
    const std::string playerKey(argv[2]);

    std::cout << "ServerUrl: " << serverUrl << "; PlayerKey: " << playerKey
              << std::endl;

    const std::regex urlRegexp("http://(.+):(\\d+)");
    std::smatch urlMatches;
    if (!std::regex_search(serverUrl, urlMatches, urlRegexp) ||
        urlMatches.size() != 3) {
        std::cout << "Bad server URL" << std::endl;
        exit(1);
    }
    const std::string serverName = urlMatches[1];
    const int serverPort = std::stoi(urlMatches[2]);
    return new Client(serverName, serverPort, atoll(playerKey.c_str()));
}

int main(int argc, char **argv) {
    Client *client = init_client(argv);

	cout << *Demodulator().demodulate("11011000101101111111111111111100101111011111111111000011100001100010100110000011000000111111110110000") << endl;
	client->join();
	client->start();
	while (true) {
		client->command();
	}

    return 0;
}