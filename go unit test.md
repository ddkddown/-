#  go unit test

### 创建go单元测试文件

如源文件是example.go

```
package example

func Add(i int, j int) int {
	return i + j
}
```

单元测试文件以*_test.go结尾，如example_test.go

```
package example

import (
	"testing" //单元测试包
)

//单元测试函数命令规则为Test开头，之后是具体待测函数名，且首字母必须大写
func TestAdd(t *testing.T) {
	got := Add(1, 2)
	want := 3
	if got != want {
		// 打印单元测试错误信息
		t.Errorf("want:%d, got:%d", want, got)
	}

}
```

###  执行单元测试

```
go test //遍历当前目录下所有符合*_test.go命名的文件,并执行符合命名规范的测试函数
go test -v //打印测试函数名字和运行时间
go test -run='TestAdd' //指定进行测试的单元函数
go test -cover //打印单元测试覆盖率
```



