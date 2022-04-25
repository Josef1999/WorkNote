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



## 同步操作

### 使用condition_variable

等待条件完成

```C++
#include <condition_variable>
std::condition_variable data_cond;
std::mutex mut;
std::queue<data_chunk> data_queue; 
void data_preparation_thread()
{
    while (more_data_to_prepare())
    {
        data_chunk const data = prepare_data();
        std::lock_guard<std::mutex> lk(mut);
        data_queue.push(data);  
        data_cond.notify_one(); 
    }
}
void data_processing_thread()
{
    while (true)
    {
        std::unique_lock<std::mutex> lk(mut); 
        data_cond.wait(
            lk, []
            { return !data_queue.empty(); }); 
        data_chunk data = data_queue.front();
        data_queue.pop();
        lk.unlock(); // 6
        process(data);
        if (is_last_chunk(data))
            break;
    }
}
```



### 使用future

future可配合async使用

async将返回一个包含函数返回值的future

```C++
#include <future>
#include <iostream>
int find_the_answer_to_ltuae();
void do_other_stuff();
int main()
{
    std::future<int> the_answer = std::async(find_the_answer_to_ltuae);
    do_other_stuff();
    std::cout << "The answer is " << the_answer.get() << std::endl;
}
```

```C++
#include <string>
#include <future>

struct X
{
    void foo(int, std::string const &);
    std::string bar(std::string const &);
};
X x;
auto f1 = std::async(&X::foo, &x, 42, "hello");        // 调用p->foo(42, "hello")，p是指向x的指针 
auto f2 = std::async(&X::bar, x, "goodbye"); // 调用tmpx.bar("goodbye")， tmpx是 x的拷贝副本

struct Y
{
    double operator()(double);
};
Y y;
auto f3 = std::async(Y(), 3.141);         // 调用tmpy(3.141)，tmpy通过Y的移动构造函数得到
auto f4 = std::async(std::ref(y), 2.718); // 调用y(2.718)
X baz(X &);
auto f5 = std::async(baz, std::ref(x)); // 调用baz(x)

class move_only
{
public:
    move_only();
    move_only(move_only &&);
    move_only(move_only const &) = delete;
    move_only &operator=(move_only &&);
    move_only &operator=(move_only const &) = delete;
    void operator()();
};
auto f6 = std::async(move_only()); // 调用tmp()，tmp是通过std::move(move_only()) 构造得到
```

async可传入额外参数

```C++
auto f7 = std::async(std::launch::async, Y(), 1.2);            // 在新线程上执行
auto f8 = std::async(std::launch::deferred, baz, std::ref(x)); // 在wait()或get()调用时执行
auto f9 = std::async(
    std::launch::deferred | std::launch::async,
    baz, std::ref(x)); // 实现选择执行方式
auto f10 = std::async(baz, std::ref(x));
f8.wait(); // 调用延迟函数
```

future可配合packaged_task使用

```C++
#include <deque>
#include <mutex>
#include <future>
#include <thread>
#include <utility>
std::mutex m;
std::deque<std::packaged_task<void()>> tasks;
bool gui_shutdown_message_received();
void get_and_process_gui_message();
void gui_thread() 
{
    while (!gui_shutdown_message_received()) 
    {
        get_and_process_gui_message(); 
        std::packaged_task<void()> task;
        {
            std::lock_guard<std::mutex> lk(m);
            if (tasks.empty()) 
                continue;
            task = std::move(tasks.front());
            tasks.pop_front();
        }
        task(); 
    }
}
std::thread gui_bg_thread(gui_thread);
template <typename Func>
std::future<void> post_task_for_gui_thread(Func f)
{
    std::packaged_task<void()> task(f);        
    std::future<void> res = task.get_future(); 
    std::lock_guard<std::mutex> lk(m);
    tasks.push_back(std::move(task));
    return res;                       
}
```

shared_future可以让多个线程等待同一事件，且它可拷贝，通过让每个线程都拥有shared_future对象来获取结果可确保共享同步结果是安全的。
