# Client-Server-with-Named-Pipes
Named pipes to transfer data and semaphores for synchronization. For Windows.

This project made for BSUIR, VMSIS, System Software task.

If-else made for parent (server) and child (client) processes. There two different processes, but code is written in one file. 
Breakpoint will work only for server.

You can add data with any length, but pipe works with bytes, and we transfer data by small pieces, which size equals BUFSIZE. 
I use only one pipe, only one semaphore. 
