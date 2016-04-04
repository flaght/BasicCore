/*=============================================================================
#    Copyright (c) 2015
#    ShanghaiKunyan.  All rights reserved
#
#    Filename     : /home/jiao/svc/library/core/loop_queue.h
#    Author       : jiaoyongqing
#    Email        : 657974959@qq.com
#    Date         : 2016-03-03 09:37
#    Description  : 
=============================================================================*/

#ifndef _LINUX_SHARE_DATA_H
#define _LINUX_SHARE_DATA_H

#include "core/common.h"

class ShareData{
 public:
  ShareData() : data(NULL){
    pthread_mutex_init(&mutex_lock, NULL);
  }
  
 virtual int set_data (void *param) { return 0;}
 virtual int get_data (void *param) { return 0;}

 void lock() { pthread_mutex_lock(&mutex_lock); }
 void unlock() { pthread_mutex_unlock(&mutex_lock); }

	void *data;
  pthread_mutex_t mutex_lock;
};

class UseShareData {
 public:
  UseShareData(ShareData *share_data) : share_data_(share_data) {
    share_data_->lock();
  }
  ~UseShareData() {
    share_data_->unlock();
  }

  int set_data (void *param) {share_data_->set_data(param);}
  int get_data (void *param) {share_data_->get_data(param);}

 private:
  ShareData *share_data_;
};


#endif
