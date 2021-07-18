/*****************************************

The grammar for input is:

Statement:
    Expression
    Print
    Quit

Print:
    ;

Quit:
    q

Expression:
    Term
    Expression + Term
    Expression - Term

Term:
    Primary
    Term * Primary
    Term / Primary

Primary:
    Number
    ( Expression )
    + Primary
    - Primary

Number:
    floating-point-literal

Input comes in from cin through the Token_stream called ts

*****************************************/


#include "std_lib_facilities.h"

class Answer {
    public:
        static double get() {
            return ans;
        }
        static void set(double val) {
            ans = val;
        }
    private:
        static double ans;
};

class Variable {
    public:
        std::string name;
        double value;
        Variable(std::string n, double v) :name(n), value(v) {}
};

vector<Variable> var_table; 

double getValue(std::string s) {
    for(Variable var : var_table) {
        if(var.name == s)
            return var.value;
    }
    throw std::runtime_error("get: undefined variable");
}
void setValue(std::string s, double d) {
    for(Variable var : var_table) {
        if(var.name == s)
            var.value = d;
    }
}
class Token {
    public:
        char kind;
        double value;
        string name;
    
    Token(char ch)
        :kind(ch) {}
    Token(char ch, double val)
        :kind(ch), value(val) {}
    Token(char ch, string n) :kind(ch), name(n) {}
};

const char number = '8';
const char quit = 'q';
const char print = ';';
const char let = 'L';
const char name = 'a';
const std::string prompt = "> ";
const std::string result = "= ";
const std::string declkey = "let";

class Token_stream {
    public:
        
        Token get();
        void putback(Token t);
        void ignore(char c);
    private:
        bool full { false };
        Token buffer {'0'};
};

void Token_stream::putback(Token t) {
    if(full) throw std::runtime_error("putback() into a full buffer");
    buffer = t;
    full = true;
};

Token Token_stream::get() {
    if(full) {
        full = false;
        return buffer;
    }

    char ch;
    std::cin>>ch;

    switch(ch) {
        case '+':
        case '-':
        case '*':
        case '/':
        case '!':
        case '%':
        case '(':
        case ')':
        case '{':
        case '}':
        case print:
        case quit:
            return Token(ch);
        case '0': case '1': case '2': case '3': 
        case '4': case '5': case '6': case '7': 
        case '8': case '9': case '.': 
            {
                std::cin.putback(ch);
                double val;
                std::cin>>val;
                return Token(number, val);
            }
        default:
            if(isalpha(ch)) {
                cin.putback(ch);
                string s;
                cin>>s;
                if(s==declkey) return Token(let);
                return Token(name, s);
            }
            throw std::runtime_error("invalid token");

    }
};

void Token_stream::ignore(char c) {
    if(full && c == buffer.kind) {
        full = false;
        return;
    }

    char ch = 0;
    while(std::cin>>ch)
        if(ch == c) return;
}

Token_stream ts;
double Answer::ans = 0;
double expression();

bool is_declared(std::string var) {
    for(int i = 0; i < var_table.size(); i++)
        if(var_table[i].name == var) return true;
    return false;
}
double define_name(std::string var, double val) {
    if(is_declared(var)) throw std::runtime_error("declared already");
    var_table.push_back(Variable(var, val));
    return val;
}

double primary() {
    Token t = ts.get();
    switch(t.kind) {
        case '(':
            {
                double d = expression();
                t = ts.get();
                if(t.kind != ')') throw std::runtime_error("')' expected");
                return d;
            }
        case '{':
        {
            double d = expression();
            t = ts.get();
            if(t.kind != '}') throw std::runtime_error("'}' expected");
            return d;
        }
        case number:
            return t.value;
        case '-': 
            return - primary();
        case '+':
            return primary();
        default:
            throw std::runtime_error("primary expected");
    }
}

double secondary() {
    double left = primary();
    Token t = ts.get();
    if(t.kind == '!') {
        int i = left;
        if(i != left) throw std::runtime_error("expected integer before '!'");
        left = 1; 
        for(; i > 0; i--)
            left *= i;
        return left;
    }
    else 
        ts.putback(t);
        return left;
}

double term() {
    double left = secondary();
    Token t = ts.get();

    while(true) {
        switch(t.kind) {
            case '*':
                left *= secondary();
                t = ts.get();
                break;
            case '/':
            {   double d = secondary();
                if(d == 0) throw std::runtime_error("divide by zero");
                left /= d;
                t = ts.get();
                break;
            }
            case '%':
                {
                int i1 = narrow_cast<int>(left);
                int i2 = narrow_cast<int>(secondary());
                left = i1 % i2;
                t = ts.get();
                break;
                }

            default:
                ts.putback(t);
                return left;
        }
    }
}

double expression() {
    double left = term();
    Token t = ts.get();
    while(true) {
        switch (t.kind)
        {
        case '+':
            left += term();
            t = ts.get();
            break;
        case '-':
            left -= term();
            t = ts.get();
            break;
        
        default:
            ts.putback(t);
            return left;
        }
    }
}

double declaration() {
    Token t = ts.get();
    if(t.kind != name) throw std::runtime_error("name expected in declaration");
    std::string var_name = t.name;

    Token t2 = ts.get();
    if(t2.kind != '=') throw std::runtime_error("= expected in declaration");

    double d = expression();
    define_name(var_name, d);
    return d;
}
double statement() {
    Token t = ts.get();
    switch(t.kind) {
        case let:
            return declaration();
        default:
            ts.putback(t);
            return expression();
    }
}
void clean_up_mess() {
    ts.ignore(print);
}
void calculate() {
    while(std::cin) {
        try {
            std::cout<<prompt;
            Token t = ts.get();
            while (t.kind == print) t = ts.get();
            if(t.kind == quit) {
                return;
            }
            ts.putback(t);
            std::cout<<result<<statement()<<std::endl; 
        }
        catch (std::exception& e) {
            std::cerr<<e.what()<<std::endl;
            clean_up_mess();
        }
    }
}



int main() {
    try {
        calculate();
        keep_window_open();
        return 0;
    }
    catch (std::exception& e) {
        std::cerr<<"Exception: "<<e.what()<<std::endl;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        keep_window_open();
        return 1;
    }
    catch(...) {
        std::cerr<<"Unknown exception\n";
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        keep_window_open();
        return 2;
    }
    return 0;
}