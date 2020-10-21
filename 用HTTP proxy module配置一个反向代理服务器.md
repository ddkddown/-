反向代理就算用代理服务器来接受internet上的请求，然后将请求转发给内网的服务器，再将内网服务器的处理结果经过反向代理服务器发送出去，这样内网服务器就不用暴露，同时反向代理服务器还可以进行负载均衡的作用。

一般情况下，nginx作为静态web服务，具有强悍的高并发高负载的能力，但是也有一些复杂，多变的业务不适合nginx。于是nginx就会被配置为静态web服务器以及动态请求的反向代理服务器。

而同时，nginx作为反向代理服务器，不会立即将请求发送给上游服务器，而是先把用户的请求缓存在本地，然后向上游服务器发起连接，再将缓存的用户请求发送过去。

缺点很明显：延长了响应时间，增加了内存和磁盘的使用空间。

优点：降低了上游服务器的负载，尽量将压力放在了nginx服务器上。

######通常，用户端和代理服务器端是通过公网连接，代理服务器和内网服务器是通过内网连接，外网环境复杂，内网环境简单，所以一般情况下内网的平均网速比外网快。例如：如果用户端要上传1G大小的文件，如果当用户请求到达反向代理服务器时就和内网服务器建立连接，那么在接受1G文件的完整HTTP包体时，因为在外网中，速度很慢，而内网服务器在这个过程中也不得不保持这个连接，即使1G文件在内网传输很快。而Nginx在收到完整的1G文件后才向内网服务器发起请求，这时候因为在内网传输就很快，内网服务器也不用再长时间地维持连接。

##- 负载均衡的基本配置
复杂均衡是指一种策略，将请求平均地分布到每一台上游服务器上。
###1. upstream
```
语法: upstream name{...}
配置块: http

例子:
upstream backend {
    server backend1.example.com;
    server backend2.example.com;
}

server{
    location / {
        proxy_pass http://backend;
    }
}
```

###2. server
server指定了一台上游服务器的名字（域名，IP地址，unix句柄，其后可跟参数
1) weight=number 指定这台服务器的权重。
2) max_fails=number 与fail_timeout配合使用，指在fail_timeout时间段内，如果向当前的上游服务器转发失败次数超过number，则认为在当前的fail_timeout时间段内这台上游服务器不可用。max_fails默认为1，如果设置为0，则表示不检查失败次数。
3) fail_timeout=time fail_timeout表示该时间段内转发失败多少次后就认为上游服务器暂时不可用，用于优化反向代理功能。它与向上游服务器建立连接的超时时间、读取上游服务器的响应超时时间等完全无关。fail_timeout默认为10秒。
4) down 表示所在的上游服务器永久下线，只在使ip_hash配置项时才有用。
5) backup：在使用ip_hash配置项时它是无效的。它表示所在的上游服务器只是备份服务器，只有在所有的非备份上游服务器都失效后，才会向所在的上游服务器转发请求。

###3. ip_hash
在有些场景下，我们希望来自某个用户的请求始终落在固定的一台上游服务器上，例如：如果服务器需要缓存用户信息，如果用户请求不落在固定的上游服务器上，就会造成每台上游服务器都有缓存，就是资源浪费。ip_hash，是根据用户Ip计算key值， 将key值按照服务器数量取模，然后根据取模结果将请求转发到上游服务器中。如果有一个上游服务器暂时不可用，就需要down标识。
######ip_hash与weight（权重）配置不可同时使用。
```
语法: ip_hash;
配置块: upstream

例子:
upstream backend {
    ip_hash;
    server backend1.example.com;
    server backend2.example.com;
    server backend3.example.com down;
    server backend4.example.com;
}
```

##- 反向代理的基本配置
###1. proxy_pass
```
语法: proxy_pass url
配置块: location, if
```
将当前请求反向代理到url参数指定的服务器上，url可以是ip/主机名+端口，也可以是unix句柄
```
proxy_pass http://localhost:8000/uri/;

proxy_pass http://unix:/path/to/backend.socket:/uri/;

也可以直接使用upstream块
upstream backend {
     …
}
server {
    location / {
        proxy_pass http://backend;
    }
}
也可以把http转换为更安全的https
proxy_pass https://192.168.0.1;
```

###2. proxy_method
此配置项表示转发的协议方法名
```
语法: proxy_method method;
配置块: http, server, location

例子:
proxy_method POST;
```
