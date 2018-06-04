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
	struct GameLoopCallback update;
	struct GameLoopCallback render;
	uint64_t oldtime;
	uint64_t curtime;
	size_t tickscount;
	size_t framescount;
	size_t fps;
	size_t ticks;
	size_t seconds;
	double lag;
	double delta;
	double interpolation;
	double tickrate;
	Vec(struct GameLoopTimedCallback) timedcallbacks;
	int done;
};

struct GameLoop* gameloop_ctor(
	struct GameLoop* self, 
	uint8_t tickspersec, 
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
void gameloop_yield(void);
void gameloop_sleep(size_t ms);
void gameloop_dtor(struct GameLoop* self);

#endif
