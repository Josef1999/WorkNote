# More Effective C++ 读书笔记

## 基础议题

### 条款1：指针与引用的区别

**区别：**

1. 引用不可指向空置，指针可【故指针需在使用前测试合法性】
2. 指针可被重新赋值，引用不可

**使用依据：**

1. 是否存在不指向任何对象的可能？
2. 是否存在指向不同对象的可能？



### 条款2：尽量使用C++风格的类型转换

**原因：**

1. C风格的类型转换过于自由
2. C风格的类型转换难以识别



### 条款3：不要对数组使用多态

**原因：**

1. 数组元素间的地址相隔与其类型相同，多态会导致地址出错



### 条款4：避免无用的缺省构造函数

**原因：**

1. 构造函数蕴含进行了有意义初始化的保证，否则将引入一系列对数据有效性检测的代码



## 运算符

### 条款5：谨慎定义类型转换函数

**原因：**

1. 编译器会在一切可能进行隐式类型转换的地方进行转换

**对策：**

	1. 使用 explicit 关键字
	2. 不定义类型转换函数，转而定义成员函数



### 条款6：自增、自减操作符前缀形式与后缀形式的区别

```c++
class Exp{
public:
	Exp& operator++();//前缀++
    const Exp operator++(int);//后缀[const预防连续两次后缀++]
    //C++规定后缀形式有一个int类型参数，当函数被调用时编译器传递一个0作为int参数的值给该函数
}
```



### 条款7：不要重载"&&","||",或","

**原因：**

1. 函数调用与短路求值相冲突
2. 逗号操作符的运算顺序无法被模仿

**原则：**

- 重载操作符以提升可读性而非迷惑读者



### 条款8：理解各种不同含义的new和delete

- new操作符（new operator）：先分配内存（operator new），再调用构造函数初始化对象

```C++
void* operator new(size_t size); // 函数operator new的通常声明
//一般不直接调用operator new
```

- placement new（特殊的operator new）：在已分配的内存上初始化对象

```c++
char *buffer = new char[100];
Cls *p = new (buffer) Cls();
```



## 异常

### 条款9：使用析构函数防止资源泄露

- 异常可能导致资源泄露，使用智能指针简化资源管理



### 条款10：在构造函数中防止资源泄露

- 若构造过程中抛出异常，则无法释放已分配资源，使用智能指针简化资源管理



### 条款11：禁止异常信息传递到析构函数外

**原因：**

1. 防止terminate被调用（析构时异常可能处于激活状态，若再次抛出异常会调用terminate终止程序）
2. 确保析构函数符合预期（正确释放所拥有资源）



### 条款12：理解“抛出一个异常”与“传递一个参数”或“调用一个虚函数”间的差异

- 所有异常捕获都将进行拷贝操作（捕获对象有因超出作用域被析构的可能，故必须复制）【抛出异常运行速度慢于参数传递】
- 拷贝异常对象静态类型
- catch子句不会进行隐式类型转换（例：不捕获int异常，只捕获double）
- catch (cosnt void *) 可捕获任何类型的指针类型异常
- 一个用来捕获基类的catch子句也可以处理派生类类型的异常



### 条款13：通过引用捕获异常

**原因：**

- 通过指针捕获异常：指针可能超出作用域
- 通过值捕获异常：无意义的额外拷贝开销&派生类行为被切除（被转为基类）



### 条款14：审慎使用异常规格（异常声明列表）

**示例：**

```C++
void func() ;	//可能抛出异常
void throw_func() throw(char,int);	//只能抛出char或int类型的异常
void no_throw_func() throw();	//不抛出异常
```

**原因：**

```c++
void throw_func() throw(char,int)
{
    func();	//func可能抛出int、char以外异常，但编译器被禁止拒绝这种调用方式
}
```

- 可能存在违背异常规则的情况
  - 随后自动调用特殊函数unexpected【缺省行为：terminate】

- 无法确认模板类型参数会抛出什么样的异常



### 条款15：了解异常处理的系统开销

- 异常处理自带开销（程序运行时记录相关信息）【程序中不使用异常时可优化这部分开销】
- try、异常规格等将增大代码体积并减缓运行速度



## 效率

### 条款16：牢记80-20准则

- 软件整体的性能取决于代码组成中的一小部分
- 胡乱猜测性能瓶颈无意义，应该使用性能分析工具寻找瓶颈



### 条款17：考虑使用lazy evaulation（懒惰计数法）

- 尽可能减少运算

  - 使用引用计数，减少拷贝

  - 区别对待读取和写入

  - 懒惰提取，只在需要时才初始化载入数据

  - ```c++
    class LargeObj{
        public:
        	const int field1()
            {
                if(fiedl1value==null)
                {
                    getField1Value();
                }
                return *fiedl1value;
            }
        private:
        	mutable int *fiedl1value;//声明mutable后可在任意函数内修改值
    }
    ```

  - 懒惰表达式计算（例：记录矩阵运算的表达式，直到使用该矩阵时再求值）



### 条款18：分期摊还期望的计算

- 使用缓存来存储可能常被调用的函数结果（min、max、avg）
- 预分配空间，预防空间不足（参考vector的扩容机制）



### 条款19：理解临时对象的来源

- 参数需进行类型转换（非常量引用不会创建临时对象，因为可能修改参数，而不希望修改临时对象）
- 函数返回对象时



### 条款20：协助完成返回值优化

- 函数返回对象时会为临时对象调用构造函数&析构函数
  - 部分函数必须返回对象（例：乘法运算符等）

- 可通过返回constructor argument而非局部对象进行优化

```c++
const Rational operator*(const Rational& lhs, const Rational& rhs)
{
    return Rational(lhs.real*rhs.real, lhs.unreal*rhs.unreal);//编译器将优化不出现的临时对象
}
```



### 条款21：通过重载避免隐式类型转换

**原因：**

隐式类型转换带来创建销毁临时对象的开销，可通过重载运算符避免

- C++规则：重载的operator必须带有一个用户自定类型参数，避免用户改变预定义操作



### 条款22：考虑用运算符的赋值形式（op=）取代其单独形式（op）

- 运算符的单独形式推荐通过赋值形式实现

```c++
class Rational
{
    public:
    	Rational& operator +=(const Rational& rhs);
    	const Rational operator +(const Rational& lhs, const Rational& rhs)
        {
            return Rational(lhs) += rhs;	//返回值优化
        }
            
}
```



### 条款23：考虑变更程序库

- 效率、扩展性、直观性、功能性不可兼得，选择最适合的程序库



### 条款24：理解虚函数、多继承、虚基类和RTTI所需的代价

- 大部分编译器通过vtbl（virtual table）、vptr（virtual table pointers）实现虚函数
  - vtbl通常为函数指针数组，存放所有虚函数指针，vptr指向vtbl



### 条款25：将构造函数和非成员函数虚拟化

- 虚拟构造函数类似工厂模式
- 虚拟非成员函数：**编写一个虚拟函数来完成工作，然后再写一个非虚拟函数，它什么不做只调用这个虚函数**

``` c++
class NLComponent
{
public:
    virtual ostream& print(ostream& s) const = 0;
    ...
};
class TextBlock:public NLComponent
{
public:
    virtual ostream& print(ostream& s) const;
    ...
};
class Graphic:public NLComponent
{
public:
    virtual ostream& print(ostream& s) const;
    ...
};
inline ostream& operator<<(ostream& s,const NLComponent& c)
{
    return c.print(s);
}
```



### 条款26：限制某个类所能产生的对象数量

- 参照单例实现方式

```c++
template<class BeingCounted>
class Counted	//计数类
{
public:
    class TooManyObjects{};
    static int objectCount()
    {
        return numObjects;
    }
protected:
    Counted();
    Counted(const Counted& rhs);
    ~Counted()
    {
        --numObjects;
    }
private:
    static int numObjects;
    static const size_t maxObjects;
    void init();
};

template<class BeingCounted>
Counted<BeingCounted>::Counted()
{
    init();
}

template<BeingCounted>
Counted<BeingCounted>::Counted(const Counted<BeingCounted>&)
{
    init();
}

template<class BeingCounted>
void Counted<BeingCounted>::init()
{
    if(numObjects >= maxObjects)
        throw TooManyObjects();
    ++numObjects;
}

template<class BeingCounted>
int Counted<BeingCounted>::numObjects = 0;


class Printer::private Counted<Printer>
{
public:
    static Printer* makePrinter();
    static Printer* makePrinter(const Printer& rhs);
    ~Printer();
    void submitJob(const PrintJob& job);
    void reset();
    void performSelfTest();
    ...
    using Counted<Printer>::objectCount;
    using Counted<Printer>::TooManyObjects;
private:
    Printer();
    Printer(const Printer& rhs);
};

const size_t Counted<Printer>::maxObjects = 10;
```



### 条款27：要求或禁止在堆中产生对象

- 禁止在栈中建立对象
  - 将析构函数声明为private或protected（private将导致无法继承），引入一个专用的伪析构函数
- 禁止在堆中建立对象
  - 将operator new、operator delete 声明为private（派生类若想允许在堆中建立对象，需要再声明其为public）



### 条款28：智能指针

- 见stl



### 条款29：引用计数

