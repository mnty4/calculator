// needed to find an algorithm for square root and the method 
// for squaring would involve converting decimals to fractions 
// e.g. 5^7.25 = 5^(725/100) which would mean that you
// would need to calculate 5^725 which would of course cause a
// stack allocation error

#include <iostream>
#include <string>
#include <vector>
#include <cmath>

class Digit {
    public:
        char kind = '0';
        int val;
        Digit() {}
        Digit(char ch)
            :kind(ch) {}
        Digit(char ch, int v)
            :kind(ch), val(v) {}
};

void error(std::string str) {
    throw std::runtime_error(str);
}
void error(std::string str, char v) {
    throw std::runtime_error(str + v);
}
void keep_window_open() {
    std::cout<<"press any key to continue...\n";
    std::cin.get();
}

std::vector<Digit> digits;

double simple_pow(double d, int exp) {
    double ans = 1;
    for(int i = 0; i < exp; i++) {
        ans *= d;
    }
    return ans;
}



void digitise() {
    std::string str;
    char ch;
    bool decimal = false;

    while(std::cin>>ch) {
        if(isdigit(ch)) {
            int n = (int)ch - '0';
            if(decimal) {
                digits.push_back(Digit('d', n));
                continue;
            }
            digits.push_back(Digit('w', n));
            continue;
        }
        switch(ch) {
            case '.':
                decimal = true;
                continue;
            case ';':
                return;
            default: error("invalid character: ", ch);
        }
    }
}

double valuify() {
    double num = 0;
    int dplace = 1;
    for(int i = 0; i < digits.size(); i++) {
        switch(digits[i].kind) {
            case 'w':
                num *= 10;
                num += digits[i].val;
                continue;
            case 'd':
                num += (double) digits[i].val / (simple_pow(10, dplace++));
                continue;
            default: error("unexpected token for valuify()");
        }
    }
    return num;
}

double readInDouble() {
    double val;
    std::cin>>val;
    return val;
}

char readInExpSign() {
    char ch;
    std::cin>>ch;
    if(ch != '^') error("expected ^");
    return ch;
}

double pow() {
    
    double base = readInDouble();
    char expsign = readInExpSign();
    digitise();
    double expo = valuify();
    double numer = 0;
    double denom = 1;
    double ans = 1;
    
    
    if(!expo) {
        return 1;
    }

    for(int i = 0; i < digits.size(); i++) {
        switch(digits[i].kind) {
            case 'w': {
                numer *= 10;
                numer += digits[i].val;
                continue;
            }
            case 'd': {
                numer *= 10;
                numer += digits[i].val;
                denom *= 10;
            }
        }
    }

    for(int i = 0; i < numer; i++) {
        ans *= base;
    }
    return ans;
}

int main() {
    try {
        
        std::cout<<pow()<<std::endl;


    }
    catch(std::exception& e) {
        std::cerr<<"Exception: "<<e.what()<<std::endl;
        keep_window_open();
    }
    catch(...) {
        std::cerr<<"Unknown exception"<<std::endl;
        keep_window_open();
    }
    return 0;
}