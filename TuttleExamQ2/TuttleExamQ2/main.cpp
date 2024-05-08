//
//  main.cpp
//  TuttleExamQ2
//
//  Created by Anastasia Tuttle  on 2/21/24.
//

#include <iostream>
#include <fstream>
#include <random>
#include <thread>
#include <chrono>
#include <queue>
#include <vector>
#include <mutex>

using namespace std;

ofstream outputStream("output.txt");
random_device rd{};
mt19937 gen{rd()};
uniform_int_distribution<int> sampleUd(0, 100);
uniform_int_distribution<int> waitUd(0,10);

struct DataSample
{
  int generator;
  int value;
};
queue<DataSample> sampleQueue;
mutex queueMutex;


void simulateInput(int generatorNumber)
{
  // TODO:
  // Add the appropriate code that uses a lock_guard and a mutex to
  // protect the critical section in this function.
  //
  while(true)
  {
    DataSample ds;
    ds.generator = generatorNumber;
    ds.value = sampleUd(gen);
      {
          lock_guard<mutex> lock(queueMutex);
          sampleQueue.push(ds);
      }
    this_thread::sleep_for(chrono::seconds(waitUd(gen)));
  }
};

int main(int argc, char *argv[])
{
  if (!outputStream.is_open())
  {
    cout << "Unable to create output file" << std::endl;
    exit(1);
  }
    // TODO:
    // Create a vector of threads and then write a loop that adds ten
    // threads to that vector with each thread using simuateInput() as the
    // the thread function, and then detaching from the thread.
    // Then make the main thread (here) wait for two seconds and  write an
    // infinite loop that checks to see if there is anything on the
    // sampleQueue queue and write to the output file and standard
    // output.
    //
    // Note: Don't forget to lock the critical section using lock_guard()
    // and a mutex.
    //
    
    //create a vector of threads
    vector<thread>threads;

    cout<<"Creating a vector of threads"<<endl; //output foe personal record
    
    //add ten threads to teh vector, each using simulateInput() as the thread function
    for (int i = 0; i < 10; ++i)
    {
        threads.emplace_back(simulateInput, i +1);
    }
    
    cout<<"Going to sleep for two minutes"<<endl; //output foe personal record
    //sleep for two minutes
    this_thread::sleep_for(chrono::seconds(120));
    
    cout<<"Done sleeping"<<endl; //output foe personal record
    
    
    //infinite loop to process the data
    while (true)
    {
        lock_guard<mutex> lock(queueMutex);
        while (!sampleQueue.empty())
        {
            DataSample ds = sampleQueue.front();
            sampleQueue.pop();
            
            //write to the output fiel
            outputStream<<"Generator: "<<ds.generator<<", Value: "<<ds.value<<endl;
            //write to standard output
            cout<<"Generator: "<<ds.generator<<", Value: "<<ds.value<<endl;
            
        }
    }
    //sleep for a short duration before checking the queue again
    this_thread::sleep_for(chrono::milliseconds(100));


  return 1;

}
