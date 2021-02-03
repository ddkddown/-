1. define 不能用来定义class专属变量，因为一旦宏被定义，就会一直在后续有效（除非undef）
```
class A {
#define TEST 1
};

int main() {
int i = TEST; //有效
#undef TEST
i = TEST; //无效
}
```

改用枚举或者static常量成员

```
class A {
enum test {
  TEST = 1
};
};

class B {
static const int TEST = 1;
};
```


2. 初始化列表是按照声明顺序进行初始化的，如果成员中有数组和数组大小成员，一定需要保证数组大小声明在数组前，不然可能导致未定义行为.
```
class A {
public:
A():size(1),
    p(new char(size)) {}
private:
int size; //确保size声明在p前
char *p;
};

```
3. shared_ptr/auto_ptr（以及某些其他智能指针）保存数组是错误的，因为智能指针在析构时只会调用delete而不是delete []
###### 如果确实需要用到可以考虑boost::scoped_array

4. 尽可能延后变量定义的出现时间：因为当程序的控制流到达这个变量定义式时，就得承受当前变量的构造成本，当变量离开作用域时又要承受析构成本。
