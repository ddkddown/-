
###### 利用keepalived实现对mysql的热备

配置文件: /etc/keepalived/keepalived.conf

```
! Configuration File for keepalived

global_defs {
   notification_email {
     acassen@firewall.loc
     failover@firewall.loc
     sysadmin@firewall.loc
   }
   notification_email_from Alexandre.Cassen@firewall.loc
   smtp_server 192.168.200.1
   smtp_connect_timeout 30
   router_id mysql-1 //表示运行keepalived服务器的一个标识
   vrrp_skip_check_adv_addr
   vrrp_strict
   vrrp_garp_interval 0
   vrrp_gna_interval 0
}

vrrp_instance VI_1 {
    state MASTER // 指定keepalived角色, 分为Master和BACKUP
    interface ens33
    virtual_router_id 50
    priority 110 // 指定用于选举master的优先级，范围为1-255
    advert_int 1 // 健康检查时间间隔
    authentication { //认证区域
        auth_type PASS
        auth_pass 1111
    }
    virtual_ipaddress { //虚拟ip区域
        192.168.10.211
    }
}

virtual_server 192.168.10.211 3306 { //设置虚拟服务器，ip port
    delay_loop 6 //运行情况检查时间
    lb_algo rr //后端调度算法，此为轮询
    lb_kind NAT
    persistence_timeout 50
    protocol TCP

    real_server 192.168.10.210 3306 { //配置真实服务器节点 ip port
        weight 1
        notify_down /etc/keepalived/bin/mysql.sh //监测到服务挂后自动执行的脚本，在脚本里需要关闭keepalived，这样才会退出服务组
        TCP_CHECK {
                connect_timeout 3 //重连超时时间
                nb_get_retry 3 // 重连次数
                delay_before_retry 3 // 重连间隔时间
                connect_port 3306 // 健康检查端口
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
