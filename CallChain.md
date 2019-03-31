###### Redis的各个逻辑链
&emsp;&emsp;一直觉得接触一个大项目需要抓好切入点(一个是提高效率，一个是容易建立正反馈)，虽然代码可读性一直都是大家的追求，不过程序语言到底还是处在人的语言和机器语言中间，通读代码来理出逻辑肯定不是一个高效的办法。
&emsp;&emsp;基于之前的实践，感觉从逻辑链入手，是一个比较好的办法，一开始不要把自己困在一个个逻辑实现的细节总，人大脑的并行能力其实很有限，不同层次的逻辑混在一起看，难度就大了很多

    + 从最粗粒度的逻辑开始梳理，对不在这一层次的逻辑予以忽略
        + e.g 一个网络服务，必然有监听端口，接受连接，以及回应请求的部分
    + 在这基础上一层层扩充细节
    + 具体实践来说，就是梳理好一个个逻辑调用链
        + 最主要的客户端请求的调用链
        + 主从同步相关调用链
        + 定时任务调用链
    + C语言有个特点就是出错了有效报错信息特别稀少(即使是用上GDB调试)，所以找不到原因的时候，一个是尽量在调用流程上输出日志，另外一个就是静下来梳理好自己代码改动的地方，看是否有逻辑问题
    + redis本身使用了异步非阻塞的编程模型，在这基础上加上多个reactor线程，以及一个worker线程，这就会造成很多共享变量存在被多个线程同时操作的问题，C语言下不好调试加上并发安全问题碰到错误，错误信息也没有给出太大的提示，所以调试起来还是挺困难的，经常都是尽量在相关调用链上输出日志，得到一点提示，然后在重复的梳理逻辑，推断导致异常的原因，这对于一个在动态类型语言上习惯了有详尽错入信息的人来说，思想观念还是需要转变，尤其是在写设计并发安全的程序时，提现梳理好逻辑，考虑到哪些点会触发线程安全问题还是很有必要的


* 一次Redis请求的执行流程
```
main();                                    //入口函数
initServer();                             //初始化服务
listenToPort();                           //端口监听
aeCreateFileEvent();                        //事件触发绑定
aeMain();                                   //主事件循环
aeProcessEvents();                           //开始处理事件
acceptTcpHandler();acceptUnixHandler();     //创建tcp/本地 连接
acceptCommonHandler();                    //TCP 连接 accept 处理器
createClient();                           //创建一个新客户端
dispatch2Reactor();                         //分配到指定reactor线程
readQueryFromClient();                    //读取客户端的查询缓冲区内容
processInputBuffer();                        //处理客户端输入的命令内容
processCommand();                         //执行命令
    call();
addReply();                               //将客户端连接描述符的写事件，绑定到指定的事件循环中
sendReplyToClient();                      //reactor线程中将内容输出给客户端
resetClient(c);
```
* 主从同步的调用链
```
//主从同步逻辑
//主库逻辑
    //定时任务
      serverCron(1);
      backgroundSaveDoneHandler(1);
      updateSlavesWaitingBgsave('a');   //在每次 BGSAVE 执行完毕之后使用
      sendBulkToSlave(1); //master 将 RDB 文件发送给 slave 的写事件处理器(slave 是由reactor线程接受的，所以不能用server.el)
      slave->repldbfd = open(server.rdb_filename,O_RDONLY)    //打开rdb文件
      replicationCron(1);

    //客户端命令执行后
      call(1);
      propagate(1);   //将指定命令传播到aof和slave
      propagateExpire(1);  //过期键传播
      replicationFeedSlaves(1);   //命令传播给从服务器
      addReplyMultiBulkLen(1);
      addReplyBulkDelayEvent(1);  //将命令缓冲输出到从节点
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
```
* 定时任务调用链
```
//主线程定时任务
serverCron(1);              //加入到时间事件
aeMain(1);                  //主循环
aeProcessEvents(1);         //循环处理事件
processTimeEvents(1);       //处理所有已到达的时间事件
databasesCron();            //对数据库操作
clientsCron(1);             //关闭超时客户端，
    clientsCronResizeQueryBuffer(1);    //回收c->querybuf空闲空间
    sdsRemoveFreeSpace(1);
```


###### 对不同平台网络IO事件驱动模型的封装
ae类库
###### 时间事件