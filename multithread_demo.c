//
// Created by onceme on 2019/3/19.
//


#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

#include <time.h>
#include <signal.h>
#include <sys/wait.h>
#include <errno.h>
#include <assert.h>
#include <ctype.h>
#include <stdarg.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/uio.h>
#include <limits.h>
#include <float.h>
#include <math.h>
#include <sys/resource.h>
#include <sys/utsname.h>
#include <locale.h>
#include <unistd.h>
int sum = 0;


void dolog(const char *fmt, ...){
    va_list ap;
    char msg[10000];

    va_start(ap, fmt);
    vsnprintf(msg, sizeof(msg), fmt, ap);
    va_end(ap);
}

void* adder(void *p)
{
//    char *fmt = 'onceme %d';

    int old = sum;
    for(int i = 0; i < 1000000; i++)  // 百万次
    {
//        while(!__sync_bool_compare_and_swap(&sum, old, old + 1))  // 如果old等于sum, 就把old+1写入sum
//        {
            old = sum; // 更新old
            printf("onceme %d",i);

//        }
    }

    return NULL;
}


int main()
{
    pthread_t threads[100];
    for(int i = 0;i < 100; i++)
    {
        pthread_create(&threads[i], NULL, adder, NULL);
    }

    for(int i = 0; i < 100; i++)
    {
        pthread_join(threads[i], NULL);
    }

    printf("sum is %d\n",sum);
}
