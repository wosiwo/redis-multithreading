//
// Created by onceme on 2019/3/16.
//

#ifndef REDIS_3_0_ANNOTATED_REACTOR_H
#define REDIS_3_0_ANNOTATED_REACTOR_H

#endif //REDIS_3_0_ANNOTATED_REACTOR_H

void rdReactorThread_loop(int reactor_id);   //IO线程
void reactorReadHandle(aeEventLoop *el,int connfd, void *privdata, int mask);