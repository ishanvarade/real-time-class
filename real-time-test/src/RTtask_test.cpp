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
#include <unistd.h>

#include <stdio.h>
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
	unsigned int time = 3000000;
	usleep(time);
	cout<<"task_fun\n"<<endl;
}

int main_1()
{
	struct timespec phase;
	phase.tv_sec = 4;
	phase.tv_nsec = 0;

	RT_task task = RT_task(3000000, 8000000, 9000000, 2, phase, &task_fun);
	task.start_execution();
	return 0;
}

int main(int argc, char *argv[])
{
	const int core_id_3 = 3;
	cpu_set_t cpuset;
	CPU_ZERO(&cpuset);
	CPU_SET(core_id_3, &cpuset);

	sched_setaffinity(getpid(), sizeof(cpu_set_t), &cpuset);


	unsigned int time;
	unsigned int cycles_high = 0, cycles_low = 0;
	unsigned int cycles_high1 = 0, cycles_low1 = 0;
	unsigned long long cycle = 0, cycle1 = 0;
	//ticks tick,tick1,tickh;

	//tick = getticks();
	for (time = 0; time < 10000000; time += 100000)
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
		unsigned long long executionTime = cycle1 - cycle;
		printf ("%llu   %llu\n", time, executionTime);
	}
}
