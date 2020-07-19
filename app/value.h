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
