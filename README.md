# OMNetpp-Client-Server-Communication

## Objective
The objective us to utilize OMNeT++ Discrete Event Simulator to simulate remote program execution, where each task is divided into n subtasks and sent to n/2 + 1 servers. The response from the majority will be considered valid. Here we have focused on executing a simple task of finding the maximum element in an array.

## Network Architecture
The network consists of two types of nodes:

- **Client Nodes:** Client nodes will create and send all the subtask to the (n/2+1) selected server. Then all the client nodes will receive the response for all the subtask from all the selected servers. The response from the
majority will be considered valid. Also all client nodes will find the score of all the servers based on the correct response sent by the servers as some of the servers may act malicious for some subtask. After finding the score for each server, each client will broadcast a gossip message containing their respective score array to all the other clients. 

- **Server Nodes:** Server nodes will receive the subtask from the clients and compute maximum for each subtask and will send the output for each subtask back to the client.




## Code Files 

- **client.ned:** Used to define the behavior and characteristics of a client entity within the simulated network environment.

- **server.ned:** Used to define the behavior and characteristics of a server entity within the simulated network environment.

- **network.ned:** The network simulation file, depicts a basic network model comprising servers and clients. It includes parameters for adjusting the number of servers and clients. The layout is visually represented with specific coordinates, and connections are established between clients and servers as well as among clients themselves. This simulation serves as a tool for analyzing network behavior.

- **client.h** The Client.h file contains the declaration of the Client class, which is a part of an OMNeT++ simulation project. It conatins the Data Members, Vectors and Methods. This header file is then included in the Client.cc.

- **client.cc** The client.cc file represents the behavior of a client node. It defines functions for initializing the client, handling incoming messages, sending messages to servers and other clients, and managing simulation rounds.

Initialization :
The initialize() function sets up the client node with parameters such as ID, server count, and round number. It then calls start() to begin the simulation.

Simulation Rounds : 
start() initiates a simulation round by sending subarrays to connected servers. For every round this function is called.

Gossip Communication :
Clients exchange gossip messages containing their scores with adjacent clients via sendGossip() and handleGossipMessage(). 

Message Handling :
Incoming messages are processed by handleMessage(). If the message is from a server, the client processes the received subarray data. If it's a gossip message from another client, it updates its internal state and forwards the message to others.

Finalizing and Updating :
After receiving gossip messages from all other clients, the client calculates its average score and determines priority. It then starts a new round by calling the start().

- **server.h** Just like the client.h, server.h also contains the declaration of the server class

- **server.cc** The server.cc file defines the behavior of server nodes. Each server receives subarrays from clients, processes them, and sends back the result to clients. 

Initialization : 
The initialize() function initializes the server node with parameters such as server ID, total number of servers, number of connected servers, and number of clients.

Handling Messages : 
Incoming messages are processed by handleMessage(). The server extracts the subarray data, calculates the maximum or minimum element based on a condition (malicious behavior), and sends the result back to the client.

Malicious Behavior : 
The server may behave maliciously based on a condition (e.g., server ID modulo 4 equals subarray ID modulo 4). In such cases, it returns the minimum element instead of the maximum.

Message Creation : 
The createMessage() function constructs and returns a message containing the result array to be sent back to the client.


## Input Format
As input we give number of servers and number of clients.

## How To Execute
1. Run the python script for dynamically creating .ned file and provide the values for no. of clients, no. of servers and no. of rounds to that script through topo.txt file.
2. Build and run the project and click on play simulation button and observe the message passing from client->server, server->client and client->client.

## Message Protocol 
1. Gossip message = <self.timestamp> : <self.ClientID> : <self.Score#>

