# Effective C++ 读书笔记

# 1.让自己习惯C++

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



# 

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



# 2.构造/析构/赋值运算

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

# 3.资源管理

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

# 4.设计与声明

## 条款18：让接口容易被正确使用，不易被误用

1. 接口一致性
2. 不要求用户必须记得做某事（如：释放对象）



## 条款19：设计class犹如设计type

1. 对象如何被创建销毁
2. 对象的初始化和赋值有什么差别
3. 对象如果被passed by value，意味着什么
4. 什么是新type的合法值
5. 新type所需的继承关系
6. 新type需要什么样的转换
7. 什么样的操作符和函数对新type是合理的
8. 什么样的标准函数应该驳回
9. 谁该取用新type的成员
10. 什么是新type的未声明接口
11. 新type有多一般化
12. 真的需要一个新type吗



## 条款20：宁以pass-by-reference-to-const替换pass-by-value

1. pass-by-value调用构造函数，造成额外性能开销
2. pass-by-reference-to-const在不修改对象的前提下，确保无额外性能开销
3. pass-by-value会导致对象切割（派生类被转换为基类，未能正确表现多态）



## 条款21：必须返回对象时，别妄想返回其reference

1. 对象可能被提前销毁（local 变量）

```c++
inline const CLS operator *(const CLS& lhs, const CLS& rhs)
{
    return CLS(lhs.v*rhs.v);//此函数无法返回引用，必须返回新对象
} 
```



## 条款22：将成员变量声明为private

1. 保持一致性
2. 精准控制成员变量的读写
3. 封装，可随时改变接口下的具体实现
4. public或protected成员变量若改变可能大范围破坏代码



## 条款23：宁以non-member、non-friend替换member函数

1. 更好的封装性（不增加能够访问class内private成分的函数数量）
2. 更好的包裹弹性
3. 更好的扩充性



## 条款24：若所有参数皆需类型转换，请为此采用non-member函数



## 条款25：考虑写出一个不抛异常的swap函数

1. stl默认的swap函数在某些情况下非最优解（pimpl技术实现的类），需特化swap

   ```c++
   namespace std{
       template<>//全特化模板
       void swap<CLS>(CLS &A, CLS &B)
       {
           swap(A.pimpl,B.pimpl);
       }
   }
   ```

2. 默认swap效率不足时，尝试如下选项：
   1. 提供一个public swap成员函数（no exception）
   2. 在class或template所在命名空间提供一个non-member swap，令它调用上述swap成员函数
   3. 如果在编写class（非class template），为class特化std::swap，令它调用上述swap成员函数

# 5.实现

## 条款26：尽可能延后变量定义式的出现时间

1. 变量的构造与析构皆有开销，若变量未被使用则使得这些开销无意义（如：因异常提前退出）

2. ```c++
   //变量定义于循环外
   Widget w;
   for(int i=0;i<n;i++)
       w=...;
   
   //变量定义于循环内
   for(int i=0;i<n;i++)
       Widget w=...;
   
   ```

   循环外：1 构造 + 1 析构 + n 赋值

   循环内：n 构造 + n 析构

   若赋值成本<构造+析构成本，且性能高度敏感，选择变量定义于循环外



## 条款27：尽量少做转型动作

1. 转型具有风险，可能引入不安全、无意义的操作

2. 单一对象可能拥有一个以上的地址（基类指针指向派生类指针）【多发于多重继承，偶发于单一继承】

   ```c++
   class B1
   {
   	int i;
   };
   
   class B2
   {
   	int i;
   };
   
   class D : public B1, public B2
   {
   	int i;
   };
   
   int main()
   {
   	D d;
   
   	D* pD = &d;
   	B2* pB2 = &d;
   
   	cout << pD << endl;
   	cout << pB2 << endl;
   
   	return 0;
   }
   ```

3. dynamic_cast 向下转换性能消耗巨大，减少使用



## 条款28：避免使用handles指向对象内部成分

1. 破坏封装性
2. 增加用户错误操作的可能性



## 条款29：为“异常安全”而努力是值得的

1. 异常安全：

   - 不泄露任何资源

   - 不允许数据败坏

2. 异常安全函数提供以下三个保证之一：

   - 基本承诺：若异常抛出，程序内任何事物仍处于有效状态，没有对象或数据结构因此败坏，然而程序的现实状态可能不可预料
   - 强烈保证：异常抛出后程序状态不改变，函数成功就是完全成功，函数失败就恢复至调用前状态
   - 不抛掷保证：承诺绝不抛出异常

3. “强烈保证”往往能够以copy-and-swap实现，但不一定具有现实意义（额外开销）

4. 异常安全保证等级遵循水桶原则（取所有调用函数的最低等级）



## 条款30：透彻了解inlining的里里外外

1. inline≈宏函数（在调用处以函数本体替换之），可能增大目标代码大小【代码膨胀】，造成程序体积太大从而降低缓存命中率
2. 将inline用于小型、频繁调用的函数身上，提升效率且减少代码膨胀的影响
3. templates是否需要inline根据实际需求确定，不盲目inline头文件内的templates
4. inline函数通常被置于头文件内，大部分情况下在编译过程中进行inlining
5. 复杂的inline函数可能被编译器拒绝（若无法inline则给出警告信息）



## 条款31：将文件间的编译依存关系降至最低

1. 以“声明的依存性”替换“定义的依存性”

   ```c++
   class Date;
   class Address;
   class PersonImpl;
   
   class Person{
   public:
       Person(const Date& birthday, const Address& addr);
   private:
       std::shared_ptr<PersonImpl> pImpl;
   }
   ```

2. 使用pImpl技术

# 6.继承与面向对象设计

## 条款32：确定你的public继承塑模出is-a关系

1. 基类比派生类更具一般化的概念，派生类比基类更具特殊化的概念（所有可施行于基类的皆可施行于派生类）



## 条款33：避免遮掩继承而来的名称

1. 派生类继承基类重载函数

   ```c++
   
   class B
   {
   public:
   	virtual void mf1() = 0;
   	virtual void mf1(int a);
   	void mf3();
   	void mf3(int a);
   };
   
   
   class D : public B
   {
   public:
   	using B::mf1;
   	using B::mf3;
   	virtual void mf1();
   	void mf3();
   };
   ```

2. 派生类选择继承基类重载函数：转交函数

   ```c++
   class B
   {
   public:
   	virtual void mf1() = 0;
   	virtual void mf1(int a);
   };
   
   
   class D : private B
   {
   public:
   	virtual void mf1()
       {
           B::mf1();
       }
   };
   ```



## 条款34：区分接口继承和实现继承

1. public继承分为两部分：接口继承、实现继承
2. pure virtual函数只含接口继承



## 条款35：考虑virtual函数以外的其他选择

NVI(non-virtual interface),函数指针成员变量



## 条款36：绝不重新定义继承而来的non-virtual函数

1. 破坏多态性（低层屏蔽高层，多态正确性受影响）



## 条款37：绝不重新定义继承而来的缺省参数值

1. 缺省参数值是静态绑定，virtual函数是动态绑定（实际运行结果会出乎意料）



## 条款38：通过复合塑模出has-a或“根据某物实现出”

1. 区分is-a与“根据某物实现出”（is inplemented in terms of）

```c++
class SET: public std::list<int>{} //is a

class SET:{
    private:
    	std::list<int> l;
}//is inplemented in terms of
```



## 条款39：明智而审慎地使用private继承

1. private继承意味着只有实现部分被继承，接口部分被略去（无法访问）
2. priavte继承意味is-inplemented-in-terms-of

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



