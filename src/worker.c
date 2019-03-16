//
// Created by onceme on 2019/3/16.
//
#include "redis.h"



//接收reactor线程触发的读事件
int workerReadHandle(aeEventLoop *el,int connfd, void *privdata, int mask){
    redisClient *c = (redisClient*) privdata;

    processInputBuffer(c);  //执行客户端操作命令

    //将返回结果抛给原来的reactor线程 的操作



}


/**
 * ReactorThread main Loop
 * 线程循环内容
 */
int rdWorkerThread_loop(int worker_id) {
    int pipe_fd;
    int sockfd;
    // 线程中的事件状态
    aeEventLoop *el;

    //线程id
    pthread_t thread_id = pthread_self();
    //创建事件驱动器
    el = aeCreateEventLoop(REDIS_MAX_CLIENTS);


    printf("rdReactorThread_loop reactor_id %d el %d \n", reactor_id, el);

    //存储线程相关信息
    server.worker[worker_id].pidt = thread_id;
    server.worker[worker_id].el = el;

    //进入事件循环
    aeMain(el);

}