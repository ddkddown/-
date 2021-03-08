//装饰器通过继承基类增加特定行为（编译时确定）,组合基类的其他子类对象，达到在进行运行时改变其行为
#include <iostream>
#include <string>

class EatFood {
public:
  virtual string getDesc() {
    return "eat food";
  }
};

class LaterEatFood : public EatFood {
public:
  string getDesc() {
    return "later eat food";
  }
};

class Decorator : public EatFood {
public:
  void setWhen(EatFood *when) {
    when_ = when;
  }
  
  virtual string getDesc() {
    if(when_) return when_->getDesc();
    return "null ptr";
  }
private:
  EatFood *when_;
};

class MilkDecorator : public Decorator {
public:
  string addFunc() {return "add milk";}
  
  virtual string getDesc() { 
    return Decorator::getDesc()+addFunc();
  }
};

int main() {
 EatFood *c = new LaterEatFood();
 Decorator *d = new MilkDecorator();
 d->setWhen(c);
 d->getDesc();
 
 delete c;
 delete d;
 return 0;
}
