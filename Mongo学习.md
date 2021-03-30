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
