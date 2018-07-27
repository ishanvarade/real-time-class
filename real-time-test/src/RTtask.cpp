/*
 * RTtask.cpp
 *
 *  Created on: 28-Jul-2018
 *      Author: ishan
 */

#define __USE_GNU
#include <sched.h>
#include "RTtask.h"


RT_task::RT_task(): PID(getpid()), core_id(0) {
	// TODO Auto-generated constructor stub
}

RT_task::RT_task(const sched_attr & _attr, const int _core_id,
		const timespec & phase) : RT_task(){

}

RT_task::~RT_task()
{
	// TODO Auto-generated destructor stubfd
}

void RT_task::set_cpu_affinity()
{
	cpu_set_t cpuset;
	CPU_ZERO(&cpuset);
	CPU_SET(core_id, &cpuset);

	int set_result = sched_setaffinity(PID, sizeof(cpu_set_t), &cpuset);
}
