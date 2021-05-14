# Sqlpp11 基础使用

## 介绍

sqlpp11 是利用c++模板特性实现的orm库，方便对数据库进行操作。支持Mysql, Sqlite3, PostgreSQL，并依赖这些数据库提供的驱动库

- 优点:上手简单，是一款轻量级框架, 相对于ODB要简单许多
- 缺点：功能不完善，例如不支持连接池，需要自己编写。



## 使用

1. sqlpp11 提供了一个python脚本：sqlpp11-ddl2cpp, 可将sql文件中的ddl描述转换为c++头文件，后续的使用中需要包含这个生成的头文件。

2. ```
   struct connection_config
       {
         typedef ::sqlpp::mysql::connection connection;
         std::string host = "localhost";
         std::string user;
         std::string password;
         std::string database;
         unsigned int port = 0;
         std::string unix_socket;
         unsigned long client_flag = 0;
         std::string charset = "utf8";
         bool auto_reconnect = true;
         bool debug = false;
   
         bool operator==(const connection_config& other) const
         {
           return (other.host == host and other.user == user and other.password == password and
                   other.database == database and other.charset == charset and other.auto_reconnect == auto_reconnect and
                   other.debug == debug);
         }
   
         bool operator!=(const connection_config& other) const
         {
           return !operator==(other);
         }
       };
   ```

sqlpp11 通过connection_config结构体来管理目标数据库的配置信息，在创建连接前，需要先指定配置信息。

3. 创建连接

   ```
   sqlpp::mysql::connection db(config);
   ```

4. 执行sql语句

   ```
   test_db::test_table test;
   	for(const auto &row : db(select(all_of(test)).from(test).unconditionally()))
   ```

   test_db是数据库中的库名，这里转换为了命名空间, test_table是目标表名。sqlpp11提供了类似sql语句一样的模板操作函数，可依此来拼接sql。在执行过程中肯定也有一个转换为sql语句字符串的过程，这也是sqlpp11与直接使用sql语句多出来的性能开销。

## 官网例子

https://github.com/rbock/sqlpp11/tree/develop/tests