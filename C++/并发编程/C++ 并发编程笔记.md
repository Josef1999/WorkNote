# C++ 并发编程笔记

## 线程管理

### 线程

#### 基础概念

可移动不可复制

#### 注意事项

std::thread::id唯一标识一个线程，可通过thread.get_id()获得指定线程id，可通过std::this_thread::get_id()获得本线程id



### join & detach

#### 基础概念

**join & detach用于线程同异步管理**

thread.join()：等待线程结束运行

thread.detach()：不等待线程结束，任其运行

detach的线程被成为守护线程（daemon threads）

#### 注意事项

join或detach前需判断thread.joinable()



## 共享数据

### 条件竞争

#### 基础概念

不变量遭受破坏时产生条件竞争，如：并发地修改一个独立对象

#### 注意事项

条件竞争难复现，常出现，需对接口精心设计来避免该问题。可通过使用互斥量或无锁编程解决

并发环境下的接口设计与常规接口往往不同



### 互斥量

#### 基础概念

线程通过mutex.lock（）获取互斥量，若已被获取则入睡，互斥量需手动释放mutex.unlock()

#### 使用示例

```c++
#include <mutex>
using namespace std;
mutex m;
mutex m_2;
void basic_use()
{
    m.lock();
    do_something();
    m.unlock();
}
void RAII_use_1()
{
    lock_guard<mutex> guard(m);//构造时lock，析构时unlock
    do_something();
}
void RAII_use_2()
{
    m.lock()
    lock_guard<mutex> guard(m,adopt_lock);//构造时不lock，析构时unlock
    do_something();
}
void multi_use()
{
    lock(m, m_2);
    lock_guard<mutex> guard(m,adopt_lock);
    lock_guard<mutex> guard(m_2,adopt_lock);
    do_something();
}
```



### 死锁

#### 基础概念

线程互相等待，无线程工作

#### 进阶指导

**避免嵌套锁**

尽可能使线程只持有一个锁，若需获取多个锁，通过std::lock

**避免在持有锁时调用外部代码**

外部代码内部逻辑不可知，无法保证结果

**使用固定顺序获取锁**

若std::lock无法使用，则人为指定获取锁的顺序

**使用层次锁结构**

若需对高层上锁必须先对所属底层上锁



### 使用unique_lock

#### 基础概念

unique_lock与lock_guard相比更自由，lock_guard的功能为unique_lock的真子集，lock_guard的性能优于unique_lock

#### 使用示例

```C++
#include <mutex>
using namespace std;
mutex m
void defer_lock()
{
    unique_lock<mutex> u_lock(m, std::defer_lock);//推迟上锁
    lock(u_lock);
}
```



### 锁的粒度

#### 基础概念

线程持有锁的时间过长会增加其他线程等待的时间，从而减少多线程带来的性能收益

在进行耗时操作时应暂时释放锁





### 其他保护共享数据的方式

#### 保护共享数据的初始化过程

使用std::call_once与std::once_flag（once_flag为call_once辅助类）

```C++
void func();
std::once_flag resource_flag;

void thread_func()
{
    std::call_once(resource_flag, func);//多线程环境下仅一个线程执行func，其余线程等待resource_flag
}
```

#### 保护不常更新的数据结构

针对多读少写场景，可使用读写锁（C++17提供std::shared_mutex & std::shared_timed_mutex）