/*
 * RTtask.cpp
 *
 *  Created on: 28-Jul-2018
 *      Author: ishan
 */

#define __USE_GNU
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include "RTtask.h"

#include <iostream>
using namespace std;

/*
 * MSR_PLATFORM_INFO[15:8] of MSR OxCE = 0x23
 * 0x23 = 35
 * 35 * 100MHz = 3.5GHz is the frequency of TSC
 * Processor’s support for invariant TSC is indicated by CPUID.80000007H:EDX[8].
 * This shows that the TSC frequency is constant
 */
#define TSC_Frequency 3500000000   //

#define CHECK(sts, msg) \
		if (sts == -1)          \
		{                                       \
			perror(msg);    \
			exit(-1);               \
		}							\
		printf("%s: Success\n", msg);

#define rdtsc_start(cycles_high, cycles_low)			\
		asm volatile ("CPUID\n\t"		\
				"RDTSC\n\t"						\
				"mov %%edx, %0\n\t"				\
				"mov %%eax, %1\n\t": "=r" (cycles_high), "=r" (cycles_low)::	\
				"%rax", "%rbx", "%rcx", "%rdx")

#define rdtsc_end(cycles_high1, cycles_low1)				\
		asm volatile("RDTSCP\n\t"										\
				"mov %%edx, %0\n\t"					\
				"mov %%eax, %1\n\t"							\
				"CPUID\n\t": "=r" (cycles_high1), "=r" (cycles_low1)::			\
				"%rax", "%rbx", "%rcx", "%rdx")

RT_task::RT_task(): PID(getpid()), core_id(0)
{
	task_function = NULL;
	// TODO Auto-generated constructor stub
}

RT_task::RT_task(__u64 sched_runtime, __u64 sched_soft_deadline, __u64 sched_deadline,
		const int _core_id, const timespec &phase, void (*_task_function) (void)) : PID(getpid()), core_id(_core_id)
{
	int sts = syscall(315, PID, &attr, sizeof (sched_attr), false);
	CHECK(sts, "sched_setattr()");
	attr.sched_policy = SCHED_DEADLINE;
	attr.sched_runtime = sched_runtime;
	attr.sched_soft_deadline = sched_soft_deadline;
	attr.sched_deadline = sched_deadline;
	attr.sched_period = sched_deadline;
	this->phase = phase;
	task_function = _task_function;
}

RT_task::~RT_task()
{
	// TODO Auto-generated destructor stubfd.
}

void RT_task::set_cpu_affinity()
{
	cpu_set_t cpuset;
	CPU_ZERO(&cpuset);
	CPU_SET(core_id, &cpuset);

	int set_result = sched_setaffinity(PID, sizeof(cpu_set_t), &cpuset);
	CHECK(set_result, "sched_setaffinity");
}

void RT_task::start_execution(int number_of_loops)
{
	int sts;
	unsigned int cycles_high = 0, cycles_low = 0;
	unsigned int cycles_high1 = 0, cycles_low1 = 0;
	unsigned long long cycle = 0, cycle1 = 0;
	//low | ((unsigned long long)high) << 32;

	set_cpu_affinity();

	syscall(332, PID, &attr);
	CHECK(sts, "sched_setparam_real()");

	// cpu_set_t: This data set is a bitset where each bit represents a CPU.
	cpu_set_t cpuset;
	// CPU_ZERO: This macro initializes the CPU set set to be the empty set.
	CPU_ZERO(&cpuset);
	// CPU_SET: This macro adds cpu to the CPU set set.
	CPU_SET(core_id, &cpuset);
	sts = syscall(333, PID, &phase, sizeof (cpu_set_t), &cpuset);
	CHECK(sts, "sched_do_job_release()");
	rdtsc_start(cycles_high, cycles_low);
	rdtsc_end(cycles_high1, cycles_low1);
	rdtsc_start(cycles_high, cycles_low);
	rdtsc_end(cycles_high1, cycles_low1);
	rdtsc_start(cycles_high, cycles_low);
	rdtsc_end(cycles_high1, cycles_low1);

	for (int k = 0; k < number_of_loops; k++)
	{
		rdtsc_start(cycles_high, cycles_low);
		task_function();
		rdtsc_end(cycles_high1, cycles_low1);


		cycle = cycles_low | ((unsigned long long)cycles_high) <<32;
		cycle1 = cycles_low1 | ((unsigned long long)cycles_high1) <<32;

		unsigned long long executionTime = cycle1 - cycle;
		cout<<"executionTime: "<<((double)executionTime / TSC_Frequency)<<endl;
		sts = syscall(334);
		CHECK(sts, "sched_do_job_complete()");
	}
	sts = syscall(335);
	CHECK(sts, "sched_task_complete()");
}
