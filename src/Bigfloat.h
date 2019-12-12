#ifndef BIGFLOAT_H
#define BIGFLOAT_H

#include <iostream>
#include <vector>
#include <cstring>
#include <cstdio>
#include <cassert>
#include <algorithm>
using namespace std;

class Bigfloat{
public:
    int len;
    vector<int>val;
    bool neg;// >=0: neg=0 ; <0: neg=1 
    Bigfloat(int x){
    	if(x==0){
    		len = 1;
    		neg = 0;
    		val.push_back(0);
		}
		else{
            if(x > 0) neg = 0;
            else{
			   neg =1;
			   x = -x;
		    }
            len = 0;
            while(x){
               val.push_back(x%10);
                x /= 10;
                len++;
            }
            
            while(val[len-1] == 0 && len > 1){
                val.pop_back();
                len--;
            }
        }
    }


    Bigfloat(){
        len = 0;
        neg = 0;
    }

    Bigfloat(const string &s){
        neg = 0;
        len = 0;
        for(int i = 0 ;i<s.length() ; i++){
            val.push_back(s[i] - '0');
            len++;
        }
        reverse(val.begin() , val.end());
        while(val.size()>1 && !val[val.size()-1]){
            val.pop_back();
            len--;
        }
        if(!val.size()) neg = 0;
    }


    friend bool operator <(const Bigfloat &a , const Bigfloat &b){
        if(a.neg != b.neg) return a.neg;
        bool res = 0; 
        if(a.len < b.len) res = 1;
        else if(a.len > b.len) res = 0;
        else{
            bool flag = 0;
            for(int i=a.len-1; ~i ;i--){
                if(a.val[i] < b.val[i]){
                    res = 1;
                    flag = 1;
                    break;
                }
                if(a.val[i] > b.val[i]){
                    res = 0;
                    flag = 1;
                    break;
                }
            }
        }
        if(a.neg) res ^= 1;
        return res;
    }

    static bool compare(const Bigfloat &a , const Bigfloat &b){
        bool res = 0; 
        if(a.len < b.len) res = 1;
        else if(a.len > b.len) res = 0;
        else{
            bool flag = 0;
            for(int i=a.len-1; ~i ;i--){
                if(a.val[i] < b.val[i]){
                    res = 1;
                    flag = 1;
                    break;
                }
                if(a.val[i] > b.val[i]){
                    res = 0;
                    flag = 1;
                    break;
                }
            }
        }
        if(a.neg) res ^= 1;
        return res;
    }

    friend bool operator ==(const Bigfloat &a , const Bigfloat &b){
        if(a.neg != b.neg || a.len != b.len) return 0;
        for(int i = a.len-1;~i;i--) if(a.val[i] != b.val[i]) return 0;
        return 1;
    }

    friend bool operator !=(const Bigfloat &a , const Bigfloat &b){
        return !(a == b);
    }

    friend bool operator >(const Bigfloat &a ,const Bigfloat &b){
        return !(a == b || a < b); 
    }

    friend bool operator <=(const Bigfloat &a , const Bigfloat &b){
        return !(a > b);
    }

    friend bool operator >=(const Bigfloat &a ,const Bigfloat &b){
        return !(a < b);
    }

    Bigfloat operator -() const {
        Bigfloat res = *this;
        res.neg ^= 1;
        if(res.len == 1 && res.val[0]==0) res.neg=0;
        return res;
    }

    friend Bigfloat operator +(const Bigfloat &a , const Bigfloat &b){
        Bigfloat res;
        res.val.push_back(0);
        if(a.len > b.len) res.len = a.len;
        else res.len = b.len;
        if(a.neg == b.neg){
            res.neg = a.neg;
            for(int i = 0;i<res.len;i++){
                if(i){
                    res.val.push_back(res.val[i-1]/10);
                    res.val[i-1]%=10;
                }
                if(i<a.len) res.val[i]+=a.val[i];
                if(i<b.len) res.val[i]+=b.val[i];
            }
            if(res.val[res.len-1] >=10){
            	res.val.push_back(res.val[res.len-1]/10);
                res.val[res.len-1]%=10;
                res.len++;
			}
        }
        else{
        	if(a.neg == 1 && b.neg == 0) {
        		res = -(-a-b);
            }
        	else if(a.neg == 0 && b.neg == 1){
        		if(a>(-b)){
        		    int f=0,tmp;
        		    res.val.clear();
        		    for(int i = 0; i < res.len ; ++i){
        			    tmp = a.val[i] - (b.len > i ? b.val[i] : 0) +f;
					    if(tmp < 0){
						    f = -1;
						    tmp +=10;
						    res.val.push_back(tmp);
					    }
					    else{
						    f = 0;
						    res.val.push_back(tmp);
					    }
					} 
				}
			    else if(a == (-b)){
			    	res.len = 1;
			    	res.neg = 0;
				}
			    else if(a < (-b)){
			    	res = -(-b-a);
				}
			}
        }
        while(res.val[res.len-1] == 0 && res.len > 1){
            res.val.pop_back();
            res.len --;
        }
        if(res.val[0] == 0 && res.len == 1) res.neg = 0;
        return res;
    }

    friend Bigfloat operator -(const Bigfloat &a , const Bigfloat &b){
        Bigfloat res;
        res = a + (-b);
        return res;
    }

    friend Bigfloat operator *(const Bigfloat &a , const Bigfloat &b){
        Bigfloat res;
        if(a.neg == b.neg) res.neg = 0;
        else res.neg = 1;
        res.len = a.len + b.len;
        res.val.resize(res.len);
        for(int i = 0;i<a.len;i++){
            for(int j = 0;j<b.len;j++){
                res.val[i + j] += a.val[i] * b.val[j];
            }
        }
        for(int i = 0 ; i < res.len ; i++){
        	res.val[i+1] += res.val[i] / 10;
        	res.val[i] %= 10;
		}
        while(res.len>1 && res.val[res.len-1]==0) {
            res.val.pop_back();
            res.len-- ;
        }
        if(res.len==1 && res.val[0]==0) res.neg=0;
        return res;
    }

    friend Bigfloat operator /(const Bigfloat &a , const Bigfloat &b){
        Bigfloat res;
        Bigfloat tmpA = a;
        res.val.clear();
        if(a.neg == 0 && b.neg == 0){
            if(compare(a,b)){
                res.neg = 0;
                res.len = 1;
                res.val.push_back(0);
            }
            else{
                res.val.resize(a.len);
                for(int i =tmpA.val.size()-b.val.size();b<tmpA;i--){
                	Bigfloat tmp;
                	tmp.val.assign(i+1,0);
                	tmp.val.back()=1;
                	tmp.len = tmp.val.size();
                	Bigfloat Tmp = tmp*b;
                	while(Tmp<=tmpA){
                		tmpA = tmpA-Tmp;
                		res.val[i]++;
					}
				}
            }
			while(res.val.size()>1 && res.val[res.val.size()-1]==0) {
            res.val.pop_back();
            }       
            res.len = res.val.size();
        }
        if(a.neg == 1 && b.neg == 1) res=(-a)/(-b);
        if(a.neg == 0 && b.neg == 1) {
            res=-((a+(-b)-1)/(-b));
        }
        if(a.neg == 1 && b.neg == 0) {
            res=-(((-a)+b-1)/b);
        }
        return res;
        if(res.len==1 && res.val[0]==0) res.neg=0;
    }

    friend Bigfloat operator %(const Bigfloat &a , const Bigfloat &b){
        Bigfloat res;
        res = a - (a/b)*b;
        return res;
    }

    Bigfloat &operator =(const Bigfloat &a){
        len = a.len;
        val = std::move(a.val);
        neg = a.neg;
        return *this;  
    }

    Bigfloat operator +=(const Bigfloat &a){
        return *this = *this + a;
    }

    Bigfloat operator -=(const Bigfloat &a){
        return *this = *this - a;
    }

    Bigfloat operator *=(const Bigfloat &a){
        return *this = *this * a;
    }

    Bigfloat operator /=(const Bigfloat &a){
        return *this = *this / a;
    }

    Bigfloat operator %=(const Bigfloat &a){
        return *this = *this % a;
    }

    Bigfloat &operator ++(){
        return *this = *this + Bigfloat((int)1);
    }

    const Bigfloat operator ++(int){
        Bigfloat tmp = *this;
        ++(*this);
        return tmp;
    }

    Bigfloat &operator --(){
        return *this = *this - Bigfloat((int)1);
    }

    const Bigfloat operator --(int){
        Bigfloat tmp = *this;
        --(*this);
        return tmp;
    }


    string String() const{
        if(!val.size()) return"0";
        string s = neg? "-" :  "";
        for(int i = val.size()-1 ;i>=0 ; i--){
           s = s + (char)('0'+val[i]);
        }
        return s;
    }

    bool Bool() const{
        bool flag;
        if(len == 1 && val[0] == 0) flag = false;
        else flag = true; 
        return flag;
    }

    double Double() const{
        double res = 0.0;
        for(int i = len-1 ; ~i ; i--){
            res = res * 10 + val[i];
        }
        return res;
    }

    void dOUBLE(const double &d){
        long long ll = d;
        val.clear();
        if(ll<0){
            neg = 1;
            ll = -ll;
        }
        else neg = 0;
        while(ll){
            val.push_back(ll%10);
            ll /= 10;
        }
        while(val[len-1] == 0 && len > 1){
            val.pop_back();
            len--;
        }
    } 

    void sTRING(const string &s){
        val.clear();
        neg = 0;
        len = 0;
        for(int i = 0 ; i < s.length() ;i++){
            len++;
            val.push_back(s[i]-'0');
        }
        reverse(val.begin() , val.end());
        while(val[len-1] == 0 && len > 1){
            val.pop_back();
            len--;
        }
        if(!len) neg = 0;
    }

    void bOOL(const bool &b){
        val.clear();
        neg = 0;
        len = 1;
        if(b) val.push_back(1);
        else val.push_back(0);
    }

    void print() const{
        if(neg) putchar('-');
        for(int i = val.size()-1 ; i>=0 ; i--){
           cout << val[i];
        }
    }

private:
};

#endif