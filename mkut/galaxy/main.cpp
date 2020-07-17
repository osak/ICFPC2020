#include <iostream>
#include <vector>
#include <map>
#include <stack>
using namespace std;

struct Value {
   int type;
   string variable; // type == 0
   long long value; // type == 1
   string ident; // type == 2
   vector<Value> args;

   Value(int type, string variable, long long value, string ident) : type(type), variable(variable), value(value), ident(ident) {
   }

   static Value new_variable(string v) {
      return Value(0, v, 0, "");
   }

   static Value new_value(long long value) {
      return Value(1, "", value, "");
   }

   static Value new_ident(string ident) {
      return Value(2, "", 0, ident);
   }
};

class Evaluator {
   map<string, Value> env;

   void reduce(Value v) {

   }
};

int main() {

   return 0;
}