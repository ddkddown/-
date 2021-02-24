# sql语句优化

## Explain
- select_type 
- partitions
- type 单表访问方式
- possible_keys
- key
- key_len
- ref
- rows
- filtered
- extra

### select_type
1. SIMPLE: 不包含union或子查询的select
2. PRIMARY: 对于包含UNION、UNION ALL或者子查询的大查询来说，它是由几个小查询组成的，其中最左边的那个查询的select_type值就是PRIMARY
3. UNION: 对于包含UNION或者UNION ALL的大查询来说，它是由几个小查询组成的，其中除了最左边的那个小查询以外，其余的小查询的select_type值就是UNION
4. UNION RESULT: MySQL选择使用临时表来完成UNION查询的去重工作，针对该临时表的查询的select_type就是UNION RESULT。union生成的去重临时表会是此type，union all 不用去重，不会生成临时表，也就没有此type了。
5. SUBQUERY: 如果包含子查询的查询语句不能够转为对应的semi-join的形式，并且该子查询是不相关子查询，并且查询优化器决定采用将该子查询物化的方案来执行该子查询时，该子查询的第一个SELECT关键字代表的那个查询的select_type就是SUBQUERY。
    ###### 半连接(semi join) 对于一些包含子查询(ex: IN)的sql,优化器会将其转变为半连接，半连接只要匹配到一条记录时就返回，不像其他连接可能返回多条记录。
6. DEPENDENT SUBQUERY: 外表的每一行都对子查询进行调用 
   ###### 如果包含子查询的查询语句不能够转为对应的semi-join的形式，并且该子查询是相关子查询，则该子查询的第一个SELECT关键字代表的那个查询的select_type就是DEPENDENT SUBQUERY。
7. DEPENDENT UNION: 在包含UNION或者UNION ALL的大查询中，如果各个小查询都依赖于外层查询的话，那除了最左边的那个小查询之外，其余的小查询的select_type的值就是DEPENDENT UNION。
8. DERIVED:
   ###### 例子: SELECT * FROM (SELECT key1, count(*) as c FROM s1 GROUP BY key1) AS derived_s1 where c > 1; 子查询被显示物化为了表derived_s1，select_type即为DERIVED。
   先组成derived_s1临时表。
   然后对临时表做where c > 1 的筛查。
   缺点在于组成临时表时没有充分利用外部的筛查条件，将多余记录组在了临时表中。

9.  MATERIALIZED: 当查询优化器在执行包含子查询的语句时，选择将子查询物化之后与外层查询进行连接查询时，该子查询对应的select_type属性就是MATERIALIZED。
    ###### its results will be stored in a temporary table with a unique key over all columns. The unique key is there to prevent the table from containing any duplicate records.

    ###### Subquery materialization using a temporary table avoids such rewrites and makes it possible to execute the subquery only once rather than once per row of the outer query.

    ###### Compared to query execution that does not use materialization, select_type may change from DEPENDENT SUBQUERY to SUBQUERY. This indicates that, for a subquery that would be executed once per outer row, materialization enables the subquery to be executed just once

    ###### MATERIALIZED是semi-join的实现策略中的一个子集

### type
const: 用主键来定位一条数据
ref: 用二级索引定位一条数据(不包含null)
ref_or_null：用二级索引定位一条数据(包含null)
range: 用索引进行范围查找
index: 遍历二级索引
all: 遍历全表
unique_subquery: 查询优化器决定将IN子查询转换为EXISTS子查询，而且子查询可以使用到主键进行等值匹配的话，那么该子查询执行计划的type列的值就是unique_subquery
index_subquery: index_subquery与unique_subquery类似，只不过访问子查询中的表时使用的是普通的索引
### possible_keys
可能会用到的索引

### key
实际用到的索引

### key_len
key_len列表示当优化器决定使用某个索引执行查询时，该索引记录的最大长度，它是由这三个部分构成的：

- 对于使用固定长度类型的索引列来说，它实际占用的存储空间的最大长度就是该固定值，对于指定字符集的变长类型的索引列来说，比如某个索引列的类型是VARCHAR(100)，使用的字符集是utf8，那么该列实际占用的最大存储空间就是100 × 3 = 300个字节。

- 如果该索引列可以存储NULL值，则key_len比不可以存储NULL值时多1个字节。

- 对于变长字段来说，都会有2个字节的空间来存储该变长列的实际长度。

### ref

### rows
扫描记录的总数

### filtered
满足条件的记录数量

### Extra
using index : 查询中只需要用到索引，不需要回表

Using index condition: where condition contains indexed and non-indexed column and the optimizer will first resolve the indexed column and will look for the rows in the table for the other condition (index push down) 中文解释为:索引条件下推


Using filesort: 在内存或者磁盘上对数据进行排序，最好想办法将使用排序的执行方式改为使用索引进行排序

Using temporary: 查询中使用到了内部的临时表。建立与维护临时表要付出很大成本的，所以我们最好能使用索引来替代掉使用临时表

Start temporary/End temporary: 查询优化器会优先尝试将IN子查询转换成semi-join，而semi-join又有好多种执行策略，当执行策略为DuplicateWeedout时，也就是通过建立临时表来实现为外层查询中的记录进行去重操作时，驱动表查询执行计划的Extra列将显示Start temporary提示，被驱动表查询执行计划的Extra列将显示End temporary提示。


## optimizer trace使用
- 开启 set optimizer_trace="enable=on";
- 执行sql语句
- 通过select * from information_schema.OPTIMIZER_TRACE; 查看
- 关闭 set optimizer_trace="enable=off";

## 连接
对于两表连接来说，驱动表只会访问一次，但被驱动表会被访问多次。每一个驱动表中取出的数据，都会到被驱动表中去做一次单表访问。
###### 外连接的驱动表是固定的，也就是说左（外）连接的驱动表就是左边的那个表，右（外）连接的驱动表就是右边的那个表

##### 内连接没有where的情况下选小表作驱动表，有where选带where的表作驱动表，都有where选小表作驱动表。

总之explain中第一个出现的表就是驱动表。


## semi-join
在mysql5.5及以前的版本中，执行子查询会先扫描外表中的所有数据，每条数据将会传到内表中与之关联，如果外表很大，那么性能上将会很差。

推出semi-join后, semi-join先执行内表的数据过滤出数据集，外表只需要在此数据集上进行查询即可。

semi-join就是一个子查询，它主要用于去重，当外表查找在内表满足条件的records时，返回外表的records，也就是说它只返回存在内表中的外表的记录。
semi-join的策略有四个:
- DuplicateWeedout strategy
- Firstmatch Strategy
- Loosescan Strategy
- Materialize scan/Materialize lookup Strategy

影响半连接策略的因素:
- inner-table和outer-table上的数据量
- inner-table和outer-table上是否有快速定位数据的索引

通过变量optimizer_switch进行开关。

在需要去重的sql语句中可以使用semi-join特性，(select distinct 性能太差不考虑), 但还是能用连接时用连接，不行再用子查询，子查询根据情况开启semi-join

###### check this out! https://dev.mysql.com/doc/refman/5.7/en/semijoins.html

## 字段类型优化
### 使用更小的类型

Use the most efficient (smallest) data types possible. MySQL has many specialized types that save disk space and memory. For example, use the smaller integer types if possible to get smaller tables. MEDIUMINT is often a better choice than INT because a MEDIUMINT column uses 25% less space

### 尽量避免用NULL

Declare columns to be NOT NULL if possible. It makes SQL operations faster, by enabling better use of indexes and eliminating overhead for testing whether each value is NULL. You also save some storage space, one bit per column. If you really need NULL values in your tables, use them. Just avoid the default setting that allows NULL values in every column.

### 索引
#### 主键类型尽量小
The primary index of a table should be as short as possible. This makes identification of each row easy and efficient. For InnoDB tables, the primary key columns are duplicated in each secondary index entry, so a short primary key saves considerable space if you have many secondary indexes

#### 联合索引
 If you access a table mostly by searching on a combination of columns, create a single composite index on them rather than a separate index for each column.The first part of the index should be the column most used. If you always use many columns when selecting from the table, the first column in the index should be the one with the most duplicates, to obtain better compression of the index