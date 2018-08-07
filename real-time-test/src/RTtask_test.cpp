/*
 * RTtask_test.cpp
 *
 *  Created on: 28-Jul-2018
 *      Author: ishan
 */
#define __USE_GNU
#include <sched.h>

#include "RTtask.h"
#include <iostream>
#include <climits>
#include <unistd.h>

#include <stdio.h>

#define TwentyMS 698597200
#define TenMS 349298600
#define ThirtyMS_loops 54750000
#define TwentyMS_loops 36500000
#define TenMS_loops 17500000

/*
 * MSR_PLATFORM_INFO[15:8] of MSR OxCE = 0x23
 * 0x23 = 35
 * 35 * 100MHz = 3.5GHz is the frequency of TSC
 * Processor’s support for invariant TSC is indicated by CPUID.80000007H:EDX[8].
 * This shows that the TSC frequency is constant
 */
#define TSC_Frequency 3500000000   //

using namespace std;

#define rdtsc_start(cycles_high, cycles_low)			\
		asm volatile ("CPUID\n\t"		\
				"RDTSC\n\t"						\
				"mov %%edx, %0\n\t"				\
				"mov %%eax, %1\n\t": "=r" (cycles_high), "=r" (cycles_low)::\
				"%rax", "%rbx", "%rcx", "%rdx")

#define rdtsc_end(cycles_high1, cycles_low1)				\
		asm volatile("RDTSCP\n\t"										\
				"mov %%edx, %0\n\t"					\
				"mov %%eax, %1\n\t"							\
				"CPUID\n\t": "=r" (cycles_high1), "=r" (cycles_low1)::		\
				"%rax", "%rbx", "%rcx", "%rdx")

typedef unsigned long long ticks;
static __inline__ ticks getticks(void)
{
	unsigned a, d;
	asm("cpuid");
	asm volatile("rdtsc" : "=a" (a), "=d" (d));

	return (((ticks)a) | (((ticks)d) << 32));
}

void task_fun(void)
{
	for (int i = 0; i < ThirtyMS_loops; i++)
	{
		asm volatile ( "movl $10, %eax;"
				"movl $20, %ebx;"
				"addl %ebx, %eax;"
		);
	}
}

int main_1(int argc, char *argv[])
{
	struct timespec phase;
	phase.tv_sec = 15;
	phase.tv_nsec = 0;
	__u64 wcet = 100000;
	__u64 soft_deadline = 1000000;
	__u64 deadline = 1000000;
	int core_id = 5;

	RT_task task = RT_task(wcet, soft_deadline, deadline, core_id, phase, &task_fun);
	task.start_execution();
	return 0;
}

int main(int argc, char *argv[])
{
	const int core_id_5 = 5;
	cpu_set_t cpuset;
	CPU_ZERO(&cpuset);
	CPU_SET(core_id_5, &cpuset);

	sched_setaffinity(getpid(), sizeof(cpu_set_t), &cpuset);


	unsigned int time;
	unsigned int cycles_high = 0, cycles_low = 0;
	unsigned int cycles_high1 = 0, cycles_low1 = 0;
	unsigned long long cycle = 0, cycle1 = 0;
	//ticks tick,tick1,tickh;

	unsigned long long executionTime;
	//tick = getticks();
	rdtsc_start(cycles_high, cycles_low);

	task_fun();

	rdtsc_end(cycles_high1, cycles_low1);
	cycle = cycles_low | ((unsigned long long)cycles_high) <<32;
	cycle1 = cycles_low1 | ((unsigned long long)cycles_high1) <<32;
	executionTime = cycle1 - cycle;

//	printf ("executionTime: %llu\n", executionTime);
	cout<<"executionTSC: "<<executionTime<<endl<<"executionTime: "<<((double)executionTime / TSC_Frequency)<<endl;
	return 0;
}

//698598403
int main_3(int argc, char *argv[])
{
	const int core_id_4 = 4;
	cpu_set_t cpuset;
	CPU_ZERO(&cpuset);
	CPU_SET(core_id_4, &cpuset);

	sched_setaffinity(getpid(), sizeof(cpu_set_t), &cpuset);


	unsigned int time;
	unsigned int cycles_high = 0, cycles_low = 0;
	unsigned int cycles_high1 = 0, cycles_low1 = 0;
	unsigned long long cycle = 0, cycle1 = 0;
	//ticks tick,tick1,tickh;

	//tick = getticks();
	time = 2000000;
	unsigned long long executionTime;
	unsigned long long cumulativeExecutionTime = 0;
	unsigned long long averageExecutionTime;
	unsigned long long minExecutionTime = ULLONG_MAX;

	//Warm-up
	rdtsc_start(cycles_high, cycles_low);
	rdtsc_end(cycles_high1, cycles_low1);
	rdtsc_start(cycles_high, cycles_low);
	rdtsc_end(cycles_high1, cycles_low1);
	rdtsc_start(cycles_high, cycles_low);
	rdtsc_end(cycles_high1, cycles_low1);

	for (int i = 0; i < 20; i++)
	{

		rdtsc_start(cycles_high, cycles_low);

		//	asm volatile ( "movl $10, %eax;"
		//			"movl $20, %ebx;"
		//			"addl %ebx, %eax;"
		//	);
		//cout<<"Start\n";
		usleep(time);
		//	time = (unsigned)((tick1-tick)/1662543);

		// tick1 = getticks();
		rdtsc_end(cycles_high1, cycles_low1);

		//cout<<"End\n";

		//printf("\ntime in MS%llu\n",tick1-tick);
		cycle = cycles_low | ((unsigned long long)cycles_high) <<32;
		cycle1 = cycles_low1 | ((unsigned long long)cycles_high1) <<32;
		executionTime = cycle1 - cycle;
		if (executionTime < minExecutionTime)
			minExecutionTime = executionTime;
		cumulativeExecutionTime += executionTime;
		averageExecutionTime = cumulativeExecutionTime / (i + 1);
		printf ("%llu   %llu	%llu	%llu %llu, %lf %lf \n", time, executionTime,
				cumulativeExecutionTime, averageExecutionTime, minExecutionTime,
				((double)executionTime) / 3501000000, ((double)executionTime) / 3500000000);
	}
}

