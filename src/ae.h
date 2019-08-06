/* A simple event-driven programming library. Originally I wrote this code
 * for the Jim's event-loop (Jim is a Tcl interpreter) but later translated
 * it in form of a library for easy reuse.
 *
 * Copyright (c) 2006-2012, Salvatore Sanfilippo <antirez at gmail dot com>
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

#ifndef __AE_H__
#define __AE_H__

/* 事件执行结果 */
#define AE_OK 0                                                 // 成功
#define AE_ERR -1                                               // 出错

/* 文件事件状态 */
#define AE_NONE 0                                               // 未设置
#define AE_READABLE 1                                           // 可读
#define AE_WRITABLE 2                                           // 可写

/* 事件处理器的执行 flags */
#define AE_FILE_EVENTS 1                                        // 文件事件
#define AE_TIME_EVENTS 2                                        // 时间事件
#define AE_ALL_EVENTS (AE_FILE_EVENTS|AE_TIME_EVENTS)           // 所有事件
#define AE_DONT_WAIT 4                                          // 不阻塞，也不进行等待

/* 决定时间事件是否要持续执行的 flag */
#define AE_NOMORE -1

/* Macros */
#define AE_NOTUSED(V) ((void) V)

/* 事件处理器状态 */
struct aeEventLoop;

/* 事件接口 */
typedef void aeFileProc(struct aeEventLoop *eventLoop, int fd, void *clientData, int mask);     // 文件事件处理
typedef int aeTimeProc(struct aeEventLoop *eventLoop, long long id, void *clientData);          // 时间事件处理
typedef void aeEventFinalizerProc(struct aeEventLoop *eventLoop, void *clientData);             //
typedef void aeBeforeSleepProc(struct aeEventLoop *eventLoop);                                  // 处理事件前执行的函数

/* 文件事件结构 */
typedef struct aeFileEvent {
    int mask;                                       // 监听事件类型掩码，值可以是 AE_READABLE 或 AE_WRITABLE 或 AE_READABLE | AE_WRITABLE
    aeFileProc *rfileProc;                          // 读事件处理器(函数指针)
    aeFileProc *wfileProc;                          // 写事件处理器(函数指针)
    void *clientData;                               // 多路复用库的私有数据
} aeFileEvent;

/* 时间事件结构 */
typedef struct aeTimeEvent {
    long long id;                                   // 时间事件的唯一标识符
    long when_sec;                                  // 事件的到达时间 秒
    long when_ms;                                   // 事件的到达时间 毫秒（秒表示之后剩余的毫秒）
    aeTimeProc *timeProc;                           // 事件处理函数
    aeEventFinalizerProc *finalizerProc;            // 事件释放函数
    void *clientData;                               // 多路复用库的私有数据
    struct aeTimeEvent *next;                       // 指向下个时间事件结构，形成链表
} aeTimeEvent;

/* 已就绪事件 */
typedef struct aeFiredEvent {
    int fd;                                         // 已就绪文件描述符
    int mask;                                       // 事件类型掩码，同上述 文件事件的掩码
} aeFiredEvent;

/* 事件处理器的状态 */
typedef struct aeEventLoop {
    int maxfd;                                      // 目前已注册监听的最大描述符
    int setsize;                                    // 可追踪的最大描述符
    long long timeEventNextId;                      // 用于生成时间事件 id
    time_t lastTime;                                // 最后一次执行时间事件的时间
    aeFileEvent *events;                            // 已注册的文件事件
    aeFiredEvent *fired;                            // 已就绪的文件事件
    aeTimeEvent *timeEventHead;                     // 时间事件
    int stop;                                       // 事件处理器的开关
    void *apidata;                                  // 多路复用库的私有数据
    aeBeforeSleepProc *beforesleep;                 // 在处理事件前要执行的函数
} aeEventLoop;


/**
 * 初始化事件处理器
 *
 * @param setsize：最大处理事件数（事件槽的数量）
 * @return 成功： eventloop
 *         失败： NULL
 */
aeEventLoop *aeCreateEventLoop(int setsize);

/**
 * 事件处理器释放
 *
 * @param eventLoop：要释放的事件处理器
 */
void aeDeleteEventLoop(aeEventLoop *eventLoop);

/**
 * 设置标识，停止事件处理
 *
 * @param eventLoop：要停止的事件处理器
 */
void aeStop(aeEventLoop *eventLoop);

/**
 * 添加文件监听事件
 *
 * @param eventLoop：事件处理器
 * @param fd：服务端针对客户端分配的 fd
 * @param mask：具体监听事件
 * @param proc：事件发生后的具体操作
 * @param clientData：客户端私有数据
 *
 * @return 成功：AE_OK
 *         失败：AE_ERR
 */
int aeCreateFileEvent(aeEventLoop *eventLoop, int fd, int mask, aeFileProc *proc, void *clientData);

/**
 * 删除文件监听事件
 *
 * @param eventLoop：事件处理器
 * @param fd：要设置的文件描述符
 * @param mask：要删除的事件
 */
void aeDeleteFileEvent(aeEventLoop *eventLoop, int fd, int mask);

/**
 * 获取给定 fd 正在监听的类型
 *
 * @param eventLoop：事件处理器
 * @param fd：要设置的文件描述符
 * @return 成功：返回 fd 正监听的掩码
 *         失败：0
 */
int aeGetFileEvents(aeEventLoop *eventLoop, int fd);

/**
 * 创建时间事件
 *
 * @param eventLoop：事件处理器
 * @param milliseconds：此后多长时间后处理
 * @param proc：事件处理函数
 * @param clientData：客户端数据
 * @param finalizerProc：时间事件删除时的清理操作
 * @return 成功：返回事件 id
 *         失败：AE_ERR
 */
long long aeCreateTimeEvent(aeEventLoop *eventLoop, long long milliseconds, aeTimeProc *proc, void *clientData, aeEventFinalizerProc *finalizerProc);

/**
 * 删除给定 id 的时间事件
 *
 * @param eventLoop：时间事件处理器
 * @param id：时间事件id
 * @return 成功：AE_OK
 *         失败：AE_ERR
 */
int aeDeleteTimeEvent(aeEventLoop *eventLoop, long long id);

/**
 * 处理事件（文件事件 和 时间事件）
 *
 * @param eventLoop：事件处理器
 * @param flags：flags == 0 函数直接返回
 *               flags = AE_ALL_EVENTS 所有类型的事件都会被处理
 *               flags = AE_FILE_EVENTS 处理文件事件
 *               flags = AE_TIME_EVENTS 处理时间事件
 *               flags = AE_DONT_WAIT 处理完所有不许阻塞的事件后，立刻返回
 *               flags 非以上选项，则会阻塞，知道文件事件就绪
 * @return 返回已处理事件的个数
 */
int aeProcessEvents(aeEventLoop *eventLoop, int flags);

/**
 * 在给定毫秒内等待，知道 fd 变成 可读 或 可写 或 异常（调用了poll）
 *
 * @param fd：监听的fd
 * @param mask：事件掩码（AE_READABLE读  AE_WRITABLE写）
 * @return 成功：有一个事件发生，并返回 flags
 *         失败：-1表示poll失败、0表示时间到，没有事件发生、大于1表示多于1个事件发生
 */
int aeWait(int fd, int mask, long long milliseconds);

/**
 * 事件处理器的主循环
 *
 * @param eventLoop：事件处理器
 * @return void
 */
void aeMain(aeEventLoop *eventLoop);

/**
 * 返回所使用多路复用的名称
 *
 * @return 返回：epoll, poll, select ....
 */
char *aeGetApiName(void);

/**
 * 设置处理事件前需要被执行的函数
 *
 * @param eventLoop：事件处理器
 * @param beforesleep：事件处理需要执行的函数
 */
void aeSetBeforeSleepProc(aeEventLoop *eventLoop, aeBeforeSleepProc *beforesleep);

/**
 * 返回当前事件槽的大小
 *
 * @param eventLoop：事件处理器
 * @return 返回事件槽的大小
 */
int aeGetSetSize(aeEventLoop *eventLoop);

/**
 * 调整事件槽的大小
 * 如果尝试调整大小 >= 现有文件描述符，那么返回 AE_ERR， 不执行任何操作
 * 否则返回 AE_OK
 *
 * @param eventLoop：事件处理器
 * @param setsize：要调整的大小
 * @return 失败：AE_ERR
 *         成功：AE_OK
 */
int aeResizeSetSize(aeEventLoop *eventLoop, int setsize);

#endif
