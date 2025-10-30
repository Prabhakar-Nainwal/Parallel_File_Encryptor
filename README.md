
# C++ File Encryptor/Decryptor

This project is a C++ application that encrypts or decrypts all files within a specified directory. It uses a simple Caesar cipher and leverages multi-processing with shared memory and semaphores to process files concurrently.

## Features

  * Recursively scans a directory and processes all files.
  * Performs both encryption and decryption.
  * Uses a simple character-based Caesar cipher.
  * Encryption key is loaded from a `.env` file.
  * Uses multi-processing (`fork`) to handle multiple files concurrently.
  * Manages tasks between processes using a shared memory queue (`mmap`, `shm_open`) and semaphores.

## Project Structure

  * `main.cpp`: The main entry point. Prompts the user for a directory and action, then submits tasks.
  * `src/app/processes/`:
      * `ProcessManagement.hpp/.cpp`: Manages the shared memory task queue and process forking.
      * `Task.hpp`: Defines the `Task` struct, which represents a file operation (encrypt/decrypt).
  * `src/app/encryptDecrypt/`:
      * `Cryption.hpp/.cpp`: Contains the core encryption/decryption logic (`executeCryption`).
      * `CryptionMain.cpp`: A separate executable for testing the cryption logic directly (not used by the main program).
  * `src/app/fileHandling/`:
      * `IO.hpp/.cpp`: A utility class for opening and managing file streams.
      * `ReadEnv.cpp`: A utility class to read the encryption key from the `.env` file.
  * `Makefile`: Build script for the project.
  * `.env`: Configuration file to store the encryption key.
  * `makeDirs.py`: A Python script to generate test files.

## How It Works

1.  The main program (`./encrypt_decrypt`) starts and asks the user for a directory path and an action (encrypt/decrypt).
2.  It initializes shared memory and semaphores for a task queue.
3.  It iterates through the specified directory. For each file, it creates a `Task`.
4.  Each `Task` is serialized into a string (e.g., "path/to/file.txt,ENCRYPT") and submitted to the shared memory queue via `ProcessManagement::submitToQueue`.
5.  For every submitted task, `submitToQueue` `fork()`s a new child process.
6.  The child process immediately calls `executeTask`.
7.  `executeTask` waits for a task to be available in the shared memory queue (using semaphores), dequeues it, and passes the task string to `executeCryption`.
8.  `executeCryption` deserializes the task string, reads the encryption key from the `.env` file, and performs an in-place Caesar cipher (add/subtract key) on the file's content.

## Prerequisites

  * A C++17 compliant compiler (e.g., `g++`)
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

To build the application, run `make` from the project's root directory. This will create two executables:

  * `encrypt_decrypt`: The main application.
  * `cryption`: A standalone utility for testing the cryption function.

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

The application will then fork processes to handle the files in that directory.

### Generating Test Files (Optional)

A Python script is included to generate a directory named `test` containing 1000 sample text files.

1.  Run the script:
    ```bash
    python3 makeDirs.py
    ```
2.  You can now run the main application on the newly created `test` directory.

## Cleanup

To remove all compiled object files and executables, run:

```bash
make clean
```