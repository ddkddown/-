# Nginx的配置
本笔记的目的： 熟悉nginx配置文件， 使用http核心模块配置web服务器，nginx反向代理。

### 块配置项
```
events {

}

http {
    upstream backend {
        server 127.0.0.1:8080;
    }

    gzip on;

    server {

    }
}
```

块配置项由一个块配置名和一对大括号组成，如上，events, http, server upstream都是块配置项。块配置项会用大括号把一系列所属的配置项全包含进来，表示配置项全部生效。并且块配置可以嵌套，内层块继承上层块（类似于全局变量和局部变量）。

### 配置项的构成部分
```
块配置名 {
    配置项名 （空格，必须）配置项值（数字，字符串, regex);
}

```

对于一个配置项，可以有多个值，值之间用空格分隔开。
注释配置项用#符号

### Nginx服务的基本配置
Nginx在运行时，至少加载几个核心模块和一个事件类模块。这些模块运行时所支持的配置项称为基本配置， 所有其他模块执行时都依赖。

####- 用于调试，定位问题的配置项
1.  是否以守护进程方式运行Nginx
```
daemon on|off （默认on）
```
守护进程是脱离终端并且在后台运行的进程，脱离终端是为了避免进程执行过程中的信息在任何终端上显示，这样进程也就不会被任何终端产生的信息打断。

2. 是否以master/worker方式工作
```
master_process on|off （默认on）
```
如果用off关闭master_process方式，就不会fork出worker子进程来处理请求，而是master进程自身来处理。

3. error日志设置
```
error_log path level
默认: error_log logs/error.log error
```
最好将path设置为一个磁盘空间足够大的位置，也可以是/dev/null,这样就不会输出任何日志了，关闭error日志的唯一手段。

4. 是否处理几个特殊的调试点
```
debug_points[stop|abort]
```
一般不用

5. 仅对指定客户端输出debug级别的日志
```
debug_connection [IP/CIDR]
```
这个配置项是事件类配置，必须放在events中才有效。
例如
```
events{
    debug_connection 10.224.66.14;
    debug_connection 10.224.57.0/24;
}
```

这样仅仅来自上诉IP地址的请求才会输出debug级别的日志。对于高并发下请求定位问题很有用。
######需要在configure时加上--with-debug参数才能生效

6. 限制coredump大小
```
worker_rlimit_core size;
```

7. 指定coredump文件生成目录
```
working_directory path
```

####- 正常运行的必备配置项
1. 定义环境变量
```
env  变量名=变量名路径

eg:

env ddk = /
```

2. 嵌入其他配置文件
```
include 配置文件路径
```

3. pid文件路径
```
pid 文件路径
默认：
pid logs/nginx.pid
```
保存master进程ID的pid文件存放路径。

4. worker进程运行的用户和用户组
```
user username groupname
默认:
user nobody nobody
```

5. worker可以打开的最大句柄描述符个数
```
worker_rlimit_nofile limit
```

6. 设置用户发往nginx的信号队列大小
```
worker_rlimit_sigpending limit
```
若某个用户的信号队列满了，那么这个用户再发送的信号量就会被丢掉。


####- 优化性能的配置项
1. worker进程个数
```
worker_process 数量
默认：
worker_process 1
```
每个worker进程都是单线程的进程，调用各个模块实现多种多样的功能。如果模块中确定不会有阻塞调用，那么有多少cpu内核就应该配置多少个进程，否则应多配置些worker进程。但若是worker进程数量多于CPU内核数量，则会增大进程间切换带来的消耗。一般情况下，需要配置和CPU内核数量相等的worker进程，并进行绑核。

2. 绑定worker进程到cpu内核
```
假如有4个核
worker_process 4;
worker_cpu_affinity 1000 0100 0010 0001;
```
如果多个worker进程抢一个CPU，那么就会出现同步问题。而如果每一个worker进程独享一个CPU，就在内核调度策略上实现了完全并发。

3. SSL硬件加速
```
ssl_engine device;
```
通过命令
```
openssl engine -t
```
可以查看服务器是否有SSL硬件加速设备，可以加快SSL协议处理速度。

4. 系统调用gettimeofday的执行频率
```
timer_resolution t
```
默认情况下，每次内核的事件调用（如epoll、select、poll、kqueue等）返回时，都会执行一次gettimeofday，实现用内核的时钟来更新Nginx中的缓存时钟。在早期的Linux内核中，gettimeofday的执行代价不小，因为中间有一次内核态到用户态的内存复制。当需要降低gettimeofday的调用频率时，可以使用timer_resolution配置。例如，“timer_resolution 100ms；”表示至少每100ms才调用一次gettimeofday。

5. worker进程优先级设置
```
worker_priority nice;
默认：
worker_priority 0;
```
优先级由静态优先级和内核根据进程执行情况所做的动态调整（目前只有±5的调整）共同决定。nice值是进程的静态优先级，它的取值范围是–20~+19，–20是最高优先级，+19是最低优先级。因此，如果用户希望Nginx占有更多的系统资源，那么可以把nice值配置得更小一些，但不建议比内核进程的nice值（通常为–5）还要小。优先级高的进程会占有更多资源。


####- 事件类配置项
1. 是否打开accept锁
```
accept_mutex on|off
默认 accept_mutex on;
```
accpet_mutex是Nginx的负载均衡锁，可以让worker进程轮流地，序列化地和新的客户端建立TCP连接。如果关闭它，那么建立TCP连接的耗时会更短，但worker进程
之间的负载会非常不均衡。
2. lock文件的路径
```
lock_file 文件位置
默认：
lock_file logs/nginx.lock
```
如果当前服务器上不支持原子锁，那么就需要文件锁来实现accept锁。

3. 使用accept锁后到连接正式建立的延迟时间
```
accpet_mutex_delay Nms
默认：
accept_mutex_delay 500ms
```
在使用accept锁后，同一时间只有一个worker进程能够取到accept锁。这个accept锁不是阻塞锁，如果取不到会立刻返回。如果有一个worker进程试图取accept锁而没有取到，它至少要等accept_mutex_delay定义的时间间隔后才能再次试图取锁。

4. 批量建立新连接
```
multi_accept on|off
默认：
multi_accept on
```
当事件模型通知有新连接时，尽可能地对本次调度中客户端发起的所有TCP请求都建立
连接。

5. 选择事件模型
```
use[kqueue|rtsig|epoll|/dev/poll|select|poll|eventport];

```
Nginx会默认选择最适合的事件模型

6. 每个worker的最大连接数
```
worker_connections number;
```

######有些配置项即使没有显式配置，也会有默认的值， 比如daemon。