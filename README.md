# TCP-Server-Client
This is a TCP server client implementation in C using socket programming and multithreading concepts.

The server program and client program can be run on two different systems (provided they are connected to the same network)

Change the server_address.sin_addr.s_addr value in client program to the IP address of server system and server_address.sin_port value to the port that the server is listening at.

The server run infinitely once started and can support multiple clients. 

Each client can choose to either read or write to a data value in the server.

This communication is established by setting up a TCP socket through which they can communicate.
