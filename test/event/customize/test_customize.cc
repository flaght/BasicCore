//  Copyright (c) 2015-2016 The kid Authors. All rights reserved.
//   Created on: 2016.12.23 Author: kerry

#include "gtest/gtest.h"

class CustomizeEventTest: public testing::Test {
};

#include <ctype.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <syslog.h>
#include <sys/time.h>
#include <errno.h>

#include <event2/event.h>
#include <event2/event_struct.h>
#include <event2/util.h>

struct timeval lasttime;
int event_is_persistent;
//struct event* ev2;
struct event *ev1, *ev2;

#define CBFUNC 10021

void cb_func(evutil_socket_t fd, short what, void *arg)
{
            const char *data = (const char*)arg;
            printf("Got an event on socket %d:%s%s%s%s [%s]\n",
                               (int) fd,
                               (what&EV_TIMEOUT) ? " timeout" : "",
                               (what&EV_READ)    ? " read" : "",
                               (what&EV_WRITE)   ? " write" : "",
                               (what&EV_SIGNAL)  ? " signal" : "",
                               (what&CBFUNC)  ? " func" : "",
                               data);
}



static void
timeout_cb(evutil_socket_t fd, short event, void *arg)
{
        struct timeval newtime, difference;
        struct event *timeout =(struct event*) arg;
        double elapsed;

        evutil_gettimeofday(&newtime, NULL);
        evutil_timersub(&newtime, &lasttime, &difference);
        elapsed = difference.tv_sec + (difference.tv_usec / 1.0e6);

        printf("timeout_cb called at %d: %.3f seconds elapsed.\n",
                   (int)newtime.tv_sec, elapsed);
        lasttime = newtime;

        if (!event_is_persistent) {
            struct timeval tv;
            evutil_timerclear(&tv);
            tv.tv_sec = 2;
            event_add(timeout, &tv);
            event_active(ev2,CBFUNC,1);
        }
}




TEST(CustomizeEventTest, Basic){
    struct event timeout;
    struct timeval tv;
    //struct event *ev1, *ev2;
    struct event_base* base;
    int flags;
    base = event_base_new();

    event_assign(&timeout, base, -1, flags, timeout_cb, (void*)&timeout);

    evutil_timerclear(&tv);

    tv.tv_sec = 2;

    event_add(&timeout, &tv);

    ev2 = event_new(base, 102, EV_WRITE|CBFUNC|EV_READ, cb_func,(char*)"Writing event");

    evutil_gettimeofday(&lasttime, NULL);
    event_base_dispatch(base);
}
