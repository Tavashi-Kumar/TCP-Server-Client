// C program for the Server Side

// inet_addr
#include <arpa/inet.h>

// For threading, link with lpthread
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>

// Semaphore variables
sem_t x, y;
pthread_t tid;
pthread_t writerthreads[100];
pthread_t readerthreads[100];
int readercount = 0;
int d = 0;
char* confirm = "data is set\n";

// Reader Function
void* reader(void* param)
{
	// Lock the semaphore
	sem_wait(&x);
	readercount++;

	if (readercount == 1)
		sem_wait(&y);

	// Unlock the semaphore
	sem_post(&x);

	printf("\n reader %d is inside\n",
		readercount);
		
        // sending some data to the client 
	send(*((int*)param), &d, sizeof(d),0);
	
	sleep(5);

	// Lock the semaphore
	sem_wait(&x);
	readercount--;
	printf("data sent to client\n");

	if (readercount == 0) {
		sem_post(&y);
	}

	// Lock the semaphore
	sem_post(&x);

	printf("\n%d Reader is leaving\n",
		readercount + 1);
	pthread_exit(NULL);
}

// Writer Function
void* writer(void* param)
{
	printf("\nWriter is trying to enter");

	// Lock the semaphore
	sem_wait(&y);

	printf("\nWriter has entered");
	read(*((int*)param),&d,sizeof(d));
	send(*((int*)param),confirm,strlen(confirm),0);

	// Unlock the semaphore
	sem_post(&y);

	printf("\nWriter is leaving");
	pthread_exit(NULL);
}

// Driver Code
int main()
{
	// Initialize variables
	int serverSocket, newSocket;
	struct sockaddr_in serverAddr;
	int addrlen = sizeof(serverAddr);
	
	sem_init(&x, 0, 1); // 0: shared b/w threads, non zero: shared b/w processes, 1 is initial value 
	sem_init(&y, 0, 1);

	serverSocket = socket(AF_INET, SOCK_STREAM, 0); //socket descriptor, acts like a file descriptor
	serverAddr.sin_addr.s_addr = INADDR_ANY;
	//When receiving, a socket bound to this address receives packets from all interfaces. For example, suppose that a host has interfaces 0, 1 and 2. If a UDP socket on this host is bound using INADDR_ANY and udp port 8000, then the socket will receive all packets for port 8000 that arrive on interfaces 0, 1, or 2. If a second socket attempts to Bind to port 8000 on interface 1, the Bind will fail since the first socket already ``owns'' that port/interface. 
	serverAddr.sin_family = AF_INET; 
	// this parameter represents an address family. In most of the Internet-based applications, we use AF_INET.
	serverAddr.sin_port = htons(8989); //converts host byte order to network byte order

	// Bind the socket to the
	// address and port number.
	bind(serverSocket,(struct sockaddr*)&serverAddr,sizeof(serverAddr));

	// Listen on the socket,
	// with 3 max connection
	// requests queued, this is called backlog
	// listen makes the socket passive so it waits for clients 
	if (listen(serverSocket, 3) == 0)
		printf("Listening\n");
	else
		printf("Error\n");

	// Array for thread
	pthread_t tid[3];

	int i = 0;

	while (1) {

		// Extract the first
		// connection in the queue
		newSocket = accept(serverSocket,
						(struct sockaddr*)&serverAddr,
						(socklen_t*)&addrlen);
		int choice = 0;
		recv(newSocket,
			&choice, sizeof(choice), 0);

		if (choice == 1) {
			// Creater readers thread
			if (pthread_create(&readerthreads[i++], NULL,
							reader, &newSocket)
				!= 0)

				// Error in creating thread
				printf("Failed to create thread\n");
		}
		else if (choice == 2) {
			// Create writers thread
			if (pthread_create(&writerthreads[i++], NULL,
							writer, &newSocket)
				!= 0)

				// Error in creating thread
				printf("Failed to create thread\n");
		}

		if (i >= 2) {
			// Update i
			i = 0;

			while (i < 2) {
				// Suspend execution of
				// the calling thread
				// until the target
				// thread terminates
				pthread_join(writerthreads[i++],
							NULL);
				pthread_join(readerthreads[i++],
							NULL);
			}

			// Update i
			i = 0;
		}
	}

	return 0;
}

