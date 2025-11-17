#ifndef PROCESS_MANAGEMENT_HPP
#define PROCESS_MANAGEMENT_HPP

#include "Task.hpp"
#include <queue>
#include <memory>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <ctime> // <-- 1. THIS LINE IS NEW

class ProcessManagement
{
public:
    ProcessManagement();  // Constructor to launch the thread pool
    ~ProcessManagement(); // Destructor to shut down the thread pool
    
    // This is the "Producer" function, called by the main thread
    bool submitToQueue(std::unique_ptr<Task> task);

private:
    // This is the "Consumer" function that all threads in the pool will run
    void workerLoop();

    std::queue<std::string> taskQueue; // The shared queue
    size_t maxQueueSize = 1000;

    std::mutex queueLock; // Mutex to protect the queue
    
    // Notifies workers when a task is added
    std::condition_variable itemsAvailable;
    
    // Notifies producer when a slot is free
    std::condition_variable slotsAvailable;

    std::mutex printLock; // <-- 2. THIS LINE IS NEW

    std::vector<std::thread> workerThreads; // The thread pool
    
    // A flag to safely stop all threads
    std::atomic<bool> stopProcessing; 
};

#endif
