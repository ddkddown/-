#include <iostream>
using namespace std;

class Duck {
public:
    void quack();
    void fly();
    void pet();
};

class Turkey {
    void gobble();
    void jump();
};

class TurkeyAdapter : public Duck {
public:
    TurkeyAdapter(Turkey turkey) {
	this.turkey_ = turkey;
    }

    void quack() {
    	turkey_.gobble();
    }

    void fly() {
    	turkey_.jump();
    }

    void pet() {
    	throw exception e("no match func");
    }
private:
    Turkey turkey_;
};
