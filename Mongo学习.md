### CRUD
- insert
```
//文档post
post = {"title" : "test",
        "date" : new Date()}

//集合blog
db.blog.insert(post)

// 批量插入
db.blog.insertMany([{"_id":1},{"_id":2}])

// 批量插入时如果某条插入失败，就会终止插入并且不会回滚, 此时可以使用try/catch定位错误插入
try {

} catch(e) {
    print(e);
}
```
- find
```
// 查找集合中全部文档(最多自动显示20个)
db.blog.find()
// 查找集合中文档，只返回title(置为1)，且不返回_id(置为0)
db.blog.find({}, {"title":1, "_id":0})
// 添加搜索条件, 例子:小于等于30,大于等于18.
db.blog.find({"age":{"$gte":18, "$lte":30}})
// 查找单个
db.blog.findOne()
```
- update
```
//搜索条件，更新值
db.blog.update({title:"test"}, {title:"updating"})
db.blog.update({"_id":ObjectId("1")}, newVal)

//修改器, 将date+1
db.blog.update({"title":"test"}, {"$inc":{"date":1}})

//修改器，将val+1,若键不存在，则创建
db.blog.update({"title":"test"}, {"$set":{"val":1}})

//修改器，去除键
db.blog.update({"title":"test"}, {"$unset":{"val":1}})

//修改器，给数组增加值
db.blog.update({"title":"test"}, {"$push":{"val":2}})

//upsert模式，update第三个参数true，如果不存在则创建
db.blog.update({"url":"/blog"}, {"$inc":{"date":1}},true)

//更新多个匹配文档，将update第四个参数true
db.blog.update({"url":"/blog"}, {"$inc":{"date":1}},false, true)
```
- remove
```
//删除全部文档，但不会删除集合
db.blog.remove()
//搜索条件
db.blog.remove({title:"updating"})
```

- 数据库
```
//创建数据库
use 数据库名 //不存在自动创建。此时数据库存放在内存中，show dbs看不到，只有当插入集合时才持久化到磁盘

//查看所有数据库
show dbs/databases

//查看当前数据库
db

//三个默认库
admin //存放系统用户以及权限文档
local //存在此库中的文档永远不会被复制
config //用于分片设置。

// 删除数据库
db.dropDatabase()

```

- 集合
```
//显式创建
db.createCollection("name")
//隐式创建
db.collectionname.insert()
//删除集合
db.collectioname.drop()
//查看集合
show collections/tables
```

- 分页查询
```
// 查询所有
db.collectioname.count()
// 条件查找
db.collectioname.count({"userid":1003})
// 列表查询, skip跳过前10条
db.collectioname.find().skip(10).limit(10)

//sort查询, 1升序列，-1降序列
db.collectioname.find().sort("_id":1)
```
### 索引
```
// 在username字段上创建索引
db.users.ensureIndex({"username":1})

// 创建复合索引
db.users.ensureIndex({"age":1, "username":1})

// 通过explain()查看查询过程信息
db.users.find({"username":"user101"}).explain()

//IXSCAN 通过索引扫描， COLLSCAN 全表扫描

// 创建唯一索引, dropDups会强制建立唯一索引，遇到重复的值，只会保存第一个，之后的重复文档都会被删除，不推荐使用！
db.users.ensureIndex({"username:1"}, {"unique":true, "dropDups":true})

// 查看集合上创建的索引
db.users.getIndexes()

//删除索引
db.users.dropIndex("userName_1")

```

- 执行计划
```
db.collectioname.find(query,option).explain(options)
```

### 分片
mongodb支持自动分片，用一个mongodb作为路由服务器管理一群mongodb子集，这样对外部应用程序而言，就像是一直在使用单机的mongodb一样。路由服务器知道哪些数据位于哪个分片，可以将请求转发给相应的分片。
####### 复制是让多台服务器都拥有相同的数据副本，作为镜像存在。分片是拥有不同数据子集。

- 创建集群
mongo --nodb
cluster = new ShardingTest({"shards":3, "chunksize":1}), 此时会创建包含3个分片的集群以及一个mongos服务，连接到mongos服务就可以开始使用集群
db = (new Mongo("localhost:mongos端口")).getDB("数据库名")

- 查看集群状态
sh.status()

- 对数据库启用分片
sh.enableSharding("test")
- 对选定为分片使用的字段建立索引
db.collectioname.ensureIndex({"字段名":1})
- 对集合进行分片
sh.shardCollection("数据库.集合名", {"username":1})
- 关闭集群
cluster.stop()
