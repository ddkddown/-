# Mysql备份

- 负载均衡

- 高可用性

- 故障切换

  ### 复制的过程

  1. 主库将数据记录的更改记录在二进制日志中
  2. 备库通过网络IO将主库的二进制日志复制到自身的中继日志(relay_log)中
  3. 备库通过中继日志将更改记录放到备库数据库中



### 配置复制

1. #### 创建复制账号

备库是通过一个IO线程上创建tcp连接来连接主库的，所以需要在主库上为该备库创建相应的用户并赋予相应的权限，语句为: grant replication slave, replication client on \*.\* to user@'ip地址' identified by 'password'; flush privileges;

2. #### 同步主从数据库

   mysqldump

3. #### 配置主库和备库

首先打开主库的二进制日志和指定一个唯一的服务器id，可通过在my.cnf文件中指定:

```
log_bin = mysql-bin
server_id = 10
binlog-do-db = db名称
```

服务器id一般的通用做法是取IP地址的末8位，并且保证其是唯一不变的，并且具有一定意义。

同样的，备库中也需要添加类似的配置

```##
log_bin = mysql-bin
server_id = 2
relay_log = /var/lib/mysql/mysql-relay-bin (中继日志的位置和命名)
read_only = 1
```

   #### 开始同步

```
show master status;
change master to master_host='主服务器ip', master_user='主服务器用户名', master_password='主服务器密码', master_log_file='主服务器同步文件名', master_log_pos=同步文件起始位;
start slave;
show slave status;
```

参考博文：https://www.jianshu.com/p/b0cf461451fb
