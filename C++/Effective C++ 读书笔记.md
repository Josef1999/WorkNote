# Effective C++ 读书笔记

## 条款1：视C++为一个语言联邦

过程procedural，面向对象OOP，函数functional，泛型generic，元编程metaprogramming



## 条款2：尽量以 const、enum、inline 替换 #define

### enum hack

```c++
class Cls{
    private:
    	enum {NUM = 5};
    ...
}
```

1. 仿类内static成员in-class初值设定（预防编译器不允许in-class初值设定）
2. 防止整数常量被指针或引用指向



### template inline 函数

```c++
template<typename T>
inline void func(const T&a, const T&b);
```

1. 效率等价于宏函数
2. 安全性等价于一般函数
3. 可用范围大于宏函数（可写class内的private inline函数）



## 条款3：尽可能使用const

```c++
const char *p; //指针指向常量

char * const p; //指针是常量

const char func();//const函数
const int a;//cosnt变量
class CLS()
{
    void func(); const // const成员函数（不改变类）
}
```





#### const函数

```c++
class CLS{...};
const CLS operator* (const CLS &lhs, const CLS &rhs);
CLS A,B,C;
A*B = C; //在A*B的成果上调用operator=，若非const则通过编译
```

1. 预防错误调用导致的错误

#### nonconst&const成员函数

1. 若仅返回类型不同，可令nonconst成员函数调用const成员函数（const_cast、static_cast）



## 条款4：确定对象被使用前已先被初始化

#### member initialization list 类内成员初始化

```c++
class CLS()
{
    private:
    	int val;
    CLS(const int v):val(v)//member initialization list,正确初始化，最高效
    {}
    CLS(const int v)//赋值，非初始化，首先调用default构造函数为成员初始化，再赋值
    {
        val=v;
    }
}
```

1. 高效，避免出现先初始化再赋值
2. 泛用，可为const、引用初始化



