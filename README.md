# Quizzie — Multi-Threaded Client–Server Quiz Application

**Description:**  
Quizzie is a multi-threaded quiz server written in **C**, built using **POSIX sockets, pthreads, and semaphores**. It allows multiple clients to connect simultaneously, add questions, take quizzes, and view scores — all while ensuring thread-safe access to shared resources.

---

## Features
- **Multi-threaded server**: each client runs in its own thread for concurrency.  
- **Thread safety**: implemented reader–writer locks with semaphores.  
- **Client–server communication**: built with POSIX sockets for stable networking.  
- **Quiz functionality**: supports adding questions, running quizzes, and tracking scores.  

---

## Technologies Used
- C programming language  
- POSIX sockets (network communication)  
- Pthreads (multi-threading)  
- Semaphores (synchronization, thread safety)  

---

## How to Run:
Clone the repository:  
   bash
   git clone https://github.com/mutluelifn/Quizzie-multithreaded-server.git
   cd Quizzie-multithreaded-server

** !!Make sure you have the required header files!! **
csapp.h and csapp.c (from the CS:APP library by Bryant & O’Hallaron)
Place them in the same directory as server.c and client.c before compiling.


Compile the server and client:

gcc -o server server.c csapp.c -lpthread
gcc -o client client.c csapp.c


Start the server (replace <port> with any free port, e.g., 8080):
./server <port>


Run the client in a new terminal window:
./client <host> <port>


Example:
./client localhost 8080


## Example Usage:

Option 1: Add a new question
Option 2: Take a quiz
Option 3: Show score
Option 4: Exit

## Technologies Used:

C programming
POSIX sockets (network communication)
Pthreads (multi-threading)
Semaphores (synchronization and thread safety)
CS:APP library (wrapper functions for sockets and concurrency)


## Project Files
server.c : Multi-threaded server logic, synchronization, and file handling
client.c : Client-side interaction and communication with server
csapp.h / csapp.c : Wrapper functions (required to build the project)

## Reference:
CS:APP wrappers are from Computer Systems: A Programmer’s Perspective by Randal Bryant & David O’Hallaron.
