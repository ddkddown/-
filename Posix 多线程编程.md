######created by ddk

##Posix 多线程编程
###Posix
因为有多种unix操作系统，而这些unix操作系统上实现的系统调用名称可能会有不同，因此要编写在这些unix操作系统上可移植的程序就会比较困难。

所以POSIX标准定义了操作系统应该为应用程序提供的接口标准，是IEEE为要在各种UNIX操作系统上运行的软件而定义的一系列API标准的总称。

######简而言之: POSIX就是unix操作系统上的统一系统接口标准。

###什么是线程
####进程
进程是操作系统创建的，有着相对线程较大的内存开销,包含一些进程资源和进程执行状态的信息:
1. 进程ID, 进程组ID, 用户ID, 用户组ID
2. 环境变量
3. 工作目录
4. 程序指令
5. 寄存器
6. 栈
7. 堆
8. 文件描述符
9. 信号操作
10. 动态库
11. 进程通信IPC

####线程
线程依赖上述的进程资源，并会从其中拷贝最基础的资源以让自身作为独立运行的程序实体。线程需要包含一下信息以成为独立的控制流:
1. 栈指针
2. 寄存器
3. 调度属性(调度策略，调度优先级)
4. 挂起和屏蔽的信号组
5. 线程特定数据

简而言之, 在unix环境下，线程具有以下特性:
1. 依赖并使用进程资源。
2. 只要操作系统支持，并且依赖进程不退出，线程将拥有自己的独立控制流。
3. 为了独立运行会从进程中拷贝最基础的资源。
4. 会和同进程的其他线程一起共享进程资源
5. 进程退出，线程退出
6. 线程之所以比进程更轻量，原因在于线程不再需要包含进程的大部分资源，直接使用进程的资源即可。

但是因为同进程的线程共享进程资源，那么即会导致一些问题:
1. 线程对共享资源的修改会被其他线程访问。
2. 多个指针指向相同数据
3. 可以对共享内存区域进行读写，因此需要显式的同步消息。

###什么是Pthreads
1. 因为一些历史原因，硬件提供商各自分别实现了自身的线程版本，这些实现各有不同，对软件层面来说，创建多线程的可移植应用就会比较困难。
2. 为了充分利用线程的功能，就需要一个标准的编程接口协议。
3. POSIX就诞生了！pthread就是POSIX里的线程编程模块。
4. pthread是由c语言和一组系统调用组成，包含pthread.h头文件和一个线程库。


###为什么要用Pthreads
####轻量
1. 创建线程的开销要比创建进程的开销小很多，同时，管理线程的开销也要比管理进程开销小很多。
####高效的数据交换/通信
例如：进程间使用共享内存通信，则至少会存在一次内存拷贝的操作。但是对于线程而言，因为线程间都使用同一个进程的虚拟地址空间，就没有中间内存的拷贝，不用像进程那样有数据传输，线程间只需要简单地传递一个指针，用指针指向需要传递的数据即可。

在最差的情况下，线程间通信变成cache-CPU或者memory-CPU，即使如此，通信速度也快过进程间共享内存的通信。

###设计多线程程序
具有如下特征的程序适合使用多线程
1. 数据可以在多个控制流中使用，或者一项任务可以在多个控制流中执行
2. 可能存在长IO阻塞
3. 在某些时候存在CPU密集型操作。
######（对于这一点有个问题：对于 CPU 密集型计算，多线程本质上是提升多核 CPU 的利用率，所以对于一个 8 核的 CPU，每个核一个线程，理论上创建 8 个线程就可以了。设置了过多的线程，每个线程都想去利用 CPU 资源来执行自己的任务，这就会造成不必要的上下文切换，此时线程数的增多并没有让性能提升，反而由于线程数量过多会导致性能下降。）
4. 需要对异步事件进行响应
5. 某些任务的优先级更高

###多线程程序模型
1. manager/worker: manager作为单一线程，负责分派任务给多个工作进程。典型的例子是manager线程处理所有输入然后打包所有输出，再创建为其他任务，交给工作进程。工作进程池的创建又分为静态创建和动态创建。
2. pipeline: 一项任务被分解为多个单元，不同的单元在不同的线程上串行执行，所有的单元是并行执行的。（与manager/worker的区别在于：在m/w中，若一个任务被分为多个单元，其单元可能不是并行执行的，这取决于工作线程池和任务分配情况：如优先级等，而pipeline是保证各个单元是同时执行的，并且没有manager线程，对于优先级不同的任务，支持并不好。）
3. peer： 与m/w模型相似，唯一的区别在于manager进程也会参与任务执行工作，而不只是任务分发。

###Pthreads API
1. 线程管理：create， detach， join
2. 锁
3. 条件变量
4. 同步


#### 线程管理
- pthread_create
同一进程能创建的线程数量是有限的，若创建线程数量超过这一上限则会失败，在bash命令下可通过ulimit -a查看

######一旦创建，线程就是对等的，并且可以创建其他线程。线程之间没有隐含的层次结构或依赖关系。

线程的工作顺序根据操作系统设定的不同而不同，例如常见的有FIFO, RR（轮转时间片）, 优先级抢占等等。linux上可以通过sched_setscheduler查看策略。

pthread没有提供绑核接口，但是
- pthread_exit
线程退出有很多种方式:
1. 线程正常退出
2. 无论任务是否执行完成，只要调用pthread_exit即可退出
3. 其他线程调用pthread_cancel终止此线程
4. 进程退出/结束
######pthread_exit退出线程，线程占有的资源不会随着线程的退出而释放，而是直到进程退出才释放（如果这个线程占有了锁，没有释放那就糟糕了。）

######在main线程终止时如果调用了pthread_exit()，那么此时终止的只是main线程，而进程的资源会为其他由main线程创建的线程保持打开的状态，直到其他线程都终止。而在其他的由main线程创建的线程中pthread_exit并没有这种作用。

- join或者detach
1. 如果一个线程需要被其他线程等待着，那么最好将其创建为joinable的。
2. 如果一个线程不再被其他线程需要等待着，那么最好将其创建为detach的。(线程进行detach后，需要保证线程结束之前，访问数据的有效性。)

######现代操作系统在关闭进程时会清理该进程创建的所有线程，包括detach线程。但若进程通过调用pthread_exit退出，那么其创建的线程能够得以继续运行，因为其共享数据依然有效。


- pthread_attr_getstacksize和pthread_attr_setstacksize
用于读写线程栈的大小

- pthread_attr_getstackaddr和pthread_attr_setstackaddr。
若应用程序需要将线程栈放在内存中的某个特定的区域，可以使用这对调用。

##锁
######即使多个线程同时尝试获取锁，也只会有一个线程成功，因此，线程需要轮流获取锁。

锁的典型工作流程是:
1. 创建以及初始化锁变量
2. 多个线程尝试获取锁
3. 只有一个线程成功获取锁
4. 成功获取锁的线程执行临界区任务
5. 线程释放锁
6. 其他线程获取锁并进行4,5操作
7. 最后销毁锁

###初始化锁
1.  
```
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
```

2. 
```
调用系统调用pthread_mutex_init()
```

锁初始化后默认是unlock的

###加锁
1. pthread_mutex_lock() 若未获取锁则阻塞
2. pthread_mutex_trylock() 若未获取锁不会阻塞，立即返回
3. pthread_mutex_unlock() 释放锁

当多个线程在等待锁时，如果没有使用线程优先级设定，那么具体哪个线程会获取锁则只能看当前操作系统的调度策略，并且会显得很随机。

##条件变量
######对条件变量进行等待和发送信号

pthread_cond_wait (condition,mutex)会阻塞当前调用线程直到收到信号发送。
pthread_cond_signal (condition) 发送信号
pthread_cond_broadcast (condition) 广播信号

需要注意的几点:
1. pthread_cond_wait在调用前必须先将mutex加锁，然后在等待时会自动释放mutex
2. 在收到信号后线程被唤醒时，锁会自动被线程获取加锁，这时需要程序员在使用完后手动释放锁。

(麻烦！)

官网例子:
```
#include <pthread.h>
 #include <stdio.h>
 #include <stdlib.h>

 #define NUM_THREADS  3
 #define TCOUNT 10
 #define COUNT_LIMIT 12

 int     count = 0;
 int     thread_ids[3] = {0,1,2};
 pthread_mutex_t count_mutex;
 pthread_cond_t count_threshold_cv;

 void *inc_count(void *t) 
 {
   int i;
   long my_id = (long)t;

   for (i=0; i<TCOUNT; i++) {
     pthread_mutex_lock(&count_mutex);
     count++;

     /* 
     Check the value of count and signal waiting thread when condition is
     reached.  Note that this occurs while mutex is locked. 
     */
     if (count == COUNT_LIMIT) {
       pthread_cond_signal(&count_threshold_cv);
       printf("inc_count(): thread %ld, count = %d  Threshold reached.\n", 
              my_id, count);
       }
     printf("inc_count(): thread %ld, count = %d, unlocking mutex\n", 
	    my_id, count);
     pthread_mutex_unlock(&count_mutex);

     /* Do some "work" so threads can alternate on mutex lock */
     sleep(1);
     }
   pthread_exit(NULL);
 }

 void *watch_count(void *t) 
 {
   long my_id = (long)t;

   printf("Starting watch_count(): thread %ld\n", my_id);

   /*
   Lock mutex and wait for signal.  Note that the pthread_cond_wait 
   routine will automatically and atomically unlock mutex while it waits. 
   Also, note that if COUNT_LIMIT is reached before this routine is run by
   the waiting thread, the loop will be skipped to prevent pthread_cond_wait
   from never returning. 
   */
   pthread_mutex_lock(&count_mutex);
   while (count<COUNT_LIMIT) {
     pthread_cond_wait(&count_threshold_cv, &count_mutex);
     printf("watch_count(): thread %ld Condition signal received.\n", my_id);
     }
     count += 125;
     printf("watch_count(): thread %ld count now = %d.\n", my_id, count);
   pthread_mutex_unlock(&count_mutex);
   pthread_exit(NULL);
 }

 int main (int argc, char *argv[])
 {
   int i, rc;
   long t1=1, t2=2, t3=3;
   pthread_t threads[3];
   pthread_attr_t attr;

   /* Initialize mutex and condition variable objects */
   pthread_mutex_init(&count_mutex, NULL);
   pthread_cond_init (&count_threshold_cv, NULL);

   /* For portability, explicitly create threads in a joinable state */
   pthread_attr_init(&attr);
   pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
   pthread_create(&threads[0], &attr, watch_count, (void *)t1);
   pthread_create(&threads[1], &attr, inc_count, (void *)t2);
   pthread_create(&threads[2], &attr, inc_count, (void *)t3);

   /* Wait for all threads to complete */
   for (i=0; i<NUM_THREADS; i++) {
     pthread_join(threads[i], NULL);
   }
   printf ("Main(): Waited on %d  threads. Done.\n", NUM_THREADS);

   /* Clean up and exit */
   pthread_attr_destroy(&attr);
   pthread_mutex_destroy(&count_mutex);
   pthread_cond_destroy(&count_threshold_cv);
   pthread_exit(NULL);

 }
```

####linux多线程编程模型
##### 1. one loop per thread
简单来说，就是在线程池中创建固定数量的线程，每个线程跑一个event loop(select, poll, epoll, libevent)，这样就在连接(IO Channel)或者timer到来的时候对线程进行负载调配，也不用频繁创建或者销毁线程。
其次对于实时性有要求的或者数据量大的任务，可以单独用一个线程的event loop。其次对于IO任务还可以加上non blocking，减少阻塞时间，增加loop利用率。
