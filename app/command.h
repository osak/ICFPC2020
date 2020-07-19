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
};

ostream& operator << (ostream &os, const Vector &v) {
   os << "(" << v.x << "," << v.y << ")" << endl;
	return os;
}

struct Command {
   virtual void modulate(Modulator& mod) {

   }
};

struct Move : Command {
   Vector acceleration;
   Move(const Vector& acceleration) : acceleration(acceleration) {}
   virtual void modulate(Modulator& mod) {
      //[4, 3985075716549918131, []]
      mod.put_cell();
      mod.put_number(4);
      mod.put_cell();
      mod.put_number(3985075716549918131);
      mod.put_cell();
      mod.put_nil();
      mod.put_nil();
      /*
      mod.put_cell();
      mod.put_number(0);
      // mod.put_cell();
      acceleration.modulate(mod);
      // mod.put_nil();
      */
   }
};

struct Kamikaze : Command {
   virtual void modulate(Modulator& mod) {
      mod.put_cell();
      mod.put_number(1);
      mod.put_nil();
   }
};

struct Attack : Command {
   Vector targetLocation;
   virtual void modulate(Modulator& mod) {
      mod.put_cell();
      mod.put_number(2);
      mod.put_cell();
      targetLocation.modulate(mod);
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