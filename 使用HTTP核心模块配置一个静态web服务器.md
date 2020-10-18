静态web服务器的主要功能由ngx_http_core_module模块实现。nginx为配置一个完整的静态web服务器提供了许多功能。所有的http配置项都必须直属于http块，server块，location块，upstream块，if块......

##- 虚拟主机与请求的分发

###1. 监听端口 
由于IP地址的数量有限，因此经常存在多个主机域名对应着同一个IP地址的情况，这时在nginx.conf中就可以按照server_name（对应用户请求中的主机域名）并通过server块来定义虚拟主机，每个server块就是一个虚拟主机，它只处理与之相对应的主机域名请求。这样，一台服务器上的Nginx就能以不同的方式处理访问不同主机域名的HTTP请求了。

```
server {
    listen localhost:80;
}

语法：
listen address:port(不加端口时默认监听80端口)
配置块:server
```
如果使用ipv6地址，可以这样
```
listen [::]:8000;
listen [fe80::1];
listen [:::a8c9:1234]:80;
```
也可以加上其他参数
```
listen 443 default_server ssl;
listen 127.0.0.1 default_server accept_filter=dataready backlog=1024;
```

default， default_server: 将所在server块作为整个nginx的web服务的默认server块，如果没有设置则以nginx.conf中找到的第一个server块作为默认server块。当一个请求无法匹配配置文件的所有主机域名时，就会选用默认的虚拟主机。

backlog=num：表示TCP中backlog队列的大小。默认为–1，表示不予设置。在TCP建立三次握手过程中，进程还没有开始处理监听句柄，这时backlog队列将会放置这些新连接。可如果backlog队列已满，还有新的客户端试图通过三次握手建立TCP连接，这时客户端将会建立连接失败。

rcvbuf=size: 设置监听句柄的SO_RCVBUF(接收缓存)

sndbuf=size: 设置监听句柄的SO_SNDBUF(发送缓存)

deferred：在设置该参数后，若用户发起建立连接请求，并且完成了TCP的三次握手，内核也不会为了这次的连接调度worker进程来处理，只有用户真的发送请求数据时（内核已经在网卡中收到请求数据包），内核才会唤醒worker进程处理这个连接。这个参数适用于大并发的情况下，它减轻了worker进程的负担。当请求数据来临时，worker进程才会开始处理这个连接。只有确认上面所说的应用场景符合自己的业务需求时，才可以使用deferred配置。(开启后可能对backlog队列有影响，瞎猜的)

bind：绑定当前端口/地址对，如127.0.0.1:8000。只有同时对一个端口监听多个地址时才会生效。

ssl：在当前监听的端口上建立的连接必须基于SSL协议。

accept_filter：设置accept过滤器，只对FreeBSD操作系统有用。

###2. 主机名称
```
语法: server_name xxxxx
默认: server_name "";
配置块: server
```
在开始处理一个HTTP请求时，Nginx会取出header头中的Host，与每个server中的server_name进行匹配，以此决定到底由哪一个server块来处理这个请求。有可能一个Host与多个server块中的server_name都匹配，这时候就需要按照优先级进行选择server块。

优先级为:
1）首先选择所有字符串完全匹配的server_name，如www.testweb.com 。
2）其次选择通配符在前面的server_name，如*.testweb.com。
3）再次选择通配符在后面的server_name，如www.testweb.* 。
4）最后选择使用正则表达式才匹配的server_name，如~^\.testweb\.com$。

而如果host和server_name都不匹配，那么就会按照下列顺序进行处理:
1）优先选择在listen配置项后加入[default|default_server]的server块。
2）找到匹配listen端口的第一个server块。

######如果server_name后跟着空字符串（如server_name"";），那么表示匹配没有Host这个HTTP头部的请求。

###3. server_names_hash_bucket_size
```
语法:server_names_hash_bucket_size size;
默认:server_names_hash_bucket_size 32|64|128
配置块: http, server, location
```
为了提高快速寻找到相应server name的能力，Nginx使用散列表来存储server name。server_names_hash_bucket_size设置了每个散列桶占用的内存大小。

###4. server_names_hash_max_size
```
语法:server_names_hash_max_size size
默认:server_names_hash_max_size 512
配置块: http, server, location
```
server_names_hash_max_size会影响散列表的冲突率server_names_hash_max_size越大，消耗的内存就越多，但散列key的冲突率则会降低，检索速度也更快。server_names_hash_max_size越小，消耗的内存就越小，但散列key的冲突率可能增高。

###5. 重定向主机名称的处理
```
语法：server_name_in_redirect on|off
默认：server_name_in_redirect on;
配置块：http, server, location
```
该配置需要配合server_name使用。在使用on打开时，表示在重定向请求时会使用
server_name里配置的第一个主机名代替原先请求中的Host头部，而使用off关闭时，表示在重定向请求时使用请求本身的Host头部。

###6. location
```
location[=|~|~*|^~|@]/uri/{...}

配置块:server

```
location会尝试根据用户请求中的URI来匹配上面的/uri表达式，如果可以匹配，就选择location{}块中的配置来处理用户请求。

```
location = / {
    只有当用户请求URL是/时，才会使用该location下的配置
}
```
~ 表示匹配URL时是字母大小写敏感的。
~* 表示匹配URL时是忽略字母大小写的
^~ 表示匹配URL时只需要前半部分与URL参数匹配即可。
@表示仅用于Nginx服务内部请求之间的重定向，带有@的location不直接处理用户请求。
```
location ^~ images{
    以images开头的请求都会使用该location下的配置
}

也可以使用正则表达式匹配URL

location ~* \.(gif|jpg|jpeg)$ {
    匹配.git, .jpg, .jpeg的请求，才会使用该location下的配置
}
```

######location是有顺序的，当一个请求有可能匹配多个location时，实际上这个请求会被第一个location处理。

在最后一个location中使用/作为参数，它会匹配所有的HTTP请求，这样就可以表示如果不能匹配前面的所有location，则由“/”这个location处理。例如：
```
location / {
    / 可以匹配所有请求
}
```

##- 文件路径的定义
###1. 以root方式设置资源路径
```
语法: root path
默认: root html
配置块: http, server, location, if
```
定义资源文件相对于http请求的根目录
```
location /download/ {
    root optwebhtml
}
```
如果有一个请求的URI是/download/index/test.html，那么Web服务器将会返回服务器上optwebhtmldownload/index/test.html文件的内容。

###2. 以alias方式设置资源路径
```
语法: alias path;
配置块: location
```
alias也是用来设置文件资源路径的。如果有一个请求的URI是/conf/nginx.conf，而用户实际想访问的文件在usrlocal/nginx/conf/nginx.conf，那么想要使用alias来进行设置的话，可以采用如下方式：
```
location conf {
    alias usr/local/nginx/conf/;
}
```
如果用root来设置的话:
```
location conf {
    root usr/local/nginx/;
}
```

###3. 访问首页
```
语法： index file...
默认:  index index.html
配置块: http, server, location
```
有时访问站点时的URL是/, 一般会返回网站首页。index后面跟多个文件参数，nginx按照从右往左的顺序来访问这些文件。（如果当前文件不可访问则访问下一个）

```
location {
    root path;
    index index.html htmlindex.php /index.php
}
```
接收到请求后，Nginx首先会尝试访问path/index.php文件，如果可以访问，就直接返回文件内容结束请求，否则再试图返回pathhtmlindex.php文件的内容，依此类推。

###4. 根据HTTP返回码重定向页面
```
语法: error_page code
配置块: http, server, location, if
```
当对于某个请求返回错误码时，如果匹配上了error_page中设置的code，则重定向到新的URI中。例如：
```
error_page 404 404.html;
error_page 502 503 504 50x.html;
error_page 403 http://example.com/forbidden.html;
error_page 404 = @fetch;
```
但是这样设置后，返回的错误码还是和以前相同，用户可以重新设置错误码
```
error_page 404 =200 empty.gif;
error_page 404 =403 forbidden.gif;

也可以不指定确切的返回错误码，而是由重定向后实际处理的真实结果来决定，这时，只要把“=”后面的错误码去掉即可。

error_page 404 = /empty.gif;

如果不想修改URI，只是想让这样的请求重定向到另一个location中进行处理:

location / (
    error_page 404 @fallback;
)
location @fallback (
    proxy_pass http://backend;
)

```

###5. try_files
```
语法：try_files ......
配置块: server, location
```
try_files后要跟若干路径，如path1 path2...，而且最后必须要有uri参数，意义如下：尝试按照顺序访问每一个path，如果可以有效地读取，就直接向用户返回这个path对应的文件结束请求，否则继续向下访问。如果所有的path都找不到有效的文件，就重定向到最后的参数uri上。因此，最后这个参数uri必须存在，而且它应该是可以有效重定向的。例如：

```
try_files systemmaintenance.html $uri $uri/index.html $uri.html @other;
location @other {
    proxy_pass http://backend;
}
```

##- 内存及磁盘资源的分配
###1. connection_pool_size
```
语法: connection_pool_size size
默认: connection_pool_size 256
配置块: http, server 
``` 
Nginx对于每个建立成功的TCP连接会预先分配一个内存池，上面的size配置项将指定这个内存池的初始大小。

###2. request_pool_size
```
语法： request_pool_size size;
默认: request_pool_size 4k;
配置块: http, server
```
nginx开始处理http请求时，会为每个请求分配一个内存池，size配置项指定这个内存池的初始大小，用于减少内核对小块内存的分配次数。

######TCP连接关闭时会销毁connection_pool_size指定的连接内存池，HTTP请求结束时会销毁request_pool_size指定的HTTP请求内存池，但它们的创建、销毁时间并不一致，因为一个TCP连接可能被复用于多个HTTP请求。

##- 网络连接的设置
###1. 对某些浏览器禁用keepalive功能
```
语法： keepalive_disable 浏览器
默认: keepalive_disable safari
配置块: http, server, location
```
HTTP请求中的keepalive功能是为了让多个请求复用一个HTTP长连接，这个功能对服务器的性能提高是很有帮助的。但有些浏览器，如IE 6和Safari，它们对于使用keepalive功能的POST请求处理有功能性问题。

###2. keepalive超时时间
```
语法: keepalive_timeout time
默认: keepalive_timeout 75;
配置块: http, server, location
```
一个keepalive连接在闲置超过一定时间后（默认的是75秒），服务器和浏览器都会去关闭这个连接。

###3. keepalive长连接上允许承载的最大请求数
```
语法: keepalive_requests n;
默认: keepalive_request 100;
配置块: http, server, location
```
一个keepalive连接上默认最多发送100个请求

##- MIME类型的设置
```
语法： type{......}
配置块: http, server, location
```
定义mime type到文件扩展名的映射， 多个扩展名可以映射到同一个mime type
```
types {
    text/html html;
    text/html conf;
    image/gif gif;
    image/jpeg jpg;
}

default_type mime-type
例如：default_type text/plain
配置块: http, server, location
```
当找不到相应的MIME type与文件扩展名之间的映射时，使用默认的MIME type作为HTTP header中的Content-Type。

##- 对客户端请求的限制


##- 文件操作的优化
###1. sendfile系统调用
```
语法: sendfile on|off
默认： sendfile off;
配置块: http, server, location
```

可以启用Linux上的sendfile系统调用来发送文件，它减少了内核态与用户态之间的两次内存复制，这样就会从磁盘中读取文件后直接在内核态发送到网卡设备，提高了发送文件的效率。


