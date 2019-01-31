#pragma once
#include <stdio.h>
#define TIMER_NAME_LEN 16

typedef void(*timer_callback)();

void timer_create_interval_frame(char name[TIMER_NAME_LEN], int frame, int loop, timer_callback cb);

void timer_create_interval_second(char name[TIMER_NAME_LEN], int second, int loop, timer_callback cb);

void timer_create_timer_day(char name[TIMER_NAME_LEN], int day_start, timer_callback cb);

void timer_create_timer_hour(char name[TIMER_NAME_LEN], int hour_start, int loop, timer_callback cb);

void timer_destroy(char name[TIMER_NAME_LEN]);

void timer_update();
