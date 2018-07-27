/*
 * RTtask.h
 *
 *  Created on: 28-Jul-2018
 *      Author: ishan
 */

#include <linux/types.h>
#include <unistd.h>
#include <time.h>


#ifndef RTTASK_H_
#define RTTASK_H_

const int SCHED_DEADLINE = 6;

struct sched_attr
{
        __u32 size;

        __u32 sched_policy;
        __u64 sched_flags;

        /* SCHED_NORMAL, SCHED_BATCH */
        __s32 sched_nice;

        /* SCHED_FIFO, SCHED_RR */
        __u32 sched_priorit;

        /* SCHED_DEADLINE */
        __u64 sched_runtime;
        __u64 sched_soft_deadline;
        __u64 sched_deadline;
        __u64 sched_period;
};


class RT_task {
private:
	sched_attr attr;
	const pid_t PID; // make this constant later
	const int core_id;
	timespec phase;
	void set_cpu_affinity();
public:
	RT_task();
	//RT_task(const sched_attr & _attr, const int _core_id, const timespec & phase);
	void start_execution();
	virtual ~RT_task();
};

#endif /* RTTASK_H_ */
