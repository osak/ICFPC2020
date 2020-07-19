#pragma once

#include "modem.h"
using namespace std;

struct Vector {
   long long x, y;
   Vector() : x(0), y(0) {}
   Vector(long long x, long long y) : x(x), y(y) {}
   Vector(pair<int, int> pair) : x(pair.first), y(pair.second) {}
   void modulate(Modulator& mod) {
      mod.put_cell();
      mod.put_number(x);
      mod.put_number(y);
   }

   double norm() const {
      return sqrt(x * x + y * y);
   }

   double operator*(const Vector& other) const {
      return x * other.x + y * other.y;
   }

   Vector operator+(const Vector& other) const {
      return Vector(x + other.x, y + other.y);
   }

   bool operator==(const Vector& other) const {
      return x == other.x && y == other.y;
   }
};

ostream& operator << (ostream &os, const Vector &v) {
   os << "(" << v.x << "," << v.y << ")";
	return os;
}

struct Command {
   int unit_id;
   Command(int unit_id) : unit_id(unit_id) {}
   virtual void modulate(Modulator& mod) {
   }
   virtual void print() {
   }
};

struct StartParams {
   int engine, armament, reactor, core;
   StartParams(int engine, int armament, int reactor, int core) : engine(engine), armament(armament), reactor(reactor), core(core) {}
   string modulate() const {
      Modulator mod;
		mod.put_cell();
		mod.put_number(engine);
		mod.put_cell();
		mod.put_number(armament);
		mod.put_cell();
		mod.put_number(reactor);
		mod.put_cell();
		mod.put_number(core);
		mod.put_nil();
      return mod.to_string();
   }
};

struct CommandParams {
   vector<Command*> commands;
   string modulate() const {
      Modulator mod;
      for (auto com : commands) {
         mod.put_cell();
         com->modulate(mod);
      }
      mod.put_nil();
      return mod.to_string();
   }
};

struct Move : Command {
   Vector acceleration;
   Move(int unit_id, const Vector& acceleration) : Command(unit_id), acceleration(acceleration) {}
   virtual void modulate(Modulator& mod) {
      mod.put_cell();
      mod.put_number(0);
      mod.put_cell();
      mod.put_number(unit_id);
      mod.put_cell();
      acceleration.modulate(mod);
      mod.put_nil();
   }
   virtual void print() {
      cout << "Move(" << unit_id << ") " << acceleration;
   }
};

struct Kamikaze : Command {
   Kamikaze(int unit_id) : Command(unit_id) {}
   virtual void modulate(Modulator& mod) {
      mod.put_cell();
      mod.put_number(1);
      mod.put_cell();
      mod.put_number(unit_id);
      mod.put_nil();
   }
   virtual void print() {
      cout << "Kamikaze(" << unit_id << ") ";
   }
};

struct Attack : Command {
   Vector target_location;
   int power;
   Attack(int unit_id, const Vector& target_location, int power) : Command(unit_id), target_location(target_location), power(power) {}
   virtual void modulate(Modulator& mod) {
      mod.put_cell();
      mod.put_number(2);
      mod.put_cell();
      mod.put_number(unit_id);
      mod.put_cell();
      target_location.modulate(mod);
      mod.put_cell();
      mod.put_number(power);
      mod.put_nil();
   }
   virtual void print() {
      cout << "Attack(" << unit_id << ") " << target_location << " " << power;
   }
};

struct Fission : Command {
   StartParams childParams;
   Fission(int unit_id, const StartParams& childParams) : Command(unit_id), childParams(childParams) {}
   virtual void modulate(Modulator & mod) {
      mod.put_cell();
      mod.put_number(3);
      mod.put_cell();
      mod.put_number(unit_id);
      mod.put_cell();
		mod.put_cell();
		mod.put_number(childParams.engine);
      mod.put_cell();
		mod.put_number(childParams.armament);
		mod.put_cell();
		mod.put_number(childParams.reactor);
		mod.put_cell();
		mod.put_number(childParams.core);
		mod.put_nil();
      mod.put_nil();
   }
   virtual void print() {
      cout << "Fission(" << unit_id << ") " << childParams.engine << ", " << childParams.armament << ", " << childParams.reactor << ", " << childParams.core;
   }
};

struct JoinParams {
   long long secret1 = 192496425430LL;
   long long secret2 = 103652820LL;
   string modulate() const {
      Modulator mod;
      mod.put_cell();
      mod.put_number(secret1);
      mod.put_cell();
      mod.put_number(secret2);
		mod.put_nil();
      return mod.to_string();
   }
};
