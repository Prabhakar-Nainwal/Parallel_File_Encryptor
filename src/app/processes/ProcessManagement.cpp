#include <iostream>
#include "ProcessManagement.hpp"
#include "../encryptDecrypt/Cryption.hpp" // For executeCryption

// --- Constructor ---
// Creates the fixed-size thread pool
ProcessManagement::ProcessManagement() : stopProcessing(false) {
    
    // Use the number of hardware cores
    unsigned int numThreads = std::thread::hardware_concurrency();
    if (numThreads == 0) numThreads = 2; // A sensible default

    std::cout << "Launching " << numThreads << " worker threads..." << std::endl;

    // Create and launch all the worker threads
    for (unsigned int i = 0; i < numThreads; ++i) {
        workerThreads.emplace_back(&ProcessManagement::workerLoop, this);
    }
}

// --- Destructor ---
// Gracefully shuts down the thread pool
ProcessManagement::~ProcessManagement() {
    std::cout << "Shutting down threads..." << std::endl;

    // 1. Set the stop flag
    stopProcessing.store(true);

    // 2. Notify ALL threads to wake up and check the flag
    itemsAvailable.notify_all();
    slotsAvailable.notify_all(); 

    // 3. Wait for each thread to finish
    for (std::thread &worker : workerThreads) {
        if (worker.joinable()) {
            worker.join(); // Wait for the thread to exit its loop
        }
    }
    std::cout << "All threads shut down." << std::endl;
}

// --- Producer (Main Thread) ---
// Adds a task to the queue and notifies a worker
bool ProcessManagement::submitToQueue(std::unique_ptr<Task> task) {
    
    // 1. Lock the queue
    std::unique_lock<std::mutex> lock(queueLock);

    // 2. Wait until there is a free slot OR it's time to stop
    slotsAvailable.wait(lock, [this] {
        return (this->taskQueue.size() < this->maxQueueSize) || this->stopProcessing.load();
    });

    // 3. If we woke up because of a stop signal, don't add the task
    if (stopProcessing.load()) return false;

    // 4. Add the task to the queue
    taskQueue.push(task->toString());

    // 5. Unlock the mutex
    lock.unlock(); 

    // 6. Notify ONE waiting worker thread that a task is ready
    itemsAvailable.notify_one();
    
    return true;
}

// --- Consumer (Worker Threads) ---
// This is the function ALL worker threads run, in a loop.
void ProcessManagement::workerLoop() {
    
    while (true) {
        std::string taskStr;

        // --- Start Critical Section (under lock) ---
        {
            // 1. Lock the queue
            std::unique_lock<std::mutex> lock(queueLock);

            // 2. Wait until there is a task OR it's time to stop
            itemsAvailable.wait(lock, [this] {
                return !this->taskQueue.empty() || this->stopProcessing.load();
            });

            // 3. Check if we should stop
            if (stopProcessing.load() && taskQueue.empty()) {
                return; // Exit the loop, which ends the thread
            }

            // 4. Get the task from the queue
            taskStr = taskQueue.front();
            taskQueue.pop();
        
        } // --- End Critical Section (Lock is released here) ---

        // 5. Notify the producer that a slot is now free
        slotsAvailable.notify_one();

        // 6. Do the heavy work OUTSIDE the lock
        if (!taskStr.empty()) {
            
            // --- START TIMING (ctime) ---
            clock_t start = clock();

            executeCryption(taskStr); // <-- Run the task
            
            // --- END TIMING (ctime) ---
            clock_t end = clock();
            
            // Calculate elapsed time in seconds
            double elapsed_secs = double(end - start) / CLOCKS_PER_SEC;

            // --- Safely print the result ---
            {
                // Use the printLock to prevent jumbled output
                std::lock_guard<std::mutex> lock(printLock);
                std::cout << "Task (" << taskStr << ") finished in " 
                          << elapsed_secs << " seconds" << std::endl;
            }
        }
    }
}
