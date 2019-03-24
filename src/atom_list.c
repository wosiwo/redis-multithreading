/* adlist.c - A generic doubly linked list implementation
 *
 * Copyright (c) 2006-2010, Salvatore Sanfilippo <antirez at gmail dot com>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *   * Redistributions of source code must retain the above copyright notice,
 *     this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *   * Neither the name of Redis nor the names of its contributors may be used
 *     to endorse or promote products derived from this software without
 *     specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */


#include <stdlib.h>
#include <pthread.h>
#include <sys/types.h>
#include <stdio.h>
#include "atom_list.h"
#include "zmalloc.h"

/* Create a new list. The created list can be freed with
 * AlFreeList(), but private value of every node need to be freed
 * by the user before to call AlFreeList().
 *
 * On error, NULL is returned. Otherwise the pointer to the new list. */
/*
 * 创建一个新的链表
 *
 * 创建成功返回链表，失败返回 NULL 。
 *
 * T = O(1)
 */
list *atomListCreate(void)
{
    struct list *list;

    // 分配内存
    if ((list = zmalloc(sizeof(*list))) == NULL)
        return NULL;

    // 初始化属性
    list->head = list->tail = NULL;
    list->len = 0;
    list->atom_switch = 1;
    list->dup = NULL;
    list->free = NULL;
    list->match = NULL;
    pthread_mutex_init(&list->mutex, NULL);

    return list;
}



void *atomListPop(list *list) {
//    redisLog(REDIS_NOTICE,"listPop \n");
//    pthread_mutex_lock(&list->mutex);   //获得互斥锁
//    pthread_mutex_unlock(&list->mutex); //释放互斥锁
//    redisLog(REDIS_NOTICE,"listPop pthread_mutex_lock \n");

    listNode *node;
    void *value;
    printf("listPop list->len %d \n",list->len);


//    node = listFirst(list);
    do {
        node = list->head; //取链表头指针的快照
        printf("listPop node \n");

        if (node == NULL){
            printf("listPop node null \n");
            return NULL;
        }
    } while( AO_CASB(&list->head, node, list->head->next) != TRUE); //如果没有把结点链在尾指针上，再试
//    AO_CASB(&list->head, p, node); //置尾结点

    printf("listPop get node \n");

//    if (node == NULL) {
//        pthread_mutex_unlock(&list->mutex); //释放互斥锁

//        return NULL;
//    }

    value = listNodeValue(node);
//    pthread_mutex_unlock(&list->mutex); //释放互斥锁
    printf("listPop  node val \n");

//    listDelNode(list, node);

    // 调整后置节点的指针
    AO_CASB(&list->tail, node, node->prev); //如果当前节点是表头节点
    // 释放节点
    zfree(node);

    // 链表数减一
//    list->len--;
    incListLen(list,-1);
    printf("listDelNode list->len-- \n");


    printf("listPop del node \n");


//    if (list->free) return NULL;

    return value;
}

/* Add a new node to the list, to tail, containing the specified 'value'
 * pointer as value.
 *
 * On error, NULL is returned and no operation is performed (i.e. the
 * list remains unaltered).
 * On success the 'list' pointer you pass to the function is returned. */
/*
 * 将一个包含有给定值指针 value 的新节点添加到链表的表尾
 *
 * 如果为新节点分配内存出错，那么不执行任何动作，仅返回 NULL
 *
 * 如果执行成功，返回传入的链表指针
 *
 * T = O(1)
 */
list *atomListAddNodeTail(list *list, void *value)
{
//    //atom_switch TODO 使用cas直接置换
//    while(!AO_CASB(&list->atom_switch,1,0)){
//        continue;   //循环等待获取锁
//    }
//    pthread_mutex_lock(&list->mutex);   //获得互斥锁
    listNode *node;

    // 为新节点分配内存
    if ((node = zmalloc(sizeof(*node))) == NULL)
        return NULL;

    // 保存值指针
    node->value = value;
    node->next = NULL;

//    listNode *tail = list->tail;
    listNode *p;
    //节点放到表尾
    p = list->tail; //取链表尾指针的快照
    if(AO_CASB(&list->tail, NULL, node) == TRUE){ //表尾指针为空
        AO_CASB(&list->head, NULL, node);    //这时表头指针也应该为NULL
//        node->prev = NULL;
    }else{
        do {
            p = list->tail; //取链表尾指针的快照
        } while( AO_CASB(&p->next, NULL, node) != TRUE); //如果没有把结点链在尾指针上，再试
        AO_CASB(&list->tail, p, node); //置尾结点
        node->prev = p;
    }
//
//
//    // 目标链表为空
//    if (list->len == 0) {
//        list->head = list->tail = node;
//        node->prev = NULL;
//    // 目标链表非空
//    } else {
//        node->prev = list->tail;
//        list->tail->next = node;
//        list->tail = node;
//    }

    // 更新链表节点数
//    list->len++;
    incListLen(list,1);  //++不是原子操作

//    AO_CASB(&list->atom_switch,0,1); //释放锁
//    pthread_mutex_unlock(&list->mutex); //释放互斥锁

    return list;
}

/**
 * 原子操作修改链表长度
 * @param list
 * @param inc
 */
int incListLen(list *list,int inc){
    unsigned long val;
    do {
        val = list->len + inc; //取链表尾指针的快照
    } while( AO_CASB(&list->len, list->len, val) != TRUE);
    printf("listAddNodeTail list->len %d \n",list->len);
    return val;
}
