#include <mutex>
#include <atomic>
#include <iostream>
#include <thread>
#include <condition_variable>
#include <unistd.h>
using namespace std;

int val = 0;
mutex m_data;
atomic_bool stop_reading;
condition_variable writer_done;
void reader()
{

    while (1)
    {

        unique_lock<mutex> l(m_data);
        if (stop_reading)
        {
            cout << "reader waiting" << endl;
            writer_done.wait(l);
        }

        cout <<this_thread::get_id()<< " reader working" << endl;
        cout << "data is " << val << endl;
    }
}
void writer()
{
    while (1)
    {
        stop_reading = true;
        cout << "writer working" << endl;
        writer_done.notify_all();
        ++val;
        stop_reading = false;
        sleep(1);
    }
}

int main()
{
    thread W(writer);
    thread R_1(reader);
    thread R_2(reader);
    R_1.join();
    R_2.join();
    W.join();
}