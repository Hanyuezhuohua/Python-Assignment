#ifndef FLOWCONTROLCENTER_H
#define FLOWCONTROLCENTER_H

#include"bfins.h"

enum Statement{Running, Continued, Broken, Returned};

class flowcontrolcenter{
private:
    static const int MAXn = 50010; 
    Statement State[MAXn];
    int top;
public:
    flowcontrolcenter(){
        memset(this , 0 , sizeof *this);
    }
    inline void push(Statement x){
        State[++top] = x;
    }
    inline Statement TOP(){
        return State[top];
    }
    inline void turnto(Statement x){
        State[top] = x;
    }
    inline void pop(){
        --top;
    }
};

#endif
