#include "clock_task.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/time.h>
#include <errno.h>
#include <signal.h>
#include "log/mig_log.h"
#include "common.h"
#include "linuxlist.h"
#include "plugins.h"

static struct list_head time_task;
static struct server* srvt = NULL;

static void on_time_msg(int fd, short event, void* argc) {

  struct list_head *tmp;
  struct list_head *n;
  struct time_task *task = NULL;
  //MIG_INFO(USER_LEVEL,"timer_task");
  if (list_empty(&time_task))
    return;

  list_for_each_safe(tmp,n,&time_task)
  {
    task = list_entry(tmp, struct time_task, list);
    task->current_time++;
    if (task->time == task->current_time) {
      //add thread
      if (task->ncount > -1)
        task->ncount--;
      //SINA_INFO(SYSTEM_LEVEL,"get time task time[%d] current_time[%d]\n",
      //    task->time,task->current_time);
      srvt->time_addtask(srvt, task);
      task->current_time = 0;
    }
    if (task->ncount == 0) {
      if (task) {
        list_del(&task->list);
        if (task->id) {
          free(task->id);
          task->id = NULL;
        }
        free(task);
        task = NULL;
      }
    }
  }

  struct timer_func_param* pParam = (struct timer_func_param*) argc;
  pParam->tv.tv_sec = 1;
  evtimer_add(pParam->timer_event, &pParam->tv);
  return;
}

static void del_time_task(struct server* srv, char* id, int opcode) {
  struct list_head *tmp;
  struct list_head *n;
  struct time_task *task = NULL;
  if (list_empty(&time_task)) {
    MIG_INFO(USER_LEVEL, "no time task");
    return;
  }
  list_for_each_safe(tmp,n,&time_task)
  {
    task = list_entry(tmp, struct time_task, list);
    if ((task->opcode == opcode) && (strcmp(task->id, id) == 0)) {
      list_del(&task->list);
      if (task->id) {
        free(task->id);
        task->id = NULL;
      }
      free(task);
      task = NULL;
    }
  }
}

static int add_time_task(struct server* srv, char* id, int opcode, int time,
                         int ncount) {
  struct time_task *task =
      ((struct time_task*) malloc(sizeof(struct time_task)));
  if (!task || srv == NULL)
    return 0;
  task->id = (char*) malloc(strlen(id) + 1);
  strcpy(task->id, id);
  task->opcode = opcode;
  task->time = time;
  task->ncount = ncount;
  task->current_time = 0;
  srvt = srv;
  list_add_tail(&task->list, &time_task);
  return 0;
}

int init_clock(struct server* srv) {

  srv->add_time_task = add_time_task;
  srv->del_time_task = del_time_task;
  INIT_LIST_HEAD(&time_task);
  plugins_call_handler_init_time(srv);

  struct timer_func_param* pParam = (struct timer_func_param*) malloc(
      sizeof(struct timer_func_param));
  memset(pParam, 0, sizeof(struct timer_func_param));
  pParam->tv.tv_sec = 1;
  pParam->timer_event = evtimer_new(srv->base, on_time_msg, pParam);
  evtimer_add(pParam->timer_event, &pParam->tv);
  srv->timer_resource = pParam;

  return 1;
}
