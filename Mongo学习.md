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
