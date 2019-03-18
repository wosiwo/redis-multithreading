//
// Created by onceme on 2019/3/16.
//
#include "redis.h"
#include "ae.h"
#include "worker.h"


//接收reactor线程触发的读事件
void workerReadHandle(aeEventLoop *el,int connfd, void *privdata, int mask){
    redisClient *c = (redisClient*) privdata;

    redisLog(REDIS_VERBOSE,"workerReadHandle reactor_id %d connfd %d ",c->reactor_id,connfd);

    //从worker线程事件循环中删除这个连接 ，避免重复执行
    aeDeleteFileEvent(el,connfd,AE_WRITABLE);

    processInputBuffer(c);  //执行客户端操作命令

//    close(c->fd);
    //
    //将返回结果抛给原来的reactor线程 的操作



}


/**
 * ReactorThread main Loop
 * 线程循环内容
 */
void rdWorkerThread_loop(int worker_id) {
    // 线程中的事件状态
    aeEventLoop *el;

    //线程id
    pthread_t thread_id = pthread_self();
    //创建事件驱动器
    el = aeCreateEventLoop(REDIS_MAX_CLIENTS);


    redisLog(REDIS_WARNING,"rdWorkerThread_loop worker_id %d ", worker_id);

    //存储线程相关信息
    server.worker[worker_id].pidt = thread_id;
    server.worker[worker_id].el = el;

    //进入事件循环
    aeMain(el);

}
