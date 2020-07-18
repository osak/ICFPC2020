#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <sstream>
#include <fstream>
#include <cassert>
#include <set>
#include <stack>

using namespace std;

struct Value {
    int type;
    string variable; // type == 0
    long long value; // type == 1
    string ident; // type == 2
    vector<Value> args;

    Value() {}

    Value(int type, string variable, long long value, string ident) : type(type), variable(variable), value(value),
                                                                      ident(ident) {
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
                auto b = st.top(); st.pop();
                auto a = st.top(); st.pop();
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

void print_value(Value &v, int indent) {
    string ind = string(indent, ' ');
    cout << ind << "type: " << v.type << endl;
    cout << ind << "value: ";
    switch (v.type) {
        case 0:
            cout << v.variable << endl;
            break;
        case 1:
            cout << v.value << endl;
            break;
        case 2:
            cout << v.ident << endl;
            break;
        default:
            assert(false);
    }
    cout << ind << "args:" << endl;
    for (auto &a : v.args) {
        print_value(a, indent + 2);
    }
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

        cout << "read " << result.size() << " lines" << endl;
        for (auto e: result) {
            cout << e.first << '\n';
            print_value(e.second, 0);
            cout << '\n' << "}" << endl;
        }

        print_keywords(vs);

        file.close();
    }
}

