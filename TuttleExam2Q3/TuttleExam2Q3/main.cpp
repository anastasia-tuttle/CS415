//
//  main.cpp
//  TuttleExamQ1
//
//  Created by Anastasia Tuttle  on 2/21/24.
//

#include <iostream>
#include <array>
#include <thread>
#include <mutex>
#include <condition_variable>

using namespace std;

array<int, 5> dataArray;
mutex dataMutex;
condition_variable dataEmpty;
condition_variable dataFull;

class Writer {
public:
    void write(int data) {
        unique_lock<mutex> lock(dataMutex);
        // Wait if the array is full
        dataFull.wait(lock, [this] { return dataCount < MAX_SIZE; });
        dataArray[dataCount++] = data;
        cout << "Data written: " << data << endl;
        lock.unlock();
        dataEmpty.notify_one();
    }
    int getDataCount() const {
        return dataCount;
    }
    void decrementDataCount() {
        --dataCount;
    }
private:
    static const int MAX_SIZE = 5;
    int dataCount = 0;
};

class Reader {
public:
    Reader(Writer& writer) : writer(writer) {}
    void read() {
        unique_lock<mutex> lock(dataMutex);
        // Wait if the array is empty
        dataEmpty.wait(lock, [this] { return writer.getDataCount() > 0; });
        int dataCount = writer.getDataCount();
        int data = dataArray[--dataCount];
        writer.decrementDataCount();
        cout << "Data read: " << data << endl;
        lock.unlock();
        dataFull.notify_one();
    }
private:
    Writer& writer;
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
    Reader reader(writer);

    thread writerThread(writerThreadFunc, ref(writer));
    thread readerThread(readerThreadFunc, ref(reader));

    writerThread.join();
    readerThread.join();

    return 0;
}

