//
// Created by onceme on 2019/3/15.
//

// 从调用链角度来理解代码

#include "redis.c";
#include "ae.c";
#include "networking.c";
#include "ae_epoll.c";
#include "db.c";
#include "adlist.c";
#include "replication.c";
#include "sds.c";
#include "rdb.c";
#include "zmalloc.c";
#include "redis-benchmark.c";
#include "t_string.c";
#include "pubsub.c";
#include "aof.c";

main(1);                                    //入口函数
initServer(1);                             //初始化服务
listenToPort(1);
aeCreateFileEvent(1);                        //事件触发绑定
aeMain(1);                                   //主事件循环
aeProcessEvents(1);                           //开始处理事件
acceptTcpHandler(1);acceptUnixHandler(1);     //创建tcp/本地 连接
acceptCommonHandler(1);                    //TCP 连接 accept 处理器
createClient(1);                           //创建一个新客户端
readQueryFromClient(1);                       //读取客户端的查询缓冲区内容
    sdsMakeRoomFor(1);
processInputBuffer(1);                        //处理客户端输入的命令内容
processCommand(1);
    call(1);
    getGenericCommand(1);
    getCommand(1);
    addReplyBulk(1);
    lookupKeyReadOrReply(1);
    resetClient(c);
addReply(1);
prepareClientToWrite(1);                       //将客户端连接描述符的写事件，绑定到指定的事件循环中
sendReplyToClient(1);
resetClient(c);

//跳转导航
aeApiAddEvent(1);

aeApiPoll(1);

server.el = aeCreateEventLoop(server.maxclients+REDIS_EVENTLOOP_FDSET_INCR);    //创建epoll事件

redisLog(1);
selectDb(1);
getTcpConnfd(1);
listCreate(1);
listAddNodeTail(1);
listDelNode(1);
listPop(1);
listFirst(1);
addReply(1);
addReplySds(1);
addReplyErrorFormat(1);
sdsempty(1);

REDIS_NOTUSED(privdata);


//主线程定时任务
serverCron(1);              //加入到时间事件
aeMain(1);                  //主循环
aeProcessEvents(1);         //循环处理事件
processTimeEvents(1);       //处理所有已到达的时间事件
clientsCron(1);
clientsCronResizeQueryBuffer(1);
    sdsRemoveFreeSpace(1);
databasesCron(1);
//主从同步逻辑
//主库逻辑
    //定时任务
    serverCron(1);
    backgroundSaveDoneHandler(1);
    updateSlavesWaitingBgsave('a');   //在每次 BGSAVE 执行完毕之后使用
    sendBulkToSlave(1); //master 将 RDB 文件发送给 slave 的写事件处理器(slave 是由reactor线程接受的，所以不能用server.el)

    slave->repldbfd = open(server.rdb_filename,O_RDONLY)    //打开rdb文件


    replicationCron(1);


    //命令执行后
    call(1);
    propagate(1);   //将指定命令传播到aof和slave
    propagateExpire(1);  //过期键传播
    replicationFeedSlaves(1);   //命令传播给从服务器
    addReplyMultiBulkLen(1);
    addReplyBulkDelayEvent(1);
    addReplyBulk(1);
    //主库接受从库请求
    //主库是把从库的请求当做一个普通命令来处理的，所以会经过reactor线程
    //sync psync命令绑定的函数
    syncCommand(1);         //部分同步与全部同步

    //尝试进行部分 resync ，成功返回 REDIS_OK ，失败返回 REDIS_ERR 。
    masterTryPartialResynchronization(1);        //    listAddNodeTail(server.slaves,c);//将client添加到server.slaves 的逻辑



//replication.c:907 行之后是slave的方法，里面使用server.el作为事件驱动
//从库逻辑   从库与主库的同步操作是在从库主线程发起的，对事件驱动器的一系列操作都是在server.el上，所以相关代码不需要调整为c->reactor_el
    initServer(1);   //file:redis.c:2158
    serverCron(1);       //file:redis.c:1327   Redis 的时间中断器，每秒调用 server.hz 次
    //file:redis.c:1542    run_with_period(1000) replicationCron();
    replicationCron(1);          //定时复制函数，每秒调用一次
    connectWithMaster(1);   //以非阻塞方式连接主服务器
    syncWithMaster(1);  //监听主服务器 fd 的读和写事件，并绑定文件事件处理器
    replicationResurrectCachedMaster(1);    //接收master节点的命令传播
    readSyncBulkPayload(1); //异步 RDB 文件读取函数
    addReply(1);
    prepareClientToWrite(1);

//跳转导航
replicationScriptCacheInit(1);
rdbSaveBackground(1);
sdsIncrLen(1);
freeClient(1);
addReplyBulk(1);
sdsIncrLen(1);
redisLog(1);
sdsRemoveFreeSpace(1);
sdsMakeRoomFor(1);
sdsAllocSize(1);
zrealloc(1);
zfree(1);
bgrewriteaofCommand(1);
bgsaveCommand(1);
rewriteAppendOnlyFileBackground(1);
pubsubUnsubscribeAllChannels(1);
databasesCron(1);


//redis-benchmark 压测逻辑
benchmark("GET",cmd,len);   //压测某个命令
c = createClient(cmd,len,NULL); //创建第一个客户端
createMissingClients(c);    //批量创建client
aeMain(config.el);  //事件循环
aeProcessEvents(1);
writeHandler(1);    //可写事件回调函数，向服务端发送命令

readHandler(1); //可读事件回调函数，接收服务端返回

//子进程问题
//fork进程，是否会把线程一起fork了


