#pragma once

#include "modem.h"
using namespace std;

struct Vector {
   long long x, y;
   Vector() : x(0), y(0) {}
   Vector(long long x, long long y) : x(x), y(y) {}
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
};

struct Kamikaze : Command {
   Kamikaze(int unit_id) : Command(unit_id) {}
   virtual void modulate(Modulator& mod) {
      mod.put_cell();
      mod.put_number(1);
      mod.put_cell();
      mod.put_number(0);
      mod.put_nil();
   }
};

struct Attack : Command {
   Vector targetLocation;
   Attack(int unit_id, const Vector& targetLocation) : Command(unit_id), targetLocation(targetLocation) {}
   virtual void modulate(Modulator& mod) {
      mod.put_cell();
      mod.put_number(2);
      mod.put_cell();
      targetLocation.modulate(mod);
      mod.put_nil();
   }
};

struct Fission : Command {
   StartParams childParams;
   Fission(int unit_id, const StartParams& childParams) : Command(unit_id), childParams(childParams) {}
   virtual void modulate(Modulator & mod) {
      mod.put_cell();
      mod.put_number(3);
      mod.put_cell();
		mod.put_number(childParams.engine);
      mod.put_cell();
		mod.put_cell();
		mod.put_number(childParams.armament);
		mod.put_cell();
		mod.put_number(childParams.reactor);
		mod.put_cell();
		mod.put_number(childParams.core);
		mod.put_nil();
      mod.put_nil();
   }
};

struct JoinParams {
   string modulate() const {
      Modulator mod;
		mod.put_nil();
      return mod.to_string();
   }
};
