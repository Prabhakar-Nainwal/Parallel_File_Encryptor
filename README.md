# C++ File Encryptor/Decryptor

This project is a C++ application that encrypts or decrypts all files within a specified directory. It uses a simple Caesar cipher and leverages **multithreading** with a **thread pool** to process files concurrently.

The application implements a **Producer-Consumer** pattern, where the main thread "produces" file-processing tasks and a pool of worker threads "consumes" them.

## Features

  * Recursively scans a directory and processes all files.
  * Performs both encryption and decryption.
  * Uses a simple character-based Caesar cipher.
  * Encryption key is loaded from a `.env` file.
  * Uses a **thread pool** (`std::thread`) to handle multiple files concurrently.
  * Implements a **Producer-Consumer** pattern using `std::mutex` and `std::condition_variable` for efficient, thread-safe task queuing.

## Project Structure

  * `main.cpp`: The main entry point. Prompts the user for a directory and action, then submits tasks to the thread pool.
  * `src/app/processes/`:
      * `ProcessManagement.hpp/.cpp`: Manages the **thread pool** and the thread-safe task queue.
      * `Task.hpp`: Defines the `Task` struct, which represents a file operation (encrypt/decrypt).
  * `src/app/encryptDecrypt/`:
      * `Cryption.hpp/.cpp`: Contains the core encryption/decryption logic (`executeCryption`).
  * `src/app/fileHandling/`:
      * `IO.hpp/.cpp`: A utility class for opening and managing file streams.
      * `ReadEnv.cpp`: A utility class to read the encryption key from the `.env` file.
  * `Makefile`: Build script for the project.
  * `.env`: Configuration file to store the encryption key.
  * `makeDirs.py`: A Python script to generate test files.

## How It Works

1.  The main program (`./encrypt_decrypt`) starts and asks the user for a directory path and an action (encrypt/decrypt).
2.  It initializes the `ProcessManagement` class, which creates and launches a **fixed-size thread pool** (typically one worker thread per hardware core). These threads immediately go to sleep, waiting for tasks.
3.  The main thread (the **Producer**) recursively scans the specified directory. For each file, it creates a `Task`.
4.  Each `Task` is serialized into a string (e.g., "path/to/file.txt,ENCRYPT") and submitted to the thread-safe queue via `ProcessManagement::submitToQueue`.
5.  `submitToQueue` notifies *one* of the sleeping worker threads that a task is available.
6.  A worker thread (a **Consumer**) wakes up, locks the queue, pulls one task, and then unlocks the queue.
7.  The worker thread executes the task by calling `executeCryption`, which reads the key from `.env` and performs the in-place Caesar cipher on the file.
8.  After finishing its task, the worker thread loops back to wait for another task. This continues until the main thread has added all files and the program exits.

## Prerequisites

  * A C++17 compliant compiler (e.g., `g++`) that supports the `-pthread` flag.
  * `make`
  * `python3` (to use the test file generation script)

## Setup

1.  **Create `.env` file**:
    Create a file named `.env` in the root of the project.
    ```bash
    touch .env
    ```
2.  **Set Encryption Key**:
    Add a single integer to the `.env` file. This will be your encryption/decryption key.
    ```
    8717
    ```
    *(Note: The application reads this value as a string and converts it to an integer.)*

## Build

To build the application, run `make` from the project's root directory. This will create a single executable:

  * `encrypt_decrypt`: The main application.

<!-- end list -->

```bash
make
```

## Usage

1.  Run the main executable:
    ```bash
    ./encrypt_decrypt
    ```
2.  When prompted, enter the full path to the directory you want to process:
    ```
    Enter the directory path: /path/to/your/files
    ```
3.  Enter the desired action (`encrypt` or `decrypt`):
    ```
    Enter the action (encrypt/decrypt): encrypt
    ```

The application will then use its thread pool to process all files. The program will automatically wait for all tasks to be completed before it exits.

### Generating Test Files (Optional)

A Python script is included to generate a directory named `test` containing 1000 sample text files.

1.  Run the script:
    ```bash
    python3 makeDirs.py
    ```
2.  You can now run the main application on the newly created `test` directory.

## Cleanup

To remove all compiled object files and the executable, run:

```bash
make clean
```
