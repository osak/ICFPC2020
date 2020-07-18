#include <cassert>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>

using namespace std;

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
    long long num;
    string name;
    Value* left;
    Value* right;
    
    Value() {}
    Value(Type type, long long num, const string& name, Value* left, Value* right) : type(type), num(num), name(name), left(left), right(right) {}
    
    static Value* create_integer(long long num) {
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
            cout << "(";
            left->print();
            cout << " ";
            right->print();
            cout << ")";
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
    
    pair<bool, Value*> reduce(Value* value) {
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
    
    Value* full_reduce(Value* value) {
        while (true) {
            pair<bool, Value*> p = reduce(value);
            value = p.second;
            if (!p.first) return value;
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
        return make_pair(Value::create_integer(stoll(term)), next + 1);
    } else {
        return make_pair(Value::create_function(term), next + 1);
    }
}

int main(int argc, char* argv[]) {
    if (argc <= 1) {
        cerr << "file is not specified" << endl;
    } else {
        ifstream file(argv[1]);
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
            top->print();
            cout << endl;
            cerr << "end reducing" << endl;
            Value* car = Value::create_function("car");
            Value* cdr = Value::create_function("cdr");
            top = Value::create_apply(car, Value::create_apply(car, Value::create_apply(cdr, Value::create_apply(cdr, top))));
            top = reducer.full_reduce(top);
            while (true) {
                if (top->name == "nil") break;
                assert(top->is_cons());
                Value* p = reducer.full_reduce(top->left->right);
                assert(p->is_cons());
                reducer.full_reduce(p->left->right)->print();
                cout << ",";
                reducer.full_reduce(p->right)->print();
                cout << endl;
                top = reducer.full_reduce(top->right);
            }
        } else {
            cerr << "file cannot be opened" << endl;
        }
    }
    
    return 0;
}
