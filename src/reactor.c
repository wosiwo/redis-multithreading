//
// Created by onceme on 2019/3/16.
//
#include "redis.h"
#include "ae.h"
#include "worker.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

extern struct redisServer server;


//reactor线程读取事件回调函数  发送数据给worker线程
void reactorReadHandle(aeEventLoop *el,int connfd, void *privdata, int mask){
    redisClient *c = (redisClient*) privdata;
    //TODO 读取数据

    //原子交换 cron_switch 为1时替换为0，并返回true,否则不替换，返回false
//    while(!AO_CASB(&c->cron_switch,1,0)){
//        redisLog(REDIS_DEBUG,"reactorReadHandle wait lock reactor_id %d connfd %d ",c->reactor_id,connfd);
//
//        continue;   //循环等待获取锁
//    }

    int ret = readQueryFromClient(el, connfd, privdata, mask);

    if(!ret){    //读到eof或者客户端关闭连接，不再把连接抛给woker线程
//        c->cron_switch=1;       //解锁
        redisLog(REDIS_WARNING,"querybuf null reactor_id %d connfd %d ",c->reactor_id,connfd);
        return;
    }
    redisLog(REDIS_NOTICE,"reactorReadHandle reactor_id %d c->request_times %d connfd %d ",c->reactor_id,c->request_times,connfd);

    c->request_times++;      //增加请求次数
    aeEventLoop *worker_el = server.worker[0].el;
//    redisLog(REDIS_DEBUG,"reactorReadHandle reactor_id %d worker_el->fired->fd %d confd %d",c->reactor_id,worker_el->fired->fd,connfd);

    //数据读取完需要立即触发woker线程执行，不能等待连接可写
    //将客户端信息添加到worker线程的队列中
    atomListAddNodeTail(server.worker[0].clients,c);
//    printf("clients list len %d connfd %d \n",server.worker[0].clients->len,c->fd);

    //通过管道通知worker线程
    int pipeWriteFd = server.worker[0].pipMasterFd;
    char buf[1];
    buf[0] = 'c';

    char str[5];
    sprintf(str,"%d",connfd);   //数字转字符串

    ret = write(pipeWriteFd, str, 5);
    redisLog(REDIS_NOTICE,"reactorReadHandle reactor_id %d  c->request_times %d pipeWriteFd %d write %d c %p connfd %s",c->reactor_id,c->request_times,pipeWriteFd,ret,c,str);

//    c->cron_switch=1;       //解锁

//    redisLog(REDIS_WARNING,"prepare write to worker c->reactor_id %d ret %d",c->reactor_id,ret);


    // 绑定到worker线程的事件循环,处于线程安全问题的考虑，暂时只使用一个worker线程
//    if (aeCreateFileEvent(worker_el,connfd,AE_WRITABLE,
//                          workerReadHandle, c) == AE_ERR)
//    {
//        freeClient(c);
//    }
}

/**
 * ReactorThread main Loop
 * 线程循环内容
 */
void rdReactorThread_loop(int reactor_id)
{
    // 线程中的事件状态
    aeEventLoop *el;

    //线程id
    pthread_t thread_id = pthread_self();
    //创建事件驱动器
    el = aeCreateEventLoop(REDIS_MAX_CLIENTS);


    redisLog(REDIS_WARNING,"rdReactorThread_loop reactor_id %d ",reactor_id);

    //存储线程相关信息
    server.reactors[reactor_id].pidt = thread_id;
    server.reactors[reactor_id].el = el;

    //进入事件循环
    aeMain(el);
}
