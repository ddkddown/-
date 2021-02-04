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

4. 尽可能延后变量定义的出现时间,直到能够给予变量初值实参为止：因为当程序的控制流到达这个变量定义式时，就得承受当前变量的构造成本，当变量离开作用域时又要承受析构成本。

5. 对dynamic_cast的使用要谨慎，否则会导致代码又大又慢。当代码面临以下情况时
```
class A {};

class B : public A {
public:
    void do(){...}
};

class C : public A {
public:
    void do(){...}
};

....


A *a;
if(B *b = dynamic_cast<B*>(a)) {
    b->do();
}
else if(C *c = dynamic_cast<C*>(c)) {
    c->do();
}

...
```

可以更改为在base类中加上虚函数

```
class A {
public:
    virtual void do(){
        //do nothing
    }
};

class B : public A {
public:
    void do(){...}
};

class C : public A {
public:
    void do(){...}
};


....

A *a;
a->do();
```

6. 异常安全性
- 不泄漏任何资源
- 不允许数据败坏

此代码块就不满足
```
...

void PrettyMenu::changeBackground(std::istream &imgSrc) {
    lock(&mutex); //当new抛出异常时就不会解锁
    //可更改为局部变量Lock m1(&mutex), 析构时自动释放锁。
    delete bgImage;
    ++imageChanges;//当new抛出异常时,imageChanges也已经被改变
    //可将累加语句放于new 之后以避免
    bgImage = new Image(imgSrc);
    unlock(&mutex);
}
...
```
7. inline函数：将每一个调用都用函数本体替换之，这样做可以不必蒙受函数调用导致的额外开销，但是却会增加目标码大小。在一台内存有限的机器上，过度的inline会导致程序体积太大，即便是拥有虚拟内存，也会因为inline造成的代码膨胀导致额外的换页行为，降低指令高速缓存装置的命中率，以及效率损失。
###### 如果程序要取某个inline函数的地址，编译器通常必须为该函数生成一个outline函数本体，因为编译器不能将指针指向一个并不存在的函数。

