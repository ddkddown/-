# HTTP传输

HTTP报文由HTTP实体首部来描述

- Content-Type: 实体种承载对象的类型，用于说明实体的基本媒体类型，客户端根据类型来处理内容，如:

  ```
  Content-Type: text/plain
  ```

- Content-Length: 所传送的实体主体的长度或大小。除非使用了分块编码，否则Content-Length首部就是带有实体主体的报文必须要使用的。其次，对于持久连接来说，Content-Length是必不可少的，因为持久化连接中，http报文是流式的，需要Content-Length来标明某个实体主体在哪儿开始，在哪儿结束 (除非采用分块编码)。

  ```
  Content-length: 18
  Hi! i'm a message! //加上空格刚好18个字符
  ```

  

- Content-Language: 与传送对象最相配的人类语言

- Content-Encoding: 对象数据所做的变换，如压缩，对数据压缩后，Content-Length应该是压缩后的长度大小。如果客户端需要告诉服务端自己接受的压缩编码方式，需要在Accept-Encoding头部中注明

  ```
  content-encoding: br
  ```

- Content-Location: 一个备用位置，请求时通过它来获得对象
- Content-Range: 如果此报文为部分实体，用该首部标明这是属于整体的哪个部分
- Content-MD5: 实体主体内容的校验和。尽管HTTP通常都是在TCP可靠传输协议上实现的，但仍然有很多因素会导致报文的一部分在传输过程种被修改，比如有不兼容的转码代理或者中间代理有误，所以需要对实体数据进行md5校验。
- Last-Modified: 所传输内容在服务器上创建或者最后修改的时间
- Expires: 实体数据将要失效的日期时间
- Allow: 该资源所允许的请求方法，例如GET和HEAD

- Range: 范围请求，允许客户端只请求文档的一部分



## 传输编码和分块编码

- Transfer-Encoding: 告知接收方为了可靠地传输报文，已经对其进行了何种编码

  ```
  Transfer-Encoding: chunked
  ```

  传输编码集合中必须包含分块，唯一例外就是使用关闭连接来结束报文。

  分块传输编码不能多次作用到一个报文主体上。