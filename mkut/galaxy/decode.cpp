#include <iostream>
#include <vector>

using namespace std;

long long binary_to_dec(string s) {
    long long val = 0;
    for (int i = 0; i < s.size(); ++i) {
        val = val * 2  + (s[i] == '1');
    }

    return val;
}

int main () {
    string s;
    cin >> s;

    int idx = 0;

    while (idx < s.size()) {
        if (s[idx] == '0' && s[idx + 1] == '0') {
            cout << " nil";
            idx += 2;
        } else if (s[idx] == '1' && s[idx + 1] == '1') {
            cout <<  " ap ap cons";
            idx += 2;
        } else {
            bool negative = s[idx] == '1';
            idx += 2;

            int dig = 0;
            while (s[idx++] == '1') {
                dig += 4;
            }

            long long val = binary_to_dec(s.substr(idx, dig));

            cout << ' ' << (negative?"-":"")  << val;

            idx += dig;
        }
    }
    cout << endl;
}

