/*
 * ESPTimerService.h
 *
 *  Created on: 2016. nov. 7.
 *      Author: Arpad Fejes
 */

#ifndef ESPTIMERSERVICE_ESPTIMERSERVICE_H_
#define ESPTIMERSERVICE_ESPTIMERSERVICE_H_

#define CONTAINER_SIZE 10

extern "C" {
#include "user_interface.h"
}

typedef void (*timerCallback)(void* pArg);

/********************************************************************************************/
/******************************** ESPTimer Stuff Starts *************************************/
/********************************************************************************************/
typedef struct ESPTimer{

	int timer_ms;	//the duration of the timer tick in milliseconds
	int timer_id;	//to bind the timer to an event
	bool periodic;	//Periodic if true
	timerCallback callbackFunction; //this callback will be called when the timer fires (tick occurred)
	bool set;		// if true, starts ticking, if false do nothing
	os_timer_t os_timer;

}ESPTimer;

/********************************* ESPTimer related functions *******************************/

void ESPTimer_init(ESPTimer* timer, int timer_ms, int timer_id, bool periodic, timerCallback yourCallback);
void ESPTimer_setTimer(ESPTimer* timer);
void ESPTimer_unsetTimer(ESPTimer* timer);
int  ESPTimer_getID(ESPTimer* timer);

/********************************************************************************************/
/********************************************************************************************/
/********************************************************************************************/

/********************************************************************************************/
/****************************** ESPTimerService stuff starts ********************************/
/********************************************************************************************/
typedef struct ESPTimerService{

	ESPTimer* tmrContainer;	//Array of the timers.
	int size;				//Size of the tmrContainer
	int count;				//Number of timers in the container.

}ESPTimerService;

/**************************** ESPTimerService related functions *****************************/


void ESPTS_init(ESPTimerService* timerService);
void ESPTS_init(ESPTimerService* timerService, ESPTimer* tmrContainerIN, int size);
void ESPTS_createTimer(ESPTimerService* timerService, int timer_ms, int timer_id, bool periodic, timerCallback yourCallback);
void ESPTS_setTimer(ESPTimerService* timerService, int timer_id);
void ESPTS_unsetTimer(ESPTimerService* timerService, int timer_id);
int  ESPTS_findTimer(ESPTimerService* timerService, int timer_id);
void ESPTS_deleteTimerService(ESPTimerService* timerService);

/********************************************************************************************/
/********************************************************************************************/
/********************************************************************************************/

#endif /* ESPTIMERSERVICE_ESPTIMERSERVICE_H_ */
