# VRRP 

### 简介

为了避免路由器出现单点故障的一种容错协议。**多个运行着VRRP协议的路由器抽象成一个虚拟路由器**（从外边来看，就像只有一个真实的路由器在工作），组成虚拟路由器的一组路由器会有一个会成为**Master路由器**，其余的会成为**Backup路由器**。正常情况下，**会由Master完成该虚拟路由器的工作**。Master一旦出现故障，从Backup中选出一个成为Master继续工作，从而避免路由器单点问题。

- vip: 虚拟IP，是一个不与特定计算机或网络接口卡(NIC)相连的IP地址。
- priority:**用来标识虚拟路由器中各成员路由器的优先级**，虚拟路由器根据优先级选举出Master和Backup。

### 工作流程

1. 从vrrp组中选举出一个master，其余为backup。
2. master负责工作，backup负责监听master状态。
3. master出现故障，回到1

## KeepAlived

### 安装

yum / 官网



### 配置实例

配置文件: /etc/keepalived/keepalived.conf

```
! Configuration File for keepalived

global_defs {
   notification_email { //通知邮箱地址
     123456@qq.com
   }
   notification_email_from keepalived@msun.com //邮箱发送地址
   smtp_server 192.168.200.1 //用于发送邮件的smtp服务器
   smtp_connect_timeout 30 //smtp服务器连接超时时间
   router_id mysql-1 // 对当前允许keepalived机器的一个标识
}

vrrp_instance VI_1 { //在一个网卡上关联一个vrrp实例
    state MASTER // 指定keepalived角色, 分为Master和BACKUP
    interface ens33
    virtual_router_id 50 //用于区分子网内vrrp组的多个实例
    priority 255 // master的优先级，如果为255且state为master，则该节点立即成为master，否则之后进行选举
    advert_int 1 // 主备间检查消息间隔时间
    virtual_ipaddress { //虚拟ip区域
        192.168.10.211
    }
}

virtual_server 192.168.10.211 3306 { //设置虚拟服务器，ip port
    protocol TCP

    real_server 192.168.10.210 3306 { //配置真实服务器节点 ip port
        notify_down /etc/keepalived/bin/mysql.sh //监测到服务挂后自动执行的脚本，在脚本里需要关闭keepalived，这样才会退出服务组
        TCP_CHECK {
        }
    }
}
```

/etc/keepalived/bin/mysql.sh

```
#! /bin/bash
pkill keepalived
/sbin/ifdown ens33 && /sbin/ifup ens33
```
