nginx处理用户请求流程的过程是：
1. worker进程在一个for循环中反复调用事件模块检测网络事件。
2. 接受到客户端发来的SYN包后，即可建立tcp连接。
3. 建立起连接后根据nginx.conf文件中配置会交给HTTP框架处理。
4. HTTP框架会试图接受完整的HTTP头部，接受到完整的HTTP头部后将请求分发到具体的HTTP模块中处理。
5. 分发策略是多样化的，最常见的是根据请求的URI和nginx.conf里location配置项的匹配度来决定如何分发。
6. HTTP模块处理完请求后，大多数会向客户端发送响应，此时会依次调用所有的HTTP过滤模块。每个过滤模块根据配置文件决定自己的行为。例如：gzip过滤模块根据配置文件的 gzip on|off 来决定是否压缩响应。

######nginx模块需要有自己的名字，按照nginx命名规则, 命名为:ngx_http_ddk_module。

##Nginx的几个基本数据结构和方法
###整数的封装
nginx使用ngx_int_t 封装有符号整形，使用ngx_uint_t封装无符号整形。
```
typedef intptr_t ngx_int_t;
typedef uintptr_t ngx_uint_t;
```

###ngx_str_t数据结构
ngx_str_t结构就是对字符串的封装
```
typedef struct {
    size_t  len;
    u_char  *data;
} ngx_str_t;
```
######注意：data成员指向的字符串可能不以'\0'作为结尾，所以需要len成员来表明长度。

ngx_str_t可以有效地降低内存使用量，比如存储字符串“GET”到0x1d0b0110上，只需要设置{len=3,data=0x1d0b0110}，就不需要再为method_name分配内存冗余的存储字符串。

###ngx_list_t数据结构
ngx_list_t是Nginx封装的链表容器。
```
typedef struct ngx_list_part_s ngx_list_part_t;
struct ngx_list_part_s {
    void *elts; //指向数组的起始位置
    ngx_uint_t nelts; //表示数组中已经使用了多少个元素
    ngx_list_part_t *next; //下个链表元素的地址
};

typedef struct {
    ngx_list_part_t *last; //指向链表的最后一个元素
    ngx_list_part_t part;//链表的第一个元素
    size_t size;//数组中存储的每一个数组元素的大小
    ngx_uint_t nalloc;//链表元素中数组的容量大小
    ngx_pool_t *pool;//链表中管理内存分配的内存池对象
} ngx_list_t;

```

每个链表元素ngx_list_part_t中的elts指向的是一个数组，有连续的内存。而ngx_list_t就是一个存储数组的链表。
######因为小块内存分配地址不连续使用链表访问的效率比较低下，而数组是连续的，通过偏移量来直接访问内存效率要高出很多。

######如果全部用数组来存，就不好确定首先要声明的数组空间大小，若是空间申请多了则浪费，若是申请少了之后的拷贝操作又太费时。

#####nginx这种设计可以即包含了数组的快速访问功能又包含了链表的动态申请空间的功能。

###ngx_table_elt_t
ngx_table_elt_t就是一个key/value对。用于处理HTTP头部信息, key存储头部名称，value存储值，lowcase_key是为了忽略HTTP头部的大小写, hash用于快速检索头部。
```
typedef struct {
    ngx_uint_t  hash;
    ngx_str_t   key;
    ngx_str_t   value;
    u_char  *lowcase_key; //指向全小写的key字符串
} ngx_table_elt_t;
```

###ngx_buf_t
ngx_buf_t是nginx用于处理大数据的关键数据结构。用于内存数据和磁盘数据。
```
typedef struct ngx_buf_s ngx_buf_t;
typedef void* ngx_buf_tag_t;
struct ngx_buf_s {
    u_char *pos //pos一般用于表明从这个位置来开始处理内存数据，因为一个ngx_buf_s可能会被多次反复处理。
    u_char *last //last表明数据的结尾，pos和last之间的内存是希望nginx处理的内容。
    off_t file_pos; //表明处理文件的起始位置。
    off_t file_last; //表明处理文件的截止位置。

    //如果ngx_buf_t用于内存缓存，那么用start和end来表示位置
    u_char *start;
    u_char *end;

    ......
}
```

###ngx_chain_t数据结构
ngx_chain_t是和ngx_buf_t配合使用的链表数据结构
```
typedef struct ngx_chain_s ngx_chain_t;
struct ngx_chain_s {
    ngx_buf_t *buf;
    ngx_chain_t *next;
}
```

##将自己的HTTP模块编译进Nginx
1. 将源代码放到一个目录下
2. 在目录中编写一个config文件通知nginx如何编译本模块
3. 在configure脚本中加入--add-module=PATH(模块路径)

### - config文件的写法
config文件只是一个shell脚本文件，开发HTTP模块，需要在config文件中定义3个变量:
1. ngx_addon_name:设置为模块名称
2. HTTP_MODULES:所有的HTTP模块名称，每个HTTP模块间用空格相连。
3. NGX_ADDON_SRCS:用于指定新增模块的源代码，多个源代码可以用空格相联。

例如:
```
ngx_addon_name=ngx_http_mytest_module
HTTP_MODULES="$HTTP_MODULES ngx_http_mytest_module"
NGX_ADDON_SRCS="$NGX_ADDON_SRCS $ngx_addon_dir/ngx_http_mytest_module.c"
```

## HTTP模块的数据结构
