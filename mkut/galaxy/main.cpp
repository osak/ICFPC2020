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
   int type;
   string variable; // type == 0
   long long value; // type == 1
   string ident; // type == 2
   vector<Value> args;

   Value() : type(1), variable(""), value(0), ident("") {}

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
public:
   map<string, Value> env;
   Value reduce(Value v) {
      if (v.type == 0) {
         Value x = env[v.variable];
         for (auto e : v.args) {
            x.args.push_back(e);
         }
         return reduce(x);
      } else if (v.type == 1) {
         assert(v.args.size() == 0);
         return v;
      } else if (v.type == 2) {
         if ("add" == v.ident) {
            if (v.args.size() < 2) {
               return v;
            }
            Value a = reduce(v.args[0]);
            Value b = reduce(v.args[1]);
            assert(a.type == 1 && b.type == 1);

            return Value::new_value(a.value + b.value);
         } else if ("b" == v.ident) {
            if (v.args.size() < 3) {
               return v;
            }
            // b x y z -> x (y z)
            Value r = v.args[0];
            Value y = v.args[1];
            y.args.push_back(v.args[2]);
            r.args.push_back(y);
            return reduce(r);
         } else if ("c" == v.ident) {
            // c x y z -> x z y
            if (v.args.size() < 3) {
               return v;
            }
            Value r = v.args[0];
            r.args.push_back(v.args[2]);
            r.args.push_back(v.args[1]);
            return reduce(r);
         } else if ("car" == v.ident) {
            if (v.args.size() < 1) {
               return v;
            }
            Value x0 = reduce(v.args[0]);
            assert (x0.type == 2 && x0.ident == "cons");
            assert (x0.args.size() == 2);
            return reduce(x0.args[0]);
         } else if ("cdr" == v.ident) {
            if (v.args.size() < 1) {
               return v;
            }
            Value x0 = reduce(v.args[0]);
            assert (x0.type == 2 && x0.ident == "cons");
            assert (x0.args.size() == 2);
            return reduce(x0.args[1]);
         } else if ("cons" == v.ident) {
            return v;
         } else if ("div" == v.ident) {
            if (v.args.size() < 2) {
               return v;
            }
            Value a = reduce(v.args[0]);
            Value b = reduce(v.args[1]);
            assert(a.type == 1 && b.type == 1);

            return Value::new_value(a.value / b.value);
         } else if ("eq" == v.ident) {
            if (v.args.size() < 2) {
               return v;
            }
            Value a = reduce(v.args[0]);
            Value b = reduce(v.args[1]);
            assert(a.type == 1 && b.type == 1);
            if (a.value == b.value) {
               return Value::new_ident("t");
            } else {
               return Value::new_ident("f");
            }
         } else if ("t" == v.ident) {
            // t x y -> x
            if (v.args.size() < 2) {
               return v;
            }
            return reduce(v.args[0]);
         } else if ("f" == v.ident) {
            // f x y -> y
            if (v.args.size() < 2) {
               return v;
            }
            return reduce(v.args[1]);
         } else if ("i" == v.ident) {
            if (v.args.size() < 1) {
               return v;
            }
            return reduce(v.args[0]);
         } else if ("isnil" == v.ident) {
            if (v.args.size() < 1) {
               return v;
            }
            if (v.args[0].type == 2 && v.args[0].ident == "nil") {
               return Value::new_ident("t");
            } else {
               return Value::new_ident("f");
            }
         } else if ("nil" == v.ident) {
            if (v.args.size() < 1) {
               return v;
            }
            return Value::new_ident("t");
         } else if ("lt" == v.ident) {
            if (v.args.size() < 2) {
               return v;
            }
            Value a = reduce(v.args[0]);
            Value b = reduce(v.args[1]);
            assert(a.type == 1 && b.type == 1);
            if (a.value < b.value) {
               return Value::new_ident("t");
            } else {
               return Value::new_ident("f");
            }
         } else if ("mul" == v.ident) {
            if (v.args.size() < 2) {
               return v;
            }
            Value a = reduce(v.args[0]);
            Value b = reduce(v.args[1]);
            assert(a.type == 1 && b.type == 1);

            return Value::new_value(a.value * b.value);
         } else if ("neg" == v.ident) {
            if (v.args.size() < 1) {
               return v;
            }
            Value a = reduce(v.args[0]);
            assert(a.type == 1);

            return Value::new_value(-a.value);
         } else if ("s" == v.ident) {
            // s x y z = x z (y z)
            if (v.args.size() < 3) {
               return v;
            }
            Value yz = v.args[1];
            yz.args.push_back(v.args[2]);
            Value r = v.args[0];
            r.args.push_back(v.args[2]);
            r.args.push_back(yz);
            return reduce(r);
         } else {
            return v;
         }
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
   if (v.ident == "cons" && v.args.size() == 2) {
      cout << "(";
      print_value_sub(v.args[0]);
      cout << ", ";
      print_value_sub(v.args[1]);
      cout << ")";
   } else if (v.ident == "nil" && v.args.size() == 0) {
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

void print_value_sub(const Value &v) {
    switch (v.type) {
        case 0:
            cout << v.variable;
            break;
        case 1:
            cout << v.value;
            break;
        case 2:
            print_function(v);
            break;
        default:
            assert(false);
    }
}

void print_value(const Value &v) {
   print_value_sub(v);
   cout << endl;
}

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