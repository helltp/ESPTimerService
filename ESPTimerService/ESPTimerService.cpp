/*
 * ESPTimerService.cpp
 *
 *    Created on: 2016. nov. 7.
 *        Author: Arpad Fejes
 *       Version: 0.3
 * Last modified: 2016.11.24.
 */

extern "C" {
#include "user_interface.h"
}

#include "ESPTimerService.h"
#include "mem.h"

/*See mem.h START*/
bool ICACHE_FLASH_ATTR check_memleak_debug_enable(void)
{
    return MEMLEAK_DEBUG_ENABLE;
}
/*See mem.h END*/

#define DEFAULT_CONTAINER_SIZE 10
#define INVALID_VALUE -17

/********************************************************************************************/
/******************************* ESPTimer function definitions ******************************/
/********************************************************************************************/

void ESPTimer_init(ESPTimer* timer, int timer_ms, int timer_id, bool periodic, timerCallback yourCallback);
void ESPTimer_setTimer(ESPTimer* timer);
void ESPTimer_unsetTimer(ESPTimer* timer);
int  ESPTimer_getID(ESPTimer* timer);

/********************************************************************************************/
/********************************************************************************************/
/********************************************************************************************/

/********************************************************************************************/
/********************************* ESPTimer related functions *******************************/
/********************************************************************************************/

/**
 * Default initializer. Initializes a timer, sets the timer time in milliseconds, the id of the timer, and the repeat flag (periodic).
 * Also binds a callback function to the timer through the given function pointer argument.
 * @param timer 		The target ESPTimer instance.
 * @param timer_ms		Timer duration the timer in milliseconds.
 * @param timer_id		Unique ID for the timer.
 * @param periodic		Repeat flag.
 * @param yourCallback	This is the callback functions function pointer what's binded to the timer. When the timer fires, calls this function.
 */
void ESPTimer_init(ESPTimer* timer, int timer_ms, int timer_id, bool periodic, timerCallback yourCallback){
	timer->timer_ms = timer_ms;
	timer->timer_id = timer_id;
	timer->periodic = periodic;
	timer->callbackFunction = yourCallback;
	timer->set = false;

	/* The first parameter is  a pointer for the os_timer behind the ESPTimer.
	 * The second parameter is the callback function which have a (void)(fp)(void*) signature.
	 * The third parameter will be the void* parameter of the callback function when the callback called. It must be the id of the timer,
	 * so when multiple timers call the same callback function, we can check which of them called it.
	 */
	os_timer_setfn(&(timer->os_timer), timer->callbackFunction, (void*)(timer_id));  //MAGIC

}


/**
 * Starts the timer.
 * @param timer 		The target ESPTimer instance.
 */
void ESPTimer_setTimer(ESPTimer* timer){
	os_timer_arm(&(timer->os_timer), timer->timer_ms, timer->periodic);
}

/**
 * Stops the timer.
 * @param timer 		The target ESPTimer instance.
 */
void ESPTimer_unsetTimer(ESPTimer* timer){
	os_timer_disarm(&(timer->os_timer));
}

/**
 * Returns the id of the timer.
 * @param timer 		The target ESPTimer instance.
 * @return				The id.
 */
int  ESPTimer_getID(ESPTimer* timer){
	return timer->timer_id;
}

/********************************************************************************************/
/********************************************************************************************/
/********************************************************************************************/

/********************************************************************************************/
/**************************** ESPTimerService related functions *****************************/
/********************************************************************************************/

/**
 * Default initializer. Makes a container for DEFAULT_CONTAINER_SIZE Timers.
 *
 * @param timerService The target TimerService instance.
 */
void ESPTS_init(ESPTimerService* timerService){

	/*NOTE: os_malloc returns void* so we need to cast it in to the desired form.*/
	timerService->tmrContainer = (ESPTimer*)os_malloc(DEFAULT_CONTAINER_SIZE * sizeof(ESPTimer*)); //MAGIC
	timerService->size = DEFAULT_CONTAINER_SIZE;
	timerService->count = 0;

}

/**
 * Creates and initializes a Timer and put it in the container.
 *
 * @param timerService	The target TimerService instance.
 * @param timer_ms		Timer duration the timer in milliseconds.
 * @param timer_id		Unique ID for the timer.
 * @param periodic		Repeat flag.
 * @param yourCallback	The callback function to call when the timer fires.
 */
void ESPTS_createTimer(ESPTimerService* timerService, int timer_ms, int timer_id, bool periodic, timerCallback yourCallback){
	ESPTimer timer;
	ESPTimer_init(&timer,timer_ms,timer_id,periodic,yourCallback);

	for(int i=0; i<timerService->size; i++){

		if(timerService->tmrContainer[i].timer_id == NULL){					//Look for the first free slot.
			timerService->tmrContainer[i] = timer;							//Put the timer there.
			timerService->count++;
			break;
		}

	}
}

/**
 * Sets the timer specified by the given timer_id. (Basically starts the timer.)
 *
 * @param timerService	The target TimerService instance.
 * @param timer_id		ID of the timer to be started.
 */
void ESPTS_setTimer(ESPTimerService* timerService, int timer_id){

	ESPTimer_setTimer(&(timerService->tmrContainer[ESPTS_findTimer(timerService, timer_id)]));

}

/**
 * Unsets the timer specified by the given timer_id. (Basically stops the timer.)
 *
 * @param timerService	The target TimerService instance.
 * @param timer_id		ID of the timer to be stopped.
 */
void ESPTS_unsetTimer(ESPTimerService* timerService, int timer_id){

	ESPTimer_unsetTimer(&(timerService->tmrContainer[ESPTS_findTimer(timerService, timer_id)]));

	/*We need to set the ID to NULL so when the next time creating a timer, this slot will be free.*/
	timerService->tmrContainer[ESPTS_findTimer(timerService, timer_id)].timer_id = NULL;

}

/**
 * Find a timer by the given ID. FOR INTERNAL USE ONLY.
 *
 * @param timerService	The target TimerService instance.
 * @param timer_id
 * @return 				The address of the specified timer in the container.
 */
int  ESPTS_findTimer(ESPTimerService* timerService, int timer_id){
	for(int i=0; i<timerService->size; i++){

			if(timerService->tmrContainer[i].timer_id == timer_id){
				return i;
			}

		}
	return INVALID_VALUE;
}

/**
 * To free the memory allocated for the tmrContainer.
 *
 * @param timerService	The target TimerService instance.
 */
void ESPTS_deleteTimerService(ESPTimerService* timerService){
	os_free(timerService->tmrContainer);
}

/********************************************************************************************/
/********************************************************************************************/
/********************************************************************************************/
