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



## 条款5：了解C++默默编写并调用哪些函数

1. 编译器默默声明：default构造函数、copy构造函数、copy assignment操作符、析构函数

2. 析构函数会自动调用其non-static成员变量的析构函数

## 条款6：若不想使用编译器自动生成得函数，就该明确拒绝

```c++
class CLS{
    public:
    	CLS(const CLS&) = delete;
    private:
    	CLS& operator=(const CLS&);
}
```

```c++
class Uncopyable{
	protected:
    	Uncopyable(){}
    	~Uncopyable(){}
    private:
    	Uncopyable(const Uncopyable&);
    	Uncopyable& operator=(const Uncopyable&);
}
class CLS: private Uncopyable{
    ...
}
```

1. 声明阻止编译器自动生成，private阻止外部调用。
2. C++11特性：delete关键词，阻止外部调用
3. 继承Uncopyable阻止编译器生成版调用base class对应函数，因为他们是private得



## 条款7：为多态基类声明virtual析构函数

1. 多态基类的析构函数应被申明为virtual，保证基类指针指向的子类对象能被正确释放。



## 条款8：别让异常逃离析构函数

1. 析构函数若吐出异常可能导致程序发生不明确行为，可创建接口给外部处理错误的机会。



## 条款9：绝不在构造和析构过程中调用virtual函数（直接or间接）

1. 存在继承关系的类从base到derived依次构造，期望调用的virtual函数可能尚未被构造，导致调用了父类的virtual函数。

2. 可通过令derived class将必要的构造信息向上传递至base class 构造函数来实现相关需求



## 条款10：令operator=返回一个reference to this*

```c++
int x,y,z;
x=y=z=2; //赋值连锁形式
```

1. 为实现"连锁赋值"，赋值操作符需返回一个reference 指向操作符的左侧实参。



## 条款11：在operator=中处理“自我赋值”

```c++
//潜在的自我赋值（合法），由别名（pointer,reference）导致
a[i]=a[j];
*px=*py;
```

```c++
class CLS()
{
    void swap(CLS &rhs);
    CLS& operator=(const CLS& rhs)//copy and swap
    {
        CLS temp(rhs);
        swap(temp);
        return *this;
    }
}
```

1. operator=中需确保自我复制正确运行（delete、new的先后顺序，引入if语句判断两个类是否相同会降低性能）
2. 除手工排列语句，亦可使用copy and swap技术



## 条款12：复制对象时勿忘其每一个成分

copy构造函数应确保copy所有对象内成员变量&所有base class成员变量（调用对应构造函数）



## 条款13：以对象管理资源

RAII，使用智能指针确保资源获取与释放（预防业务逻辑变动or代码维护出错）



## 条款14：在资源管理类中小心coping行为

面对coping行为：

1. 禁止复制 
2. 对底层资源“引用计数” 
3. 复制底层资源
4. 转移底部资源所有权



## 条款15：在资源管理类中提供对原始资源的访问

提供get函数或显式类型转换（部分老旧api不支持智能指针，借此提高泛用性）



## 条款16：成对使用new和delete时要采取相同形式

```c++
int *v= new int;
delete v;
int *a= new int[10];
delete []a;
```



## 条款17：以独立语句将newed对象置入智能指针

```c++
int priority();
void process(shared_ptr<CLS> pc, int pri);

process(new CLS,priority());//可能先于shared_ptr构造函数前调用priority（取决于编译器），存在资源泄露可能
```



## 条款40：明智而审慎地使用多重继承

多重继承的问题：菱形继承（可用virtual继承解决多基类，但造成额外空间&性能开销）；不同class内的相同名称、

多重继承应用场景：

```c++
class IPerson{...}//含纯虚函数的接口类
class PersonInfo{...}//有若干可复用函数的类
class CPerson: public IPerson,private PersonInfo{...}//pulbic继承接口，private继承实现
```

virutal继承建议：

1. 尽可能不使用virtual继承
2. 尽可能避免在virtual base class中存放数据



# 7.模板与泛型编程

## 条款41：了解隐式接口和编译器多态

```c++
//假定w.normalize()为虚函数
void func(Widget &w)
{
    w.normalize();//显式接口，运行期多态
}
```

```c++
//T类型需含有normalize接口
//类似python
//def func(ins):
//	ins.normalize()
template<typename T>
void func(T &w)
{
    w.normalize();//隐式接口，编译期多态
}
```

编译器多态vs运行期多态 ≈ 调用哪个重载函数vs绑定哪个virtual函数

显式接口vs隐式接口 ≈ 先实现接口再调用（函数适应类）vs先调用接口再实现（类适应函数）



## 条款42：了解typename的双重意义

```c++
template<class T>class Widget;
template<typename T>class Widget;
//大部分情况下等价
template<typename T>
void func(const T& var)
{
    T::const_iterator *iter;//嵌套从属名称
    int num = 0;//非从属名称
}
//嵌套从属名称可能导致解析困难（二义性）
//例：T含有命名为const_iterator的static变量，存在名为iter的全局变量
//解析器默认嵌套从属名称不是类型，可用typename显式标注：
//typename T::const_iterator *iter;
```

从属名称（dependent names）：template中出现的依赖于某个template参数的名称

嵌套从属名称：在class内呈嵌套状的从属名称

非从属名称：不依赖任何template参数的名称

