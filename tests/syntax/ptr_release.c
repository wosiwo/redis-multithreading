//
// Created by YD-YF-2018090603-001 on 2019/3/25.
//

#include <stdlib.h>
#include <pthread.h>
#include <stdio.h>
#include <sys/types.h>
#include "../../src/zmalloc.h"
#include "../../src/config.h"

/**
 * 测试结构体释放后，其他持有指针的变量的表现
 * @return
 */
/*
 * 双端链表节点
 */
typedef struct listNode {

    // 前置节点
    struct listNode *prev;

    // 后置节点
    struct listNode *next;

    // 节点的值
    int value;

} listNode;

int main(){

    listNode *node;

    // 为新节点分配内存
    if ((node = zmalloc(sizeof(*node))) == NULL){
//        return NULL;

    }
    // 保存值指针
    node->value = 1;
    node->next = NULL;
    node->prev = NULL;

    listNode *tmp = node;

    printf("node %p sizeof %d node->value %d \n",node, sizeof(*node),node->value);
    printf("tmp %p sizeof %d tmp->value %d \n",tmp, sizeof(*tmp),tmp->value);
    zfree(node);

    printf("after zfree node %p sizeof %d node->value %d \n",node, sizeof(*node),node->value);
    printf("after zfree tmp %p sizeof %d tmp->value %d \n",tmp, sizeof(*tmp),tmp->value);

    printf("after zfree node %p sizeof %b \n",node, sizeof(*node));
    printf("after zfree tmp %p sizeof %b \n",tmp, sizeof(*tmp));





}