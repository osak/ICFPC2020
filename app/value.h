#pragma once

#include <iostream>
#include <vector>

using namespace std;

struct GalaxyValue {
	enum Type {
		NUMBER,
		LIST
	} type;
	long long num;
	vector<GalaxyValue*> list;

	GalaxyValue(long long num) : type(NUMBER), num(num) {}
	GalaxyValue(const vector<GalaxyValue*> &list) : type(LIST), list(list) {}
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

ostream& operator << (ostream &os, const GalaxyValue &gv) {
	if (gv.type == GalaxyValue::NUMBER) {
		os << gv.num;
	} else {
		os << "[";
		bool first = true;
		for (auto v : gv.list) {
			if (!first) {
				os << ",";
			}
			os << *v;
			first = false;
		}
		os << "]";
	}
	return os;
}

ostream& operator << (ostream &os, const vector<long long> &v) {
	bool first = true;
	os << "[";
	for (auto &val : v) {
		if (!first) {
			os << ",";
		}
		first = false;
		os << val;
	}
	os << "]";
	return os;
}

