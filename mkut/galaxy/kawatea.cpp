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

map<string, int> arg_size = {
    {"add", 2},
    {"mul", 2},
    {"div", 2},
    {"lt", 2},
    {"neg", 1},
    {"eq", 2},
    {"s", 3},
    {"c", 3},
    {"b", 3},
    {"i", 1},
    {"t", 2},
    {"f", 2},
    {"cons", 3},
    {"car", 1},
    {"cdr", 1},
    {"nil", 1},
    {"isnil", 1}
};

enum Type {
    INTEGER,
    FUNCTION,
    APPLY
};

struct Value {
    Type type;
    __int128 num;
    string name;
    Value* left;
    Value* right;
    
    Value() {}
    Value(Type type, __int128 num, const string& name, Value* left, Value* right) : type(type), num(num), name(name), left(left), right(right) {}
    
    static Value* create_integer(__int128 num) {
        return new Value(INTEGER, num, "", nullptr, nullptr);
    }
    
    static Value* create_function(const string& name) {
        return new Value(FUNCTION, 0, name, nullptr, nullptr);
    }
    
    static Value* create_apply(Value* left, Value* right) {
        return new Value(APPLY, 0, "", left, right);
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
            //cout << ")";
        }
    }
};

struct Reducer {
    map<string, Value*> env;
    
    void add_variable(const string& name, Value* value) {
        env[name] = value;
    }
    
    bool same(Value* v1, Value* v2) {
        if (v1 == nullptr && v2 == nullptr) return true;
        if (v1 == nullptr && v2 != nullptr) return false;
        if (v1 != nullptr && v2 == nullptr) return false;
        if (v1->type != v2->type) return false;
        if (v1->name != v2->name) return false;
        if (v1->num != v2->num) return false;
        if (!same(v1->left, v2->left)) return false;
        if (!same(v1->right, v2->right)) return false;
        return true;
    }
    
    pair<bool, Value*> reduce(Value* value, bool stop_on_cons = false) {
        if (value->type == INTEGER) {
            return make_pair(false, value);
        } else if (value->type == FUNCTION) {
            if (env.count(value->name)) {
                return make_pair(true, env[value->name]);
            } else {
                return make_pair(false, value);
            }
        } else {
            vector<Value*> values;
            while (value->type == APPLY) {
                values.push_back(value);
                value = value->left;
            }
            assert(value->type == FUNCTION);
            if (env.count(value->name)) {
                values.back()->left = env[value->name];
                return make_pair(true, values[0]);
            } else {
                int size = arg_size[value->name];
                if (value->name == "cons" && values.size() == 2 && !stop_on_cons) {
                    Value* x0 = full_reduce(values[values.size() - 1]->right, false);
                    Value* x1 = full_reduce(values[values.size() - 2]->right, false);
                    values[values.size() - 1]->right = x0;
                    values[values.size() - 2]->right = x1;
                }
                if (values.size() < size) return make_pair(false, values[0]);
                Value* next;
                if (value->name == "add" || value->name == "mul" || value->name == "div" || value->name == "lt") {
                    Value* x0 = full_reduce(values[values.size() - 1]->right);
                    Value* x1 = full_reduce(values[values.size() - 2]->right);
                    assert(x0->type == INTEGER);
                    assert(x1->type == INTEGER);
                    if (value->name == "add") {
                        next = Value::create_integer(x0->num + x1->num);
                    } else if (value->name == "mul") {
                        next = Value::create_integer(x0->num * x1->num);
                    } else if (value->name == "div") {
                        next = Value::create_integer(x0->num / x1->num);
                    } else if (value->name == "lt") {
                        if (x0->num < x1->num) {
                            next = Value::create_function("t");
                        } else {
                            next = Value::create_function("f");
                        }
                    }
                } else if (value->name == "neg") {
                    Value* x0 = full_reduce(values[values.size() - 1]->right);
                    assert(x0->type == INTEGER);
                    next = Value::create_integer(-x0->num);
                } else if (value->name == "eq") {
                    Value* x0 = full_reduce(values[values.size() - 1]->right);
                    Value* x1 = full_reduce(values[values.size() - 2]->right);
                    if (same(x0, x1)) {
                        next = Value::create_function("t");
                    } else {
                        next = Value::create_function("f");
                    }
                } else if (value->name == "s") {
                    Value* x0 = values[values.size() - 1]->right;
                    Value* x1 = values[values.size() - 2]->right;
                    Value* x2 = values[values.size() - 3]->right;
                    next = Value::create_apply(Value::create_apply(x0, x2), Value::create_apply(x1, x2));
                } else if (value->name == "c") {
                    Value* x0 = values[values.size() - 1]->right;
                    Value* x1 = values[values.size() - 2]->right;
                    Value* x2 = values[values.size() - 3]->right;
                    next = Value::create_apply(Value::create_apply(x0, x2), x1);
                } else if (value->name == "b") {
                    Value* x0 = values[values.size() - 1]->right;
                    Value* x1 = values[values.size() - 2]->right;
                    Value* x2 = values[values.size() - 3]->right;
                    next = Value::create_apply(x0, Value::create_apply(x1, x2));
                } else if (value->name == "i") {
                    next = values[values.size() - 1]->right;
                } else if (value->name == "t") {
                    next = values[values.size() - 1]->right;
                } else if (value->name == "f") {
                    next = values[values.size() - 2]->right;
                } else if (value->name == "cons") {
                    Value* x0 = values[values.size() - 1]->right;
                    Value* x1 = values[values.size() - 2]->right;
                    Value* x2 = values[values.size() - 3]->right;
                    next = Value::create_apply(Value::create_apply(x2, x0), x1);
                } else if (value->name == "car" || value->name == "cdr") {
                    Value* x0 = full_reduce(values[values.size() - 1]->right);
                    assert(x0->is_cons());
                    if (value->name == "car") {
                        next = x0->left->right;
                    } else if (value->name == "cdr") {
                        next = x0->right;
                    }
                } else if (value->name == "nil") {
                    next = Value::create_function("t");
                } else if (value->name == "isnil") {
                    Value* x0 = full_reduce(values[values.size() - 1]->right);
                    if (x0->name == "nil") {
                        next = Value::create_function("t");
                    } else {
                        next = Value::create_function("f");
                    }
                } else {
                    return make_pair(false, values[0]);
                }
                if (values.size() == size) {
                    return make_pair(true, next);
                } else {
                    values[values.size() - size - 1]->left = next;
                    return make_pair(true, values[0]);
                }
            }
        }
    }
    
    Value* full_reduce(Value* value, bool stop_on_cons = true) {
        Value* original = value;
        while (true) {
            pair<bool, Value*> p = reduce(value, stop_on_cons);
            value = p.second;
            if (!p.first) {
                original->type = value->type;
                original->name = value->name;
                original->num = value->num;
                original->left = value->left;
                original->right = value->right;
                return value;
            }
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
        return make_pair(Value::create_apply(p1.first, p2.first), p2.second);
    } else if (term[0] == '-' || (term[0] >= '0' && term[0] <= '9')) {
        return make_pair(Value::create_integer(parsei128(term)), next + 1);
    } else {
        return make_pair(Value::create_function(term), next + 1);
    }
}

Value* car(Value *node) {
    Value* car = Value::create_function("car");
    return Value::create_apply(car, node);
}

Value* cdr(Value *node) {
    Value* cdr = Value::create_function("cdr");
    return Value::create_apply(cdr, node);
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

void interactive_output(Value *top, Reducer &reducer) {
    cout << "full = ";
    print_cons_list(reducer.full_reduce(top));
    cout << endl;

    Value *state = reducer.full_reduce(car(cdr(top)), false);
    Value *data = reducer.full_reduce(car(cdr(cdr(top))), false);

    cout << "state = ";
    print_cons_list(state);
    cout << endl;

    Value *cur = data;
    while (cur->is_cons()) {
        Value *img = reducer.full_reduce(car(cur));
        cout << "data = ";
        print_cons_list(img);
        cout << endl;

        cur = reducer.full_reduce(cdr(cur));
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
        Value* top = Value::create_function(":galaxy");
        while (true) {
            pair<bool, Value*> p = reducer.reduce(top);
            top = p.second;
            if (!p.first) break;
        }
        //top->print();
        cout << endl;
        cerr << "end reducing" << endl;
        if (interactive_mode) {
            interactive_output(top, reducer);
        } else {
            Value* car = Value::create_function("car");
            Value* cdr = Value::create_function("cdr");
            cout << "flag: ";
            reducer.full_reduce(Value::create_apply(car, top))->print();
            cout << endl << "state: ";
            reducer.full_reduce(Value::create_apply(car, Value::create_apply(cdr, top)))->print();
            Value* data = Value::create_apply(car, Value::create_apply(cdr, Value::create_apply(cdr, top)));
            for (int i = 0; ; i++) {
                if (data->type == FUNCTION && data->name == "nil") break;
                    cout << endl << "data" << (i+1) << ": ";
                    reducer.full_reduce(Value::create_apply(car, data))->print();
                    cout << endl << endl;
                    data = reducer.full_reduce(Value::create_apply(cdr, data));
            }
            cout << endl;
            //top = reducer.full_reduce(top);
        }
    } else {
        cerr << "file cannot be opened" << endl;
    }

    return 0;
}
