#pragma once

#include <string>
#include <vector>
#include <iostream>
#include <cassert>

#include "value.h"

using namespace std;

class Modulator {
  public:
    void put_number(long long num) {
        if (num >= 0) {
            buffer_ += "01";
        } else {
            buffer_ += "10";
            num = -num;
        }

        assert(num >= 0);
        int bits = 0;
        while ((1ULL << bits) <= num && bits < 64)
            bits += 4;
        buffer_ += string(bits / 4, '1');
        buffer_ += "0";

		cout << "[DBG] modulating number: " << num << " " << bits << endl;
        if (bits > 0) {
            unsigned long long mask = (1ULL << (bits - 1));
            while (mask > 0) {
                buffer_ += (num & mask) ? "1" : "0";
                mask >>= 1;
            }
        }
    }

	void put_cell() {
		buffer_ += "11";
	}

	void put_nil() {
		buffer_ += "00";
	}

	void put_list(const vector<long long> &nums) {
		for (auto n : nums) {
			put_cell();
			put_number(n);
		}
		put_nil();
	}

	void put_raw(const string& modulated) {
		buffer_ += modulated;
	}

	string to_string() const {
		return buffer_;
	}

  private:
    string buffer_;
};

struct ConsCell;
struct Value {
	enum Type {
		NUMBER,
		PTR
	};

	Type type;
	long long value;
	ConsCell *ptr;

	Value(long long v) : type(NUMBER), value(v) {}
	Value(ConsCell *ptr) : type(PTR), ptr(ptr) {}
};

struct ConsCell {
	Value *car, *cdr;

	ConsCell(Value *car, Value *cdr) : car(car), cdr(cdr) {}
};

ostream& operator <<(ostream&, const ConsCell&);
ostream& operator << (ostream &os, const Value &v) {
	if (v.type == Value::NUMBER) {
		os << v.value;
	} else if (v.ptr == nullptr) {
		os << "nil";
	} else {
		os << *v.ptr;
	}
	return os;
}

ostream& operator << (ostream &os, const ConsCell &v) {
	os << "(" << *v.car << "," << *v.cdr << ")";
	return os;
}

class Demodulator {
	public:
	Value* demodulate(const string &str) {
		int pos = 0;
		return demodulate_top(str, pos);
	}

	private:
	Value* demodulate_top(const string &str, int &pos) {
		const string tag = str.substr(pos, 2);
		pos += 2;
		if (tag == "00") {
			return new Value(nullptr);
		} else if (tag == "01") {
			return demodulate_number(str, pos, 1);
		} else if (tag == "10") {
			return demodulate_number(str, pos, -1);
		} else if (tag == "11") {
			return demodulate_cell(str, pos);
		}
		assert(false);
	}

	Value* demodulate_number(const string &str, int &pos, int sign) {
		int bits = 0;
		while(str[pos] == '1') {
			bits += 4;
			pos++;
		}
		pos++;

		long long value = 0;
		for (int i = 0; i < bits; ++i) {
			value <<= 1;
			if (str[pos] == '1') value += 1;
			pos++;
		}
		return new Value(value * sign);
	}

	Value* demodulate_cell(const string &str, int &pos) {
		Value *car = demodulate_top(str, pos);
		Value *cdr = demodulate_top(str, pos);
		return new Value(new ConsCell(car, cdr));
	}
};

GalaxyValue* as_galaxy(Value *v) {
	if (v->type == Value::NUMBER) {
		return new GalaxyValue(v->value);
	}
	if (v->ptr == nullptr) {
		return new GalaxyValue(vector<GalaxyValue*>());
	}

	vector<GalaxyValue*> result;
	const Value *cur = v;
	while (cur->type == Value::PTR && cur->ptr != nullptr) {
		result.push_back(as_galaxy(cur->ptr->car));
		cur = cur->ptr->cdr;
	}

	// treat cons lists like (1, (2, 3)) as [1, 2, 3]
	if (cur->type == Value::NUMBER) {
		result.push_back(new GalaxyValue(cur->value));
	}
	return new GalaxyValue(result);
}
