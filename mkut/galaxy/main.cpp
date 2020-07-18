#include <iostream>
#include <vector>
#include <map>
#include <stack>
#include <cassert>
#include <string>
#include <sstream>
#include <fstream>
#include <set>
using namespace std;

struct Value {
   // type == 0 -> Variable variable args
   // type == 1 -> Number value
   // type == 2 -> Identifier ident args
   // type == 3 -> Cons args[0] args[1]
   // type == 4 -> nil args
   int type;
   string variable;
   long long value;
   string ident;
   vector<Value> args;

   Value() : type(-1), variable(""), value(0), ident("") {}

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

   static Value new_cons(Value x, Value y) {
      Value r(3, "", 0, "");
      r.args.push_back(x);
      r.args.push_back(y);
      return r;
   }

   static Value new_nil() {
      return Value(4, "", 0, "");
   }
};

void print_keywords(vector<string> vs) {
    set<string> kws;
    for (auto &s : vs) {
        stringstream ss(s);
        string t;
        while (ss >> t) {
            if (isalpha(t[0])) {
                kws.insert(t);
            }
        }
    }

    cout << "keywords:" << endl;
    for (auto kw: kws) {
        cout << kw << endl;
    }
}

void print_value_sub(const Value&);

void print_function(const Value &v) {
   /*
   if (v.ident == "cons" && v.args.size() == 2) {
      cout << "(";
      print_value_sub(v.args[0]);
      cout << ", ";
      print_value_sub(v.args[1]);
      cout << ")";
   } else */if (v.ident == "nil" && v.args.size() == 0) {
      cout << "nil";
   } else {
      cout << "(" << v.ident;
      for (auto a: v.args) {
         cout << " ";
         print_value_sub(a);
      }
      cout << ")";
   }
}

void print_cons(const Value &v) {
   if (v.args.size() > 2) {
      cout << "(";
   }
   cout << "(";
   print_value_sub(v.args[0]);
   cout << ", ";
   print_value_sub(v.args[1]);
   cout << ")";
   if (v.args.size() > 2) {
      for (int i = 2; i < v.args.size(); i++) {
         cout << " ";
         print_value_sub(v.args[i]);
      }
      cout << ")";
   }
}

void print_value_sub(const Value &v) {
    switch (v.type) {
        case 0:
            cout << "(" << v.variable;
            for (auto a: v.args) {
               cout << " ";
               print_value_sub(a);
            }
            cout << ")";
            break;
        case 1:
            cout << v.value;
            break;
        case 2:
            print_function(v);
            break;
        case 3:
            print_cons(v);
            break;
        case 4:
            cout << "nil";
            break;
        default:
            cerr << "!!!" << v.type << endl;
            assert(false);
    }
}

void print_value(const Value &v) {
   print_value_sub(v);
   cout << endl;
}

class Evaluator {
   int get_argc(string ident) {
      if (ident == "nil") {
         return 0;
      } else if (ident == "car" || ident == "cdr" || ident == "i" || ident == "isnil" || ident == "neg") {
         return 1;
      } else if (ident == "add" || ident == "div" || ident == "eq" || ident == "t" || ident == "f" || ident == "lt" || ident == "mul" || ident == "cons") {
         return 2;
      } else if (ident == "b" || ident == "c" || ident == "s") {
         return 3;
      }
      assert(false);
   }

public:
   map<string, Value> env;
   Value reduce2(Value v) {
      // number (strict eval)
      if ("add" == v.ident) {
         Value x = reduce(v.args[0], false);
         Value y = reduce(v.args[1], false);
         assert(x.type == 1 && y.type == 1);
         return Value::new_value(x.value + y.value);
      } else if ("mul" == v.ident) {
         Value x = reduce(v.args[0], false);
         Value y = reduce(v.args[1], false);
         assert(x.type == 1 && y.type == 1);
         return Value::new_value(x.value * y.value);
      } else if ("div" == v.ident) {
         Value x = reduce(v.args[0], false);
         Value y = reduce(v.args[1], false);
         assert(x.type == 1 && y.type == 1);
         return Value::new_value(x.value / y.value);
      } else if ("neg" == v.ident) {
         Value x = reduce(v.args[0], false);
         assert(x.type == 1);
         return Value::new_value(-x.value);
      } else if ("eq" == v.ident) {
         Value x = reduce(v.args[0], false);
         Value y = reduce(v.args[1], false);
         assert(x.type == 1 && y.type == 1);
         return Value::new_ident(x.value == y.value ? "t" : "f");
      } else if ("lt" == v.ident) {
         Value x = reduce(v.args[0], false);
         Value y = reduce(v.args[1], false);
         assert(x.type == 1 && y.type == 1);
         return Value::new_ident(x.value < y.value ? "t" : "f");
      // cons
      } else if ("car" == v.ident) {
         Value x = reduce(v.args[0], false);
         assert(x.type == 3);
         return x.args[0];
      } else if ("cdr" == v.ident) {
         Value x = reduce(v.args[0], false);
         assert(x.type == 3);
         return x.args[1];
      } else if ("cons" == v.ident) {
         Value x = v.args[0];
         Value y = v.args[1];
         return Value::new_cons(x, y);
      // nil
      } else if ("isnil" == v.ident) {
         Value x = reduce(v.args[0], false);
         if (x.type == 4) {
            return Value::new_ident("t");
         } else {
            return Value::new_ident("f");
         }
      } else if ("nil" == v.ident) {
         return Value::new_nil();
      // general
      } else if ("b" == v.ident) {
         // b x y z -> x (y z)
         Value x = v.args[0];
         Value y = v.args[1];
         Value z = v.args[2];
         y.args.push_back(z);
         x.args.push_back(y);
         return x;
      } else if ("c" == v.ident) {
         // c x y z -> x z y
         Value x = v.args[0];
         Value y = v.args[1];
         Value z = v.args[2];
         x.args.push_back(z);
         x.args.push_back(y);
         return x;
      } else if ("t" == v.ident) {
         // t x y -> x
         Value x = v.args[0];
         return x;
      } else if ("f" == v.ident) {
         // f x y -> y
         Value y = v.args[1];
         return y;
      } else if ("i" == v.ident) {
         // i x -> x
         Value x = v.args[0];
         return x;
      } else if ("s" == v.ident) {
         // s x y z = x z (y z)
         Value x = v.args[0];
         Value y = v.args[1];
         Value z = v.args[2];
         y.args.push_back(z);
         x.args.push_back(z);
         x.args.push_back(y);
         return x;
      } else {
         return v;
      }
   }

   Value reduce(Value v, bool deep = true) {
      if (deep) {
         //print_value(v);
      }
      if (v.type == 0) {
         if (!env.count(v.variable)) {
            cerr << v.variable << endl;
         }
         assert(env.count(v.variable));
         Value x = env[v.variable];
         for (auto e : v.args) {
            x.args.push_back(e);
         }
         return reduce(x, deep);
      } else if (v.type == 1) {
         assert(v.args.size() == 0);
         return v;
      } else if (v.type == 2) {
         int argc = get_argc(v.ident);
         if (v.args.size() < argc) {
            return v;
         }
         Value r = reduce2(v);
         for (int i = argc; i < v.args.size(); i++) {
            r.args.push_back(v.args[i]);
         }
         return reduce(r, deep);
      } else if (v.type == 3) {
         if (v.args.size() > 2) {
            Value x = v.args[0];
            Value y = v.args[1];
            Value z = v.args[2];
            z.args.push_back(x);
            z.args.push_back(y);
            for (int i = 3; i < v.args.size(); i++) {
               z.args.push_back(v.args[i]);
            }
            return reduce(z, deep);
         } else if (deep) {
            v.args[0] = reduce(v.args[0], deep);
            print_value(v.args[0]);
            v.args[1] = reduce(v.args[1], deep);
            print_value(v.args[1]);
            return v;
         } else {
            return v;
         }
      } else if (v.type == 4) {
         if (v.args.size() > 0) {
            Value x = Value::new_ident("t");
            for (int i = 1; i < v.args.size(); i++) {
               x.args.push_back(v.args[i]);
            }
            return reduce(x, deep);
         } else {
            return v;
         }
      } else {
         assert(false);
      }
   }
};

map<string, Value> parse_records(vector<string> vs) {
    map<string, Value> result;

    set<string> kws;
    for (auto &s : vs) {
        stringstream ss(s);
        string name, _;

        if (!(ss >> name >> _)) {
            break;
        }
        assert(_ == "=");

        vector<string> def;
        string t;
        while (ss >> t) {
            def.push_back(t);
        }

        stack<Value> st;
        for (auto e : vector<string>(def.rbegin(), def.rend())) {
            char c = e[0];
            if (e == "ap") {
                auto a = st.top(); st.pop();
                auto b = st.top(); st.pop();
                a.args.push_back(b);

                st.push(a);
            } else if (c == ':') {
                st.push(Value::new_variable(e));
            } else if (isalpha(c)) {
                st.push(Value::new_ident(e));
            } else {
                st.push(Value::new_value(stoll(e)));
            }
        }

        assert(st.size() == 1);

        result[name] = st.top();
    }

    return result;
};

int main(int argc, char *argv[]) {
    string line;
    ifstream file(argv[1]);
    if (file.is_open()) {
        vector<string> vs;
        while (getline(file, line)) {
            vs.push_back(line);
        }
        auto result = parse_records(vs);

        Evaluator ev;
        ev.env = result;
        print_value(ev.reduce(Value::new_variable(":galaxy")));

        file.close();
    }
}