//
// Created by onceme on 2019/3/16.
//
#include "ae.h"
#include "redis.h"
#include "worker.h"

extern struct redisServer server;


//reactor线程读取事件回调函数  发送数据给worker线程
void reactorReadHandle(aeEventLoop *el,int connfd, void *privdata, int mask){
    redisClient *c = (redisClient*) privdata;
    //TODO 读取数据
    readQueryFromClient(el, connfd, privdata, mask);
    redisLog(REDIS_WARNING,"reactorReadHandle connfd %d ",connfd);
    aeEventLoop *worker_el = server.worker[0].el;
    redisLog(REDIS_WARNING,"reactorReadHandle worker_el->fired->fd %d ",worker_el->fired->fd);

    // 绑定到worker线程的事件循环,处于线程安全问题的考虑，暂时只使用一个worker线程
    if (aeCreateFileEvent(worker_el,connfd,AE_WRITABLE,
                          workerReadHandle, c) == AE_ERR)
    {
        close(connfd);
        zfree(c);
    }
}

/**
 * ReactorThread main Loop
 * 线程循环内容
 */
void rdReactorThread_loop(int *reactor_id)
{
    // 线程中的事件状态
    aeEventLoop *el;

    //线程id
    pthread_t thread_id = pthread_self();
    //创建事件驱动器
    el = aeCreateEventLoop(REDIS_MAX_CLIENTS);


    redisLog(REDIS_WARNING,"rdReactorThread_loop reactor_id %d ",*reactor_id);

    //存储线程相关信息
    server.reactors[*reactor_id].pidt = thread_id;
    server.reactors[*reactor_id].el = el;

    //进入事件循环
    aeMain(el);
}
