# Effective Modern C++ 阅读笔记

## 1.类型推导

### 条款1：理解模板类型推导

- 引用类型的实参会被当做非引用类型来处理，忽略其引用性
- 对万能引用形参推导时，左值实参会进行特殊处理

```C++
template<typename T>
void f(T&& param);

int x=0;
const int cx=x;
const int& rx=x;
f(x);	//左值，T的类型是int&，param的类型是int&
f(cx);	//左值，T的类型是const int&，param的类型是const int&
f(rx);	//左值，T的类型是const int&，param的类型是const int&
```

- 对按值传递的形参进行推导时，若实参类型中带有const或volatile饰词，则它们还是会被当作去除饰词的类型来处理
- 在模板类型推导中，数组或函数类型的实参会退化成对应的指针，除非它们被用来初始化引用

```C++
template<typename T>
void f(T param);

const char name[] = "josefren";
f(name);	//T的类型被推导为const char*;

```

```C++
template<typename T>
void f(T& param);

const char name[] = "josefren";
f(name);	//T的类型被推导为const char [13];

template<typename T, std::size_t N>
constexpr std::size_t arraySize(T (&)N) noexcept
{
    return N;
}
```

### 条款2：理解auto类型推导

- 与模板类型推导基本一致
- 特例：

```C++
auto x1 = 27;	//int
auto x2(27);	//int
auto x3 = {27};	//std::initializer_list<int>
auto x4{27};	//std::initializer_list<int>

//auto假定用大括号括起的初始化表达式代表一个std::initializer_list，但模板类型推导不会
```

- 在函数返回值或lambda表达式中的形参中使用auto，表示使用模板类型推导而非auto类型推导



### 条款3：理解decltype

```c++
//返回值类型尾序语法【指定返回值类型将在形参列表之后(在"->"之后)】
template<typename Container, typename Index>
auto authAndAccess(Container&& c, Index i)
-> decltype(std::forward<Container>(c)[i])
{
    authenticateUser();
    return std::forward<Container>(c)[i];
}
```

```C++
int x=0;
decltype(x); 	//int
decltype((x));	//int&
```

- 绝大多数情况下，decltype得出变量或表达式的类型而不作任何修改
- 对于类型为T的左值表达式，除非该表达式仅有一个名字，decltype总是得出类型T&
- C++14支持decltype（auto），和auto一样，它会从其初始化表达式出发来推导类型，但是他的类型推导使用的是decltype的规则



### 条款4：掌握查看类型推导结果的方法

- 编译器诊断信息

  - ```c++
    //构造模板类诱发错误消息
    template<typename T>
    class TD;
    
    const int val = 42;
    auto x = val;
    auto y = &val;
    TD<decltype(x)> xType;
    TD<decltype(y)> yType;
    
    ```

- 运行时输出

## 2.auto

### 条款5：优先选用auto，而非显式类型声明

- auto变量必须初始化，且与右值类型匹配，可避免兼容性和效率问题
- auto可能影响程序可读性



### 条款6：当auto推导的类型不符合要求时，使用带显式类型的初始化物习惯用法

- “隐形”的代理类型可以导致auto根据初始化表达式推导出错误的类型
- 带显示类型的初始化物习惯用法强制auto推导出你想要的类型

```C++
vector<bool> b(3);

auto val = static_cast<bool>b[0];	//b[0]为std::vector<bool>::reference,vector对bool类型特化
```



## 3.转向现代C++

### 条款7：在创建对象时之一区分()和{}

- 大括号初始化可以应用的语境最为宽泛，可以阻止隐式窄化类型转换，还对最令人苦恼之解析语法免疫

```C++
//解析语法
Widget w3();	//希望调用构造函数，却声明了函数
```



- 在构造函数重载决议期间，只要有任何可能，大括号初始化物就会与带有std::initializer_list类型的形参相匹配，即使其他重载版本有貌似更匹配的形参表
- 使用小括号与大括号造成结果大相径庭的例子：

```C++
vector<int> v1(10,2);	//10个元素，全为2
vector<int> v2{10,3};	//2个元素，10与3
```

- 在模板内容进行对象创建时，到底用小括号还是大括号会成为一个棘手问题



### 条款8：优先选用nullptr，而非0或NULL

- nullptr优化可读性，强调指针的存在
- 避免在整形和指针类型之间重载



### 条款9：优先选用别名声明，而非typedef

- 别名声明在证明函数指针时可读性更佳

```C++
typedef void (*FP)(int, const string &);	//typedef

using FP = void (*)(int, const string &);	//别名声明，可读性更优
```

- 别名声明可模板化，typedef不可

```C++
template<typename T>
using MyAllocList = std::list<T, MyAlloc<T>>;
```

- 别名声明模板可免去"::type"后缀，且内嵌的typedef引用常被要求加上typename前缀



### 条款10：优先选用限定作用域的枚举类型，而非不限作用域的枚举类型

- C++98风格的枚举类型，现在称为不限范围的枚举类型

```C++
enum Color {black, white};	//C++98

enum class Color {black, white};	//C++11
```



- 限定作用域的枚举类型尽在枚举类型内可见，它们只能通过强制类型转换以转换至其他类型
- 限定作用域的枚举类型和不限范围的枚举类型都支持底层类型指定。
- 限定作用于的枚举类型总是可以进行前置声明，而不限范围的枚举类型却只有在制定了默认底层类型的前提下才可以进行前置声明



### 条款11：优先选用删除函数，而非private未定义函数

```C++
//两者等价
class Example
{
    public:
    	Example(const Example& e) = delete;	//C++先校验可访问性后校验删除状态，故为public
    private:
    	Example();
}
```

- 任何函数可为删除函数

- 删除函数可阻止不应进行的模板具现

```C++
template<typename T>
void processPointer(T* ptr);

template<>
void processPointer(void* )=delete;
template<>
void processPointer(char* )=delete;
```



### 条款12：为意在改写的函数添加override声明

- 成员函数引用饰词使得对于左值和右值对象的处理能够区分开来

```C++
class Widget{
public:
    using DataType = std::Vector<double>;
    DataType& data() &	//左值Widget，返回左值
    {
        return values;
    }
    DataType data() &&	//右值Widget，返回右值
    {
        return std::move(values);
    }
private:
   	DataType values;
};
```

- 添加override声明可确保程序运行符合预期



### 条款13：优先选用const_iterator，而非iterator

- 使用const_iterator强调不进行值的修改
- 在最通用的代码中，优先选用非成员函数版本的cbegin、cend，因为部分容器仅含begin、end



### 条款14：只要函数不会发射异常，就为其加上noexcept声明

- noexcept声明式函数接口的组成部分，意味着调用方可能对它有依赖
- noexcept声明的函数有更多机会得到优化
- noexcept性质对于移动操作、swap、函数释放函数和析构函数最有价值
- 大多数函数都是异常终止的，不具备noexcept性质



### 条款15：只有有可能使用constexpr，就使用它

- constexpr对象
  - 具有const属性
  - 在编译期已知

- constexpr函数

  - 若实参皆为编译期已知，则结果也为编译期已知；否则与普通函数无异，结果为运行期已知

  - ```C++
    //C++11，constexpr函数内只可有一句可执行语句
    constexpr int pow(int base, int exp) noexcept
    {
        return (exp==0? 1 : base * pow(base, exp-1));
    }
    
    //C++14，限制放宽
    constexpr int pow(int base, int exp) noexcept
    {
        auto result = 1;
        for(int i=0; i<exp; i++)
            	result *= base;
        return result;
    }
    ```



### 条款16：保证const成员函数的线程安全性

- const具有只读的语义，需确保在并发时值不变
- 运用std::atomic类型的变量会比运用互斥量提供更好的性能，但前者仅适用对单个变量或内存区域的操作



### 条款17：理解特种成员函数的生成机制

- 默认构造函数：仅当类中不含用户声明的构造函数时生成
- 析构函数：默认为noexcept，仅当类中不含用户声明的构造函数时生成
- 复制构造函数：仅当类中不含用户声明的复制构造函数时生成；若类中声明了移动操作，则默认复制构造函数将被删除；在已经存在复制赋值运算符或析构函数的条件下，仍然生成复制构造函数已经成为了被废弃的行为
- 复制赋值运算符：仅当类中不含用户声明的复制赋值运算符时生成；若类中声明了移动操作，则默认复制构造函数将被删除；在已经存在复制构造函数或析构函数的条件下，仍然生成复制赋值运算符已经成为了被废弃的行为
- 移动构造函数和移动赋值运算符：仅当类中不含用户声明的复制操作、移动操作和析构函数时才生成
- 可用default关键字显式要求编译器生成默认构造函数

```C++
class Example
{
    public:
    	~Example(){...}
    	Example(const &Example rhs) = default;
}
```

- 成员函数模板在任何情况下都不会抑制特种成员函数的生成



## 4.智能指针

### 条款18：使用std::unique_ptr管理具备专属所有权的资源

- std::unique_ptr可转换为std::shared_ptr
- 资源析构可以指定自定义删除器，使用自定义删除器会增加unique_ptr类型的对象尺寸

```C++
auto delInvmt = [](Investment * p)
{
    makeLogEntry(p);
    delete p;
}
std::unique_ptr<Investment, decltype(delInvmt)> pInv(nullptr, delInvmt);
```



### 条款19：使用std::shared_ptr管理具备共享所有权的资源

- std::shared_ptr的尺寸时裸指针的两倍，因为含有一个指涉到控制块的指针
  - 控制块含有：引用计数、弱计数、其他数据（自定义删除器、分配器等）
    - 控制块创建时机：std::make_shared、从unqiue_ptr出发构造一个shared_ptr时、使用裸指针构造shared_ptr时
    - 避免将裸指针传递给shared_ptr构造函数（可能导致同一对象对应多控制块，造成未定义析构行为）
- 引用计数的内存必须动态分配
- 引用计数的递增和递减必须是原子操作
- shared_ptr的自定义析构器与unique_ptr不同

```C++
auto delInvmt = [](Investment * p)
{
    makeLogEntry(p);
    delete p;
}
std::unique_ptr<Investment, decltype(delInvmt)> upInv(nullptr, delInvmt);
std::shared_ptr<Investment> sp(nullptr, delInvmt);
```



### 条款20：对于类似std::shared_ptr但有可能空悬的指针使用std::weak_ptr

- weak_ptr可避免shared_ptr指针环路
- weak_ptr可用于缓存、观察者列表



### 条款21：优先选用std::make_unique和std::make_shared，而非直接使用new

- 可用C++版本：
  - make_unique：C++14起
  - make_shared：C++11起
- make系列函数会把一个任意实参集合完美转发给动态分配内存的对象的构造函数，并返回一个涉及该对象的智能指针
- make系列函数消除了重复代码、改进了异常安全性、并且对于make_shared和allocated_shared而言，目标代码尺寸会更小、速度会更快
- 不适用于make系列函数的场景：
  - 需要定制删除其
  - 期望直接传递大括号初始化物
- 对于shared_ptr不建议使用make系列函数的额外场景：
  - 自定义内存管理的类
  - 内存紧张的系统、非常大的对象
  - 存在比指涉到相同对象的shared_ptr生存期更久的weak_ptr

```C++
auto upw1(std::make_unique<Widget>());		//出现一次对象类别，一次内存分配

std::unique_ptr<Widget> upw2(new Widget);	//出现两次对象类别，增加代码冗余，增加代码缺陷可能性，两次内存分配
```



### 条款22：使用Pimpl习惯用法时，将特殊成员函数的定义放到实现文件中

```C++
//Widget.h中
class Widget {
public:
    Widget();
    ~Widget();  //声明析构函数
    Widget(const Widget&& w);  //声明移动构造函数
    Widget& operator=(const Widget&& w);  //声明移动赋值函数
private:
    class Impl;                 //声明结构体或类
    std::unique_ptr<Impl> pImpl; //声明指涉到它的指针
};


//Widget.cpp中
#include "Widget.h"
#include "Impl.h"  //在实现文件内包含Impl的头文件

Widget::Widget():pImpl(std::make_unique<Impl>()){

}

//在实现文件内使用默认析构及移动操作
Widget::~Widget() = default;
Widget::Widget(const Widget&& w) = default;
Widget& Widget::operator=(const Widget&& w)= default;


//main.cpp
#include "Widget.h"
int main()
{
    Widget w;   //编译通过
}	

```

- 对于采用unique_ptr实现的Pimlp指针，必须在类的头文件中声明特种成员函数，但在cpp文件中实现，即使默认函数实现有着正确行为也必须这样做（原因见下条）
- 若使用shared_ptr则无需遵顼此条款，因为shared_pt于unique_ptr对于自定义析构器的支持不同
  - 对unique_ptr而言，析构器类别是智能指针类型的一部分，这使得编译器产生更小的数据结构以及更快的运行速度，但要求其指涉到的类型必须是完整类型
  - shared_ptr于unique_ptr正好相反



## 5.右值引用、移动语义和完美转发

### 条款23：理解std::move和std::forward

- std::move不进行任何移动，std::forward不进行任何转发，在运行期它们不做任何操作
  - std::move无条件将实参强制转换成右值（不保证返回值具有可移动性）
  - std::forward仅在其实参是通过右值完成初始化时才将实参强制转换成右值

- 针对常量对象执行的移动操作将变换成复制操作

- 理论上可弃用std::forward，只用std::forward，但实际应用于理论相悖
  - std::move之取用一个实参，std::forward需取用一个实参和一个模板类型实参



### 条款24：区分万能引用和右值引用

```C++
//万能引用，必涉及类型推导
template<typename T>
void f(T&& param);
auto&& var2 = var1;

//右值引用
void f(Widget&& param);
Widget&& var2 = Widget();
```

- 万能引用形式固定：T&& 或 auto&&，不可存在const饰词

- 模板内存在T&&的函数形参不一定代表它就是万能引用

  - T可能依赖示例的具现实现

  - ```C++
    template<class T>
    class vector{
        public:
        	void push_back(T&& x);
    }
    
    vector<Widget> v;
    
    template<Widget T>
    class vector{
        public:
        	void push_back(Widget&& x);
    }
    ```



### 条款25：针对右值引用实施std::move,针对万能引用实施std::forward

- 针对右值引用的最后一次使用实施std::move，针对万能引用的最后一次使用实施std::forward
- 若局部对象可能适用于返回值优化（RVO，return value optimization），请勿针对其实施std::move或std::forward
  - RVO标准：编译器若要在一个按值返回的函数里省略对局部对象的复制，则需要满足两个前提条件：1.局部对象类型于函数返回值类型相同；2.返回的就是局部对象本身



### 条款26：避免依万能引用类型进行重载

- 把万能引用作为重载候选类型，几乎总会使它在始料未及的情况下被调用
- 完美转发构造函数的问题尤其严重，因为对于非常量的左值类型而言，它们一般都会形成相对于复制构造函数的更佳匹配，并且还会劫持派生类中对基类的复制和移动构造函数的调用



### 条款27：熟悉依万能引用类型进行重载的替代方案

- 传值

  - 当确定肯定需要复制形参时

- 标签分派

  - ```C++
    template<typename T>
    void logAndAdd(T&& name)
    {
        logAndAddImpl(std::forward<T>(name), std::is_integral<typename std::remove_reference<T>::type>());
        
    }
    
    template<typename T>
    void logAndAddImpl(T&& name, std::false_type)
    {
        ...
    }
    
    template<typename T>
    void logAndAddImpl(int idx, std::true_type)
    {
        ...
    }
    ```

- 使用std::enable_if限制模板
  - ```C++
    class Person{
        public:
        	//限制模板推导条件
        	template<typename T,
        			 typename = typename std::enable_if<!std::is_base_of<Person, typename std::deacy<T>::type>::value>::type>
            explicit Person(T&& n);
    }
    ```



### 条款28：理解引用折叠

- 引用折叠：任一引用为左值引用，则结果为左值引用，否则为右值引用
  - 发生语境：模板实例化、auto‘类型生成、创建和引用typedef与别名声明、decltype



### 条款29：假定移动操作不存在、成本高、未使用

- std::array的数据存储在栈上，移动和复制一样需要线性时间复杂度
- 许多std::string的实现采用了SSO（小型字符串优化）——小型字符串会存储在std::string对象内的某个缓冲区内（栈上），导致移动并不比复制更快

- 若已知类的移动语义支持情况，则无需作以上假定



### 条款30：熟悉完美转发的失败情形

**定义：直接调用目标函数的执行结果与通过转发函数调用目标函数的结果不同，即为完美转发失败**

- 大括号初始化物

  - 形参未声明为std::initializer_list，编译器被禁止从它触发推导类型

  - 解决方法：使用自动变量

  - ```C++
    auto il = {1,2,3};
    fwd(il);
    ```

- 0和NULL用作空指针
  - 解决方法：使用nullptr

- 仅有声明的整形static const成员变量

  - 编译器绕过缺少定义的事实，直接将值填入所有提及处（类似宏定义），但在取地址时将失败（引用与指针本质上相同）
  - 解决方法：提供定义

- 重载的函数名字和模板名字

  - 编译器无法获知所要求的是哪个函数（缺少类型）

  - 解决方法：手动指定函数的重载版本

  - ```C++
    using ProcessFuncTYpe = int(*)(int);
    ProcessFuncTYpe processValPtr = processVal;
    fwd(processValPtr);
    ```

- 位域

  - 位域是由机器字的若干任意部分组成的，无法直接对其取值，故编译器规定”非const引用不得绑定到位域“
  - 解决方法：使用自动变量
