# Effective Modern C++ 阅读笔记

## 1.类型推导

### 条款1：理解模板型别推导

- 引用类型的实参会被当做非引用类型来处理，忽略其引用性
- 对万能引用形参推导时，左值实参会进行特殊处理

```C++
template<typename T>
void f(T&& param);

int x=0;
const int cx=x;
const int& rx=x;
f(x);	//左值，T的型别是int&，param的型别是int&
f(cx);	//左值，T的型别是const int&，param的型别是const int&
f(rx);	//左值，T的型别是const int&，param的型别是const int&
```

- 对按值传递的形参进行推导时，若实参型别中带有const或volatile饰词，则它们还是会被当作去除饰词的型别来处理
- 在模板型别推导中，数组或函数型别的实参会退化成对应的指针，除非它们被用来初始化引用

```C++
template<typename T>
void f(T param);

const char name[] = "josefren";
f(name);	//T的型别被推导为const char*;

```

```C++
template<typename T>
void f(T& param);

const char name[] = "josefren";
f(name);	//T的型别被推导为const char [13];

template<typename T, std::size_t N>
constexpr std::size_t arraySize(T (&)N) noexcept
{
    return N;
}
```

### 条款2：理解auto型别推导

- 与模板型别推导基本一致
- 特例：

```C++
auto x1 = 27;	//int
auto x2(27);	//int
auto x3 = {27};	//std::initializer_list<int>
auto x4{27};	//std::initializer_list<int>

//auto假定用大括号括起的初始化表达式代表一个std::initializer_list，但模板型别推导不会
```

- 在函数返回值或lambda表达式中的形参中使用auto，表示使用模板型别推导而非auto型别推导



### 条款3：理解decltype

```c++
//返回值型别尾序语法【指定返回值型别将在形参列表之后(在"->"之后)】
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

- 绝大多数情况下，decltype得出变量或表达式的型别而不作任何修改
- 对于类型为T的左值表达式，除非该表达式仅有一个名字，decltype总是得出型别T&
- C++14支持decltype（auto），和auto一样，它会从其初始化表达式出发来推导型别，但是他的型别推导使用的是decltype的规则



### 条款4：掌握查看型别推导结果的方法

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

### 条款5：优先选用auto，而非显式型别声明

- auto变量必须初始化，且与右值类型匹配，可避免兼容性和效率问题
- auto可能影响程序可读性



### 条款6：当auto推导的型别不符合要求时，使用带显式型别的初始化物习惯用法

- “隐形”的代理型别可以导致auto根据初始化表达式推导出错误的型别
- 带显示型别的初始化物习惯用法强制auto推导出你想要的型别

```C++
vector<bool> b(3);

auto val = static_cast<bool>b[0];	//b[0]为std::vector<bool>::reference,vector对bool类型特化
```



## 3.转向现代C++

### 条款7：在创建对象时之一区分()和{}

- 大括号初始化可以应用的语境最为宽泛，可以阻止隐式窄化型别转换，还对最令人苦恼之解析语法免疫

```C++
//解析语法
Widget w3();	//希望调用构造函数，却声明了函数
```



- 在构造函数重载决议期间，只要有任何可能，大括号初始化物就会与带有std::initializer_list型别的形参相匹配，即使其他重载版本有貌似更匹配的形参表
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



### 条款10：优先选用限定作用域的枚举型别，而非不限作用域的枚举型别

- C++98风格的枚举型别，现在称为不限范围的枚举型别

```C++
enum Color {black, white};	//C++98

enum class Color {black, white};	//C++11
```



- 限定作用域的枚举型别尽在枚举型别内可见，它们只能通过强制型别转换以转换至其他型别
- 限定作用域的枚举型别和不限范围的枚举型别都支持底层型别指定。
- 限定作用于的枚举型别总是可以进行前置声明，而不限范围的枚举型别却只有在制定了默认底层型别的前提下才可以进行前置声明



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



