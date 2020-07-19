#include <iostream>
#include <chrono>
#include <thread>
using namespace std;

int main(int argc, char const *argv[])
{
    for (size_t i = 0; i < 10; i++)
    {
        cout << "yes " << argv[1] << " " << argv[2] << endl;
        std::this_thread::sleep_for(1s);
        cout << "yes" << endl;
    }
}