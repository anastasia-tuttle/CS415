//
//  main.cpp
//  TuttleAsg4
//
//  Created by Anastasia Tuttle  on 3/14/24.
//

#include <iostream>
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <random>
#include <chrono>
#include <iomanip>
#include <sstream>

using namespace std;

// Job structure
struct Job {
    unsigned int id;
    unsigned int projectedRunTime;
    unsigned int executionTime;
    unsigned int idleTime;
    unsigned int priority;
    unsigned int passes;

    Job(unsigned int _id, unsigned int _projectedRunTime, unsigned int _priority)
        : id(_id), projectedRunTime(_projectedRunTime), executionTime(0),
          idleTime(0), priority(_priority), passes(0) {}
};

// Queue Manager class
class QueueManager {
private:
    queue<Job*> foregroundQueue;
    queue<Job*> backgroundQueue;
    queue<Job*> batchQueue;
    mutex mtx;
    condition_variable cv;
    unsigned int quantum;
    unsigned int maxPasses;

public:
    QueueManager(unsigned int _quantum = 200, unsigned int _maxPasses = 2)
        : quantum(_quantum), maxPasses(_maxPasses) {}

    void enter(Job* job) {
        lock_guard<mutex> lock(mtx);
        foregroundQueue.push(job);
        cv.notify_one();
    }

    void reschedule(Job* job) {
        lock_guard<mutex> lock(mtx);
        job->passes++;
        if (job->passes >= maxPasses) {
            if (job->priority == 0) { // Demote to next queue
                backgroundQueue.push(job);
            } else if (job->priority == 1) {
                batchQueue.push(job);
            }
        } else {
            foregroundQueue.push(job);
        }
    }

    Job* schedule() {
        lock_guard<mutex> lock(mtx);
        if (!foregroundQueue.empty()) {
            Job* job = foregroundQueue.front();
            foregroundQueue.pop();
            return job;
        } else if (!backgroundQueue.empty()) {
            Job* job = backgroundQueue.front();
            backgroundQueue.pop();
            return job;
        } else if (!batchQueue.empty()) {
            Job* job = batchQueue.front();
            batchQueue.pop();
            return job;
        }
        return nullptr;
    }

    unsigned int getQuantum() const {
        return quantum;
    }
};

// Print current time in format HH:MM:SS
string getCurrentTime() {
    auto now = chrono::system_clock::now();
    auto now_c = chrono::system_clock::to_time_t(now);
    stringstream ss;
    ss << put_time(localtime(&now_c), "%T");
    return ss.str();
}

// Job Creation Thread
void jobCreationThread(QueueManager& qm) {
    random_device rd;
    mt19937 rng(rd());
    double averageArrival = 3; // 3 jobs per second
    double lambda = 1 / averageArrival;
    exponential_distribution<double> exp(lambda);

    while (true) {
        unsigned int projectedRunTime = 2000 + rng() % 1000;
        unsigned int priority = rng() % 10; // Uniformly generated priority between 0 and 9
        Job* job = new Job(1, projectedRunTime, priority); // Assign a temporary id

        // Print job arrival
        cout << getCurrentTime() << " - Arrival\tJob " << job->id << "\tPriority: " << job->priority<<endl;

        qm.enter(job);
        this_thread::sleep_for(chrono::milliseconds(static_cast<long long>(exp(rng) * 1000)));
    }
}

// Processor Thread
void processorThread(QueueManager& qm, unsigned int totalTime) {
    auto start = chrono::high_resolution_clock::now();
    unsigned int currentTime = 0;

    while (currentTime < totalTime) {
        Job* job = qm.schedule();
        if (job) {
            // Print job scheduled
            cout << getCurrentTime() << " - Scheduled\tJob " << job->id<<endl;

            // Execute job
            unsigned int quantum = qm.getQuantum();
            unsigned int timeToExecute = min(job->projectedRunTime - job->executionTime, quantum);
            this_thread::sleep_for(chrono::milliseconds(timeToExecute));

            // Update job stats
            job->executionTime += timeToExecute;
            if (timeToExecute < quantum)
                job->idleTime += quantum - timeToExecute;

            // Check if job is completed
            if (job->executionTime >= job->projectedRunTime) {
                // Print job completion
                cout << getCurrentTime() << " - Completion\tJob " << job->id<<endl;

                // Delete the completed job
                delete job;
            } else {
                // Reschedule the job
                qm.reschedule(job);
            }
        }

        auto end = chrono::high_resolution_clock::now();
        currentTime = chrono::duration_cast<chrono::seconds>(end - start).count();
    }
}

int main(int argc, char* argv[]) {
    unsigned int totalTime = 300; // Default total simulation time: 5 minutes (300 seconds)
    if (argc > 1) {
        totalTime = atoi(argv[1]);
    }

    QueueManager qm;
    thread jobCreator(jobCreationThread, ref(qm));
    thread processor(processorThread, ref(qm), totalTime);

    jobCreator.join();
    processor.join();

    return 0;
}

