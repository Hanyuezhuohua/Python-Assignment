#ifndef BFINS_H
#define BFINS_H

#include "Bigfloat.h"
#include <cmath>
using namespace std;

class BFINS{
public:
    bool b;
    double d;
    Bigfloat bb;
    string s;
    int category;
    BFINS(){category=0 , b = 0 , bb =Bigfloat(0), d=0 ; s = "";}

    explicit BFINS(const int &x){
        category = x;
    }

    explicit BFINS(const bool &x){
        category = 1;
        b = x;
    }

    explicit BFINS(const Bigfloat &x){
        category = 2;
        bb = x;
    }

    explicit BFINS(const double &x){
        category = 3;
        d = x;
    }

    explicit BFINS(const string &x){
        category = 5;
        s = x;
    }

    explicit operator bool() const{
        bool res;
        switch(category)
        {
        case 1:
            res = b;
            break;
        case 2:
            res = bb.Bool();
            break;
        case 3:
            res = d;
            break;
        case 5:
            res = s.length();
            break;
        default:
            break;
        }
        return res;
    }

    explicit operator Bigfloat() const{
        Bigfloat res;
        switch(category)
        {
        case 1:
            res.bOOL(b);
            break;
        case 2:
            res = bb;
            break;
        case 3:
            res.dOUBLE(d);
            break;
        case 5:
            res = Bigfloat(s);
            break;
        default:
            break;
        }
        return res;
    }

    explicit operator double() const{
        bool res;
        switch(category)
        {
        case 1:
            res = b;
            break;
        case 2:
            res = bb.Double();
            break;
        case 3:
            res = d;
            break;
        case 5:
            res = std::stod(s);
            break;
        default:
            break;
        }
        return res;
    }

    explicit operator string() const{
        string res;
        switch(category)
        {
        case 1:
            if(b)res = string("true");
            else res = string("false");
            break;
        case 2:
            res = bb.String();
            break;
        case 3:
            res = std::to_string(d);
            break;
        case 5:
            res = std::move(s);
            break;
        default:
            break;
        }
        return res;
    }
    
    BFINS Bool() {
        BFINS ret = *this;
    	bool flag = (bool)ret;
    	BFINS res(flag);
        return res;
    }

    BFINS Double() {
        BFINS ret = *this;
    	double flag = (double)ret;
    	BFINS res(flag);
        return res;
    }

    BFINS Int() {
        BFINS ret = *this;
    	Bigfloat flag = (Bigfloat)ret;
    	BFINS res(flag);
        return res;
    }

    BFINS String() {
        BFINS ret = *this;
    	string flag = (string)ret;
    	BFINS res(flag);
        return res;
    };

    friend bool operator <(const BFINS &a , const BFINS &b){
        int MAX = max(a.category , b.category);
        switch(MAX)
        {
        case 1:
            return (Bigfloat)a < (Bigfloat)b;
            break;
        case 2:
            return (Bigfloat)a < (Bigfloat)b;
            break;
        case 3:
            return (double)a < (double)b;
            break;
        case 5:
            return a.s < b.s;
            break;
        default:
            break;
        }     
    };

    friend bool operator ==(const BFINS &a , const BFINS &b){
        int MAX = max(a.category , b.category);
        switch(MAX)
        {
        case 1:
            return (Bigfloat)a == (Bigfloat)b;
            break;
        case 2:
            return (Bigfloat)a == (Bigfloat)b;
            break;
        case 3:
            return (double)a == (double)b;
            break;
        case 5:
            return a.s == b.s;
            break;
        default:
            break;
        }     
    };

    friend bool operator !=(const BFINS &a , const BFINS &b){
        return !(a == b);
    };

    friend bool operator >(const BFINS &a , const BFINS &b){
        return !(a < b || a == b);
    };

    friend bool operator <=(const BFINS &a , const BFINS &b){
        return !(a > b);
    };

    friend bool operator >=(const BFINS &a , const BFINS &b){
        return !(a < b);
    };
    
    BFINS operator +() const{
        switch(category)
        {
        case 1:
            return BFINS((Bigfloat)(*this));
            break;
        case 2:
            return BFINS(bb);
            break;
        case 3:
            return BFINS(d);
            break;
        default:
            break;
        }     
    };

    BFINS operator -() const{
        switch(category)
        {
        case 1:
            return BFINS(-(Bigfloat)(*this));
            break;
        case 2:
            return BFINS(-bb);
            break;
        case 3:
            return BFINS(-d);
            break;
        default:
            break;
        }     
    };

    friend BFINS operator +(const BFINS &a , const BFINS &b){
        int MAX = max(a.category , b.category);
        switch(MAX)
        {
        case 1:
            return BFINS((Bigfloat)a+(Bigfloat)b);
            break;
        case 2:
            return BFINS((Bigfloat)a+(Bigfloat)b);
            break;
        case 3:
            return BFINS((double)a+(double)b);
            break;
        case 5:
            return BFINS(a.s+b.s);
            break;
        default:
            break;
        }
    };
    
    friend BFINS operator -(const BFINS &a , const BFINS &b){
        int MAX = max(a.category , b.category);
        switch(MAX)
        {
        case 1:
            return BFINS((Bigfloat)a-(Bigfloat)b);
            break;
        case 2:
            return BFINS((Bigfloat)a-(Bigfloat)b);
            break;
        case 3:
            return BFINS((double)a-(double)b);
            break;
        default:
            break;
        }
    };

    friend BFINS operator *(const BFINS &a , const BFINS &b){
        int MAX = max(a.category , b.category);
        string S = "";
        string tmp;
        Bigfloat Tmp;
        switch(MAX)
        {
        case 1:
            return BFINS((Bigfloat)a*(Bigfloat)b);
            break;
        case 2:
            return BFINS((Bigfloat)a*(Bigfloat)b);
            break;
        case 3:
            return BFINS((double)a*(double)b);
            break;
        case 5:
            tmp = a.category == 5? a.s : b.s;
            Tmp = a.category == 3? a.bb :b.bb;
            while(Tmp > Bigfloat(0)){
                Tmp--;
                S += tmp;
            }
            return BFINS(S);
            break;
        default:
            break;
        }
    };

    friend BFINS operator /(const BFINS &a , const BFINS &b){
        int MAX = max(a.category , b.category);
        switch(MAX)
        {
        case 1:
            return BFINS((double)a/(double)b);
            break;
        case 2:
            return BFINS((double)a/(double)b);
            break;
        case 3:
            return BFINS((double)a/(double)b);
            break;
        default:
            break;
        }
    }

    friend BFINS operator ^(const BFINS &a , const BFINS &b){
      int MAX = max(a.category , b.category);
        switch(MAX)
        {
        case 1:
            return BFINS((Bigfloat)a/(Bigfloat)b);
            break;
        case 2:
            return BFINS((Bigfloat)a/(Bigfloat)b);
            break;
        default:
            break;
        }
    }; 
    
    friend BFINS operator %(const BFINS &a , const BFINS &b){
        int MAX = max(a.category , b.category);
        switch(MAX)
        {
        case 1:
            return BFINS((Bigfloat)a%(Bigfloat)b);
            break;
        case 2:
            return BFINS((Bigfloat)a%(Bigfloat)b);
            break;
        default:
            break;
        }
    };

    BFINS &operator=(const BFINS &a){
        category = a.category;
        b = a.b;
        d = a.d;
        bb = a.bb;
        s = std::move(a.s);
    };

    BFINS operator +=(const BFINS &a){
        BFINS ret = *this;
        ret = ret + a;
        return ret;
    };

    BFINS operator -=(const BFINS &a){
        BFINS ret = *this;
        ret = ret - a;
        return ret;
    };

    BFINS operator *=(const BFINS &a){
        BFINS ret = *this;
        ret = ret * a;
        return ret;
    };

    BFINS operator /=(const BFINS &a){
        BFINS ret = *this;
        ret = ret / a;
        return ret;
    };

    BFINS operator ^=(const BFINS &a){
        BFINS ret = *this;
        ret = ret ^ a;
        return ret;
    }

    BFINS operator %=(const BFINS &a){
        return *this = *this % a;
    };
    
    friend BFINS operator && (const BFINS &a ,const BFINS &b){
        return BFINS((bool)a && (bool)b);
    }

    friend BFINS operator || (const BFINS &a ,const BFINS &b){
        return BFINS((bool)a && (bool)b);
    }

    BFINS operator !() const{
        return BFINS(!(bool)(*this));
    }

    BFINS operator &=(const BFINS &a){
        return *this = *this && a;
    }

    BFINS operator |=(const BFINS &a){
        return *this = *this || a;
    }

    void print() const{
        switch(category){
        case 0:
            break;
        case 1:
            if(b == 1) cout << "True";
            else cout << "False";
            break;
        case 2:
            bb.print();
            break;
        case 3:
            printf("%.6lf",d);
            break;
        case 4:
            cout << "None";
            break;
        case 5:
            cout << s;
            break;
        default:
            break;
        }
    }
}; 
#endif