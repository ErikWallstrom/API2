#ifndef GAMELOOP_H
#define GAMELOOP_H

#include <inttypes.h>
#include "vec.h"

struct GameLoop;
typedef void(*GameLoopFunction)(struct GameLoop* gameloop, void* userdata);
typedef size_t GameLoopCallbackID;

struct GameLoopCallback
{
	GameLoopFunction func;
	void* userdata;
};

struct GameLoopTimedCallback
{
	struct GameLoopCallback callback;
	double oldtime;
	double curtime;
	double delay;
	int active;
};

struct GameLoop
{
	uint64_t oldtime;
	uint64_t curtime;
	uint64_t tickscount;
	uint64_t framescount;
	uint64_t fps;
	uint64_t ticks;
	uint64_t seconds;
	double lag;
	double delta;
	double interpolation;
	double tickrate;
	Vec(struct GameLoopTimedCallback) timedcallbacks;
	struct GameLoopCallback update;
	struct GameLoopCallback render;
	int done;
};

struct GameLoop* gameloop_ctor(
	struct GameLoop* self, 
	double tickrate, 
	struct GameLoopCallback update,
	struct GameLoopCallback render
);
void gameloop_start(struct GameLoop* self);
GameLoopCallbackID gameloop_addcallback(
	struct GameLoop* self, 
	struct GameLoopCallback callback,
	double delay
);
void gameloop_removecallback(struct GameLoop* self, GameLoopCallbackID id);
void gameloop_dtor(struct GameLoop* self);

#endif
