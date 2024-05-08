//
//  main.cpp
//  TuttleExam2Q2
//
//  Created by Anastasia Tuttle  on 3/21/24.
//

#include <iostream>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>

using namespace std;

queue<int> dataQueue;
mutex queueMutex;
condition_variable queueEmpty;
condition_variable queueFull;

class Writer {
public:
    void write(int data) {
        unique_lock<mutex> lock(queueMutex);
        // Wait if the queue is full
        queueFull.wait(lock, []{ return dataQueue.size() < MAX_SIZE; });
        dataQueue.push(data);
        cout<<"Data written: "<<data<<endl;
        lock.unlock();
        queueEmpty.notify_one();
    }
private:
    static const int MAX_SIZE = 5;
};

class Reader {
public:
    void read() {
        unique_lock<mutex> lock(queueMutex);
        // Wait if the queue is empty
        queueEmpty.wait(lock, []{ return !dataQueue.empty(); });
        int data = dataQueue.front();
        dataQueue.pop();
        cout<<"Data read: "<<data<<endl;
        lock.unlock();
        queueFull.notify_one();
    }
};

void writerThreadFunc(Writer& writer) {
    for (int i = 0; i < 10; ++i) {
        writer.write(i);
        this_thread::sleep_for(chrono::milliseconds(500));
    }
}

void readerThreadFunc(Reader& reader) {
    for (int i = 0; i < 10; ++i) {
        reader.read();
        this_thread::sleep_for(chrono::milliseconds(1000));
    }
}

int main() {
    Writer writer;
    Reader reader;

    thread writerThread(writerThreadFunc, ref(writer));
    thread readerThread(readerThreadFunc, ref(reader));

    writerThread.join();
    readerThread.join();

    return 0;
}

