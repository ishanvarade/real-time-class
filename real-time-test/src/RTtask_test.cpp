/*
 * RTtask_test.cpp
 *
 *  Created on: 28-Jul-2018
 *      Author: ishan
 */

#include "RTtask.h"
#include <iostream>
#include <unistd.h>

#include <stdio.h>
using namespace std;

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

void task_fun(void)
{
	unsigned int time = 3000000;
	usleep(time);
	cout<<"task_fun\n"<<endl;
}

int main()
{
	struct timespec phase;
	phase.tv_sec = 4;
	phase.tv_nsec = 0;

	RT_task task = RT_task(3000000, 8000000, 9000000, 2, phase, &task_fun);
	task.start_execution();


	unsigned int cycles_high = 0, cycles_low = 0;
	unsigned int cycles_high1 = 0, cycles_low1 = 0;
	unsigned long long cycle = 0, cycle1 = 0;
	rdtsc_start(cycles_high, cycles_low);
	task_fun();
	rdtsc_end(cycles_high1, cycles_low1);

	cycle = cycles_low | ((unsigned long long)cycles_high) <<32;
	cycle1 = cycles_low1 | ((unsigned long long)cycles_high1) <<32;
	unsigned long long executionTime = cycle1 - cycle;
	printf ("Execution Time: %llu\n", executionTime);
	return 0;
}
