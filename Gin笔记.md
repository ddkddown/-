# quick start
```
package main

import "github.com/gin-gonic/gin"

func main() {
	r := gin.Default()
	r.GET("/ping", func(c *gin.Context) {
		c.JSON(200, gin.H{
			"message": "pong",
		})
	})
	r.Run() 
}
```

r是gin中的一个路由，可通过r的方法:GET,POST,PATCH等来绑定url以及处理函数。

处理函数的类型是:
```
type HandlerFunc func(*Context)
```

其中Context是gin中对http会话的一个封装
```
type Context struct {
	writermem responseWriter
	Request   *http.Request
	Writer    ResponseWriter

    ......
}
```

Run()成员函数会解析传入的ip:port, 同时在其内部也是封装的net/http的ListenAndServe方法
```
func (engine *Engine) Run(addr ...string) (err error) {
	defer func() { debugPrintError(err) }()

	address := resolveAddress(addr)
	debugPrint("Listening and serving HTTP on %s\n", address)
	err = http.ListenAndServe(address, engine)
	return
}
```
可以这样指定ip:port 
```
r.Run("192.168.10.210:8080")
```

# 路由匹配
```
func main() {
	router := gin.Default()

	router.GET("/user/:name", func(c *gin.Context) {
		name := c.Param("name")
		c.String(http.StatusOK, "Hello %s", name)
	})

	router.GET("/user/:name/*action", func(c *gin.Context) {
		name := c.Param("name")
		action := c.Param("action")
		message := name + " is " + action
		c.String(http.StatusOK, message)
	})

	router.POST("/user/:name/*action", func(c *gin.Context) {
		c.FullPath() == "/user/:name/*action" // true
	})

	router.Run(":8080")
}
```

:表示一个通配符，匹配模式是精确匹配，且只能匹配一个，例如。
```
/test/:everything

匹配:
/test/123
/test/hello
/test/测试

不匹配:
/test
/test/
/test/123/321
```

*也表示一个通配符，表示匹配所有，例如。
```
/test/*everything

匹配:
/test
/test/
/test/123
/test/123/321
/test/hello
/test/测试
```

# query参数

## 对于GET
```
func main() {
	router := gin.Default()

	// The request responds to a url matching:  /welcome?firstname=Jane&lastname=Doe
	router.GET("/welcome", func(c *gin.Context) {
		firstname := c.DefaultQuery("firstname", "Guest")
		lastname := c.Query("lastname") // shortcut for c.Request.URL.Query().Get("lastname")

		c.String(http.StatusOK, "Hello %s %s", firstname, lastname)
	})
	router.Run(":8080")
}
```
该GET请求中参数是firstname=Jane&lastname=Doe, 通过Query方法取得参数值， DefaultQuery在未找到参数时会返回一个指定的默认值。

## 对于POST
```
router.POST("/form_post", func(c *gin.Context) {
		message := c.PostForm("message")
		nick := c.DefaultPostForm("nick", "anonymous")

		c.JSON(200, gin.H{
			"status":  "posted",
			"message": message,
			"nick":    nick,
		})
	})
```
POST请求的处理和GET差不多，不再概述。


当请求中既有url参数又有payload的时候，例如:
```
POST /post?ids[a]=1234&ids[b]=hello HTTP/1.1
Content-Type: application/x-www-form-urlencoded

names[first]=thinkerou&names[second]=tianou
```

可以分别调用QueryMap和PostFormMap获取
```
router.POST("/post", func(c *gin.Context) {

		ids := c.QueryMap("ids")
		names := c.PostFormMap("names")

		fmt.Printf("ids: %v; names: %v", ids, names)
	})
```

# 路由组
```
v1 := router.Group("/v1")
{
	v1.POST("/login", loginEndpoint)
	v1.POST("/submit", submitEndpoint)
	v1.POST("/read", readEndpoint)
}

//url: /v1/login, /v1/submit, /v1/read

// Simple group: v2
v2 := router.Group("/v2")
{
	v2.POST("/login", loginEndpoint)
	v2.POST("/submit", submitEndpoint)
	v2.POST("/read", readEndpoint)
}

//url: /v2/login, /v2/submit, /v2/read

```

# 设置获取coockie
```
router.GET("/cookie", func(c *gin.Context) {

        cookie, err := c.Cookie("gin_cookie")

        if err != nil {
            cookie = "NotSet"
            c.SetCookie("gin_cookie", "test", 3600, "/", "localhost", false, true)
        }

        fmt.Printf("Cookie value: %s \n", cookie)
    })
```