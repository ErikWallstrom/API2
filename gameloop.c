#include "gameloop.h"
#include "log.h"
#include <time.h>
#include <sched.h>

static uint64_t getperformancefreq(void)
{
	return 1000000000; //10^-9 (nano)
}

static uint64_t getperformancecount(void)
{
	struct timespec ts;
	timespec_get(&ts, TIME_UTC);

	uint64_t ticks = ts.tv_sec;
	ticks *= getperformancefreq();
	ticks += ts.tv_nsec;

	return ticks;
}

struct GameLoop* gameloop_ctor(
	struct GameLoop* self, 
	uint8_t tickspersec, 
	struct GameLoopCallback update,
	struct GameLoopCallback render)
{
	log_assert(self, "is NULL");

	self->tickrate = 1000.0 / tickspersec;
	self->oldtime = getperformancecount();
	self->tickscount = 0;
	self->framescount = 0;
	self->ticks = 0;
	self->fps = 0;
	self->seconds = 0;
	self->lag = 0.0;
	self->timedcallbacks = vec_ctor(struct GameLoopTimedCallback, 0);
	self->update = update;
	self->render = render;
	self->done = 0;

	//NOTE: Members below does not have initial values, but set anyways
	self->delta = 0.0;
	self->curtime = 0;
	self->interpolation = 0.0;

	return self;
}

void gameloop_start(struct GameLoop* self)
{
	log_assert(self, "is NULL");
	while(!self->done)
	{
		self->curtime = getperformancecount();
		self->delta = (self->curtime - self->oldtime) * 1000.0 
			/ getperformancefreq(); //Get delta ms
		self->lag += self->delta;
		self->oldtime = self->curtime;

		while(self->lag >= self->tickrate)
		{
			self->tickscount++;
			self->lag -= self->tickrate;
			if(self->update.func != NULL) //Should this be necessary?
			{
				self->update.func(self, self->update.userdata);
			}

			for(size_t i = 0; i	< vec_getsize(self->timedcallbacks); i++)
			{
				if(self->timedcallbacks[i].active)
				{
					self->timedcallbacks[i].curtime += self->tickrate;
					//Is the timing really accurate?
					if(self->timedcallbacks[i].curtime
						- self->timedcallbacks[i].oldtime
						>= self->timedcallbacks[i].delay)
					{
						self->timedcallbacks[i].oldtime
							= self->timedcallbacks[i].curtime;
						self->timedcallbacks[i].callback.func(
							self, 
							self->timedcallbacks[i].callback.userdata
						);
					}
				}
			}
		}

		self->framescount++;
		self->interpolation = self->lag / self->tickrate;
		if(self->render.func != NULL) //Should this be necessary?
		{
			self->render.func(self, self->render.userdata);
		}

		//Every second
		if(self->curtime / getperformancefreq() > self->seconds)
		{
			self->seconds = self->curtime / getperformancefreq();
			self->ticks = self->tickscount;
			self->tickscount = 0;
			self->fps = self->framescount;
			self->framescount = 0;

			#ifndef NDEBUG
			log_info("FPS: %zu, TICKS: %zu", self->fps, self->ticks);
			#endif
		}
	}
}

//Callbacks will be called after `update` function
GameLoopCallbackID gameloop_addcallback(
	struct GameLoop* self, 
	struct GameLoopCallback callback,
	double delay)
{
	log_assert(self, "is NULL");
	log_assert(callback.func, "is NULL");
	log_assert(delay > self->tickrate, "%f is not a valid delay", delay);

	vec_pushback(
		self->timedcallbacks, 
		(struct GameLoopTimedCallback){
			.callback = callback,
			.delay = delay,
			.oldtime = 0,
			.curtime = 0,
			.active = 1,
		}
	);

	return vec_getsize(self->timedcallbacks) - 1;
}

void gameloop_removecallback(struct GameLoop* self, GameLoopCallbackID id)
{
	log_assert(self, "is NULL");
	log_assert(id < vec_getsize(self->timedcallbacks), "invalid id (%zu)", id);

	//NOTE: Memory will not be freed
	self->timedcallbacks[id].active = 0;
}

void gameloop_yield(void)
{
	sched_yield(); //NOTE: No error handling
}

void gameloop_sleep(size_t ms)
{
	struct timespec spec;
	spec.tv_nsec = ms * 1000 * 1000;
	spec.tv_sec = 0;
	nanosleep(&spec, NULL);
}

void gameloop_dtor(struct GameLoop* self)
{
	log_assert(self, "is NULL");
	vec_dtor(self->timedcallbacks);
}

