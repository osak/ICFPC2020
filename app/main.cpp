#include "client.h"
#include "modem.h"

#include <iostream>
#include <regex>
#include <string>

using namespace std;

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

void test() {
	Value *v = Demodulator().demodulate("110110000111011000011111011110000100000000110101111011110001000000000110110000111011100100000000111101110000100001101110100000000011110110000111011000011101100001110110000100001111010111101110000100001101110100000000011111111011000011101011110111000110000101100010101111110100101111011000011101100001110110000111011000010011010110111001000000110110000100110000111111010110110000111111011000110000011100010101111110100101111011010101101101010110110101011011010100011010110111001000000110110000100110000000000");
	cout << *v << endl;
	cout << *as_galaxy(v) << endl;
}

int main(int argc, char **argv) {
	test();

    Client *client = init_client(argv);

	client->join();
	client->start();
	while (true) {
		client->command();
	}

    return 0;
}