#include "timer.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "tools.h"
#include "ftime.h"
#define TIMER_INFINITY -255

typedef struct timer
{
	char name[TIMER_NAME_LEN];
	int loop;
	union
	{
		int val;
		int day_start;
		int hour_start;
	} interval;
	int ty;
	int cv;
	timer_callback cb;

	timer* next;
}timer;

typedef struct timer_mgr
{
	timer* thead;
}timer_mgr;

typedef void (*op_update)(timer* t);

static timer_mgr tmgr;

static void timer_mgr_add_timer(timer* t)
{
	if (tmgr.thead == NULL)
	{
		tmgr.thead = (timer*)malloc(sizeof(timer));
		memset(tmgr.thead, 0, sizeof(timer));
	}	
	timer* ptr = tmgr.thead;	
	while (ptr->next)	
		ptr = ptr->next;
	ptr->next = t;
}

void timer_create_interval_frame(char name[TIMER_NAME_LEN], int interval, int loop, timer_callback cb)
{
	timer* t = (timer*)malloc(sizeof(timer));
	strcpy_s(t->name, TIMER_NAME_LEN, name);
	t->interval.val = interval;
	t->loop = loop < 0 ? TIMER_INFINITY : loop;
	t->cv = 0;
	t->ty = 0;
	t->cb = cb;	
	t->next = NULL;
	timer_mgr_add_timer(t);
}

void timer_create_interval_second(char name[TIMER_NAME_LEN], int second, int loop, timer_callback cb)
{
	timer* t = (timer*)malloc(sizeof(timer));
	strcpy_s(t->name, TIMER_NAME_LEN, name);
	t->interval.val = second;
	t->loop = loop < 0 ? TIMER_INFINITY : loop;
	t->cv = second;
	t->ty = 1;
	t->cb = cb;
	t->next = NULL;
	timer_mgr_add_timer(t);
}

void timer_create_timer_day(char name[TIMER_NAME_LEN], int day_start, timer_callback cb)
{
	timer* t = (timer*)malloc(sizeof(timer));
	strcpy_s(t->name, TIMER_NAME_LEN, name);
	t->interval.val = day_start;
	t->loop = TIMER_INFINITY;
	t->cv = 0;
	t->ty = 2;
	t->cb = cb;
	t->next = NULL;
	timer_mgr_add_timer(t);
}

void timer_create_timer_hour(char name[TIMER_NAME_LEN], int hour_start, int loop, timer_callback cb)
{
	timer* t = (timer*)malloc(sizeof(timer));
	strcpy_s(t->name, TIMER_NAME_LEN, name);
	t->interval.val = hour_start;
	t->loop = TIMER_INFINITY;
	t->cv = 0;
	t->ty = 3;
	t->cb = cb;
	t->next = NULL;
	timer_mgr_add_timer(t);
}

void timer_destroy(char name[TIMER_NAME_LEN])
{
	timer *ptr, *pre;
	pre = tmgr.thead;
	ptr = pre->next;
	while (ptr)
	{
		if (strcmp(name, ptr->name) == 0)
		{
			pre->next = ptr->next;
			free(ptr);
			ptr = pre->next;
		}
		else
		{
			pre = ptr;
			ptr = ptr->next;
		}
	}
}

static void timer_update_frame(timer* t)
{
	t->cv += 1;
	if (t->cv >= t->interval.val)
	{
		t->cv = 0;
		(*t->cb)();
		if (t->loop != TIMER_INFINITY)
		{
			t->loop--;
		}
	}
}

static void timer_update_second(timer* t)
{
	float tm = Time::time();
	if (tm > (float)t->cv)
	{
		t->cv = (int)tm + t->interval.val;
		(*t->cb)();
		if (t->loop != TIMER_INFINITY)
		{
			t->loop--;
		}
	}
}

static void timer_update_second_day(timer* t)
{
	int s = GetStartSecondToday();
	if (s >= t->interval.day_start)
	{
		if (t->cv == 0)
		{
			(*t->cb)();
			t->cv = 1;
		}
	}
	else
	{
		if (t->cv == 1)
		{
			t->cv = 0;
		}
	}
}

static void timer_update_second_hour(timer* t)
{
	int s = GetStartSecondHour();
	if (s >= t->interval.day_start)
	{
		if (t->cv == 0)
		{
			(*t->cb)();
			t->cv = 1;
		}
	}
	else
	{
		if (t->cv == 1)
		{
			t->cv = 0;
		}
	}
}

static op_update op[] = {
	timer_update_frame,
	timer_update_second,
	timer_update_second_day,
	timer_update_second_hour,
	NULL
};

void timer_update()
{
	if (tmgr.thead == NULL)
		return;

	timer *ptr, *pre;
	pre = tmgr.thead;
	ptr = pre->next;
	while (ptr)
	{
		(*op[ptr->ty])(ptr);

		if (ptr->loop > TIMER_INFINITY && ptr->loop <= 0)
		{
			pre->next = ptr->next;
			free(ptr);
			ptr = pre->next;
		}
		else
		{
			pre = ptr;
			ptr = ptr->next;
		}		
	}
}


