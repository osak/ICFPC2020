#include <cassert>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>

using namespace std;

std::ostream &operator<<(std::ostream &dest, __int128_t value) {
  std::ostream::sentry s(dest);
  if (s) {
    __uint128_t tmp = value < 0 ? -value : value;
    char buffer[128];
    char *d = std::end(buffer);
    do {
      --d;
      *d = "0123456789"[tmp % 10];
      tmp /= 10;
    } while (tmp != 0);
    if (value < 0) {
      --d;
      *d = '-';
    }
    int len = std::end(buffer) - d;
    if (dest.rdbuf()->sputn(d, len) != len) {
      dest.setstate(std::ios_base::badbit);
    }
  }
  return dest;
}

__int128 parsei128(string &s) {
  __int128 ret = 0;
  bool minus = false;
  for (int i = 0; i < s.length(); i++)
    if (s[i] == '-')
        minus = true;
    else if ('0' <= s[i] && s[i] <= '9')
      ret = 10 * ret + s[i] - '0';
  return ret * (minus ? -1 : 1);
}

enum Type {
    INTEGER,
    FUNCTION,
    APPLY
};

struct Value {
    static map<string, Value*> functions;
    
    Type type;
    __int128 num;
    string name;
    Value* left;
    Value* right;
    Value* reduced;
    
    Value() {}
    Value(Type type, __int128 num, const string& name, Value* left, Value* right, Value* reduced) : type(type), num(num), name(name), left(left), right(right), reduced(reduced) {}
    
    static Value* integer(__int128 num) {
        return new Value(INTEGER, num, "", nullptr, nullptr, nullptr);
    }
    
    static Value* func(const string& name) {
        if (functions.count(name)) {
            return functions[name];
        } else {
            return functions[name] = new Value(FUNCTION, 0, name, nullptr, nullptr, nullptr);
        }
    }
    
    static Value* ap(Value* left, Value* right) {
        return new Value(APPLY, 0, "", left, right, nullptr);
    }
    
    bool is_cons() {
        if (type != APPLY) return false;
        if (left->type != APPLY) return false;
        return left->left->name == "cons";
    }
    
    void print() {
        if (type == INTEGER) {
            cout << num;
        } else if (type == FUNCTION) {
            cout << name;
        } else {
            assert(left != nullptr);
            assert(right != nullptr);
            cout << "ap ";
            left->print();
            cout << " ";
            right->print();
        }
    }
};

map<string, Value*> Value::functions;

struct Reducer {
    map<string, Value*> env;
    
    void add_variable(const string& name, Value* value) {
        env[name] = value;
    }
    
    Value* reduce(Value* value) {
        if (value->reduced != nullptr) {
            return value->reduced;
        } else if (value->type == INTEGER) {
            return value;
        } else if (value->type == FUNCTION) {
            if (env.count(value->name)) {
                return env[value->name];
            } else {
                return value;
            }
        } else {
            Value* x1 = value->right;
            Value* func = full_reduce(value->left);
            if (func->type == FUNCTION) {
                if (func->name == "neg") {
                    x1 = full_reduce(x1);
                    assert(x1->type == INTEGER);
                    return Value::integer(-x1->num);
                }
                if (func->name == "i") return x1;
                if (func->name == "nil") return Value::func("t");
                if (func->name == "isnil") return Value::ap(x1, Value::ap(Value::func("t"), Value::ap(Value::func("t"), Value::func("f"))));
                if (func->name == "car") return Value::ap(x1, Value::func("t"));
                if (func->name == "cdr") return Value::ap(x1, Value::func("f"));
            } else if (func->type == APPLY) {
                Value* x2 = func->right;
                func = full_reduce(func->left);
                if (func->type == FUNCTION) {
                    if (func->name == "t") return x2;
                    if (func->name == "f") return x1;
                    if (func->name == "add" || func->name == "mul" || func->name == "div" || func->name == "lt" || func->name == "eq") {
                        x1 = full_reduce(x1);
                        x2 = full_reduce(x2);
                        assert(x1->type == INTEGER);
                        assert(x2->type == INTEGER);
                        if (func->name == "add") return Value::integer(x2->num + x1->num);
                        if (func->name == "mul") return Value::integer(x2->num * x1->num);
                        if (func->name == "div") return Value::integer(x2->num / x1->num);
                        if ((func->name == "lt" && x2->num < x1->num) || (func->name == "eq" && x2->num == x1->num)) {
                            return Value::func("t");
                        } else {
                            return Value::func("f");
                        }
                    }
                    if (func->name == "cons") {
                        x1 = full_reduce(x1);
                        x2 = full_reduce(x2);
                        Value* new_value = Value::ap(Value::ap(Value::func("cons"), x2), x1);
                        new_value->reduced = new_value;
                        return new_value;
                    }
                } else if (func->type == APPLY) {
                    Value* x3 = func->right;
                    func = full_reduce(func->left);
                    if (func->name == "s") return Value::ap(Value::ap(x3, x1), Value::ap(x2, x1));
                    if (func->name == "c") return Value::ap(Value::ap(x3, x1), x2);
                    if (func->name == "b") return Value::ap(x3, Value::ap(x2, x1));
                    if (func->name == "cons") return Value::ap(Value::ap(x1, x3), x2);
                }
            }
        }
        return value;
    }
    
    Value* full_reduce(Value* value) {
        if (value->reduced != nullptr) return value->reduced;
        Value* now = value;
        while (true) {
            Value* next = reduce(now);
            if (now == next) {
                value->reduced = next;
                return next;
            }
            now = next;
        }
    }
};

pair<Value*, int> parse(const string& line, int pos) {
    int next = line.find(' ', pos);
    if (next == -1) next = line.size();
    string term = line.substr(pos, next - pos);
    if (term == "ap") {
        pair<Value*, int> p1 = parse(line, next + 1);
        pair<Value*, int> p2 = parse(line, p1.second);
        return make_pair(Value::ap(p1.first, p2.first), p2.second);
    } else if (term[0] == '-' || (term[0] >= '0' && term[0] <= '9')) {
        return make_pair(Value::integer(parsei128(term)), next + 1);
    } else {
        return make_pair(Value::func(term), next + 1);
    }
}

Value* car(Value *node) {
    assert(node->is_cons());
    return node->left->right;
}

Value* cdr(Value *node) {
    assert(node->is_cons());
    return node->right;
}

void print_cons_list(Value *top) {
    vector<pair<Value*, int>> stk;

    stk.push_back(make_pair(top, 0));
    while (!stk.empty()) {
        auto &cur = stk.back();
        
        if (!cur.first->is_cons()) {
            cur.first->print();
            stk.pop_back();
            continue;
        }

        if (cur.second == 0) {
            cout << "(";
            cur.second = 1;
            stk.push_back(make_pair(cur.first->left->right, 0));
        } else if (cur.second == 1) {
            cout << ",";
            cur.second = 2;
            stk.push_back(make_pair(cur.first->right, 0));
        } else {
            cout << ")";
            stk.pop_back();
        }
    }
}

void interactive_output(Value *top) {
    cout << "full = ";
    print_cons_list(top);
    cout << endl;

    Value *state = car(cdr(top));
    Value *data = car(cdr(cdr(top)));

    cout << "state = ";
    print_cons_list(state);
    cout << endl;

    Value *cur = data;
    while (cur->is_cons()) {
        Value *img = car(cur);
        cout << "data = ";
        print_cons_list(img);
        cout << endl;

        cur = cdr(cur);
    }
}

int main(int argc, char* argv[]) {
    if (argc <= 1) {
        cerr << "file is not specified" << endl;
        return 1;
    }

    bool interactive_mode = false;
    string filename = "";
    if (string(argv[1]) == "--interactive") {
        if (argc <= 2) {
            cerr << "file is not specified" << endl;
            return 2;
        }
        interactive_mode = true;
        filename = string(argv[2]);
    } else {
        filename = string(argv[1]);
    }
    ifstream file(filename);
    if (file.is_open()) {
        cerr << "start parsing" << endl;
        Reducer reducer;
        string line;
        while (getline(file, line)) {
            int index = line.find('=');
            string name = line.substr(0, index - 1);
            Value* value = parse(line, index + 2).first;
            reducer.add_variable(name, value);
        }
        file.close();
        cerr << "end parsing" << endl;
        cerr << "start reducing" << endl;
        Value* top = Value::func(":galaxy");
        reducer.full_reduce(top);
        cerr << "end reducing" << endl;
        if (interactive_mode) {
            interactive_output(reducer.full_reduce(top));
        } else {
            Value* car = Value::func("car");
            Value* cdr = Value::func("cdr");
            cout << "flag: ";
            reducer.full_reduce(Value::ap(car, top))->print();
            cout << endl << "state: ";
            reducer.full_reduce(Value::ap(car, Value::ap(cdr, top)))->print();
            Value* data = Value::ap(car, Value::ap(cdr, Value::ap(cdr, top)));
            for (int i = 0; ; i++) {
                if (data->type == FUNCTION && data->name == "nil") break;
                    cout << endl << "data" << (i+1) << ": ";
                    reducer.full_reduce(Value::ap(car, data))->print();
                    cout << endl << endl;
                    data = reducer.full_reduce(Value::ap(cdr, data));
            }
            cout << endl;
        }
    } else {
        cerr << "file cannot be opened" << endl;
    }

    return 0;
}
