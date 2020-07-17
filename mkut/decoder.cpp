#include <iostream>
#include <sstream>
using namespace std;

class Demodulater {
public:
   string str;
   int index = 0;
   Demodulater(string str) : str(str) {}

   string demodulate() {
      if (index >= str.size()) {
         return "";
      }
      string type = str.substr(index, 2);
      if (type == "11") {
         index += 2;
         string a = demodulate();
         string b = demodulate();
         return "(" + a + "," + b + ")";
      }
      if (type == "00") {
         index += 2;
         return "nil";
      }
      if (type == "01") {
         index += 2;
         int len = 0;
         while (str[index + len] == '1') {
            len++;
         }
         index += len + 1;
         int num = 0;
         for (int i = index; i < index + len*4; i++) {
            num = num * 2 + str[i] - '0';
         }
         index += len*4;
         stringstream ss; ss << num; return ss.str();
      }
   }
};

int main() {
   string str; cin >> str;
   Demodulater demodulater(str);
   cout << demodulater.demodulate() << endl;
}