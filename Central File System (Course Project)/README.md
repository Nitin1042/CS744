###############################################

This Project consits of the following:
1. Server (server.c)
2. Client (client.c)
3. Test Client (test_client.c)
4. Load-Generator (loadgen.sh)

###############################################

Server:

This is our server process. Upon starting this process waits for incoming connections and creates a thread for every new client.

The server listens on 127.0.0.1:1042. This can be modified if needed.

###############################################

Client:

This is our client process. The client is the one who initiates all the communication. The client should first login or register the user. Makesure that clientfiles folder is created where-ever the client process is executed.

We already have two users (user1, pass1) and (user2, pass2) registered.

After Login/Register we can proceed with our operation. We will have the following options available:
LIST, READ, WRITE, DELETE, CHECKOUT, CREATE, COMMIT, EXIT.

Make sure that the word is fully capitalized.

We also have a sample of dummy files already initialized in the files subdirectory for server to use.

###############################################

Test Client:

This is a client process that is used for load test. We will compile this to get test_client which will be used in loadgen.sh

This will imitate some work by sending some data and sleeping for 10 microseconds continuosly.

###############################################

Load-Generator:

This will create the test clients to put load on the server. It needs a number <n> as an command line argument and created n test clients.

Upon CTRL+C it will terminate as well as killing all the created test clients.

###############################################

Evaluation:

The experiment is done using 1, 2, 5, 10, 50, 100, 200, 500, 1000 clients.

For each number we conducted the test for 5 times and used the averaged results to plot the graph.
