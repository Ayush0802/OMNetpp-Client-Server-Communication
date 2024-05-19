# CSL3080 Assignment-3

## Objective
The objective of this assignment is to Utilize OMNeT++ Discrete Event Simulator to simulate remote program execution, where each task is divided into x subtasks, each with ID i, which is randomly generated. Each subtask with ID i is sent to the client with ID i%N, where N is the total number of clients in the network. For this assignment, focus on executing a simple task, like finding the maximum element in an array. Given an array of k elements, the client divides it into approximately x equal parts, where k/x ≥ 2.

## Network (p2p)
- **Methodology:** We used the CHORD protocol to setup our connection. If simple ring shaped topology is used then each client will take O(n) in worst case to send a message to another client. But using CHORD protocol it is reduced to log(n). CHORD defines the connection of each client by a simple formula that is [n+2^(i-1)] % m. It also defines the max connections that a client will make that is p = ceil(log(m)). Here m = total clients, n = client id, i = [1,p]. This topology is very efficient and can handle millions of node.

- **Client Node:** Each client node has an array. It has to find the max of array using other clients. The client will first divide the array in subarrays. Now the subtask is to find the max of that subarray. So it will send all the subtask to the particular selected client except itself. Then all the client nodes who sent the subtasks will receive the corresponding answers of that particular subtasks from all the selected clients. After receiveing all the answers, client will then find the maximum of the recieved response to find the absolute maximum. Upon completion, it then broadcasts gossip message to rest of the connected clients.

## Code files and its functionality

- **Client.ned:** Used to define the behavior and characteristics of a client entity within the simulated network environment.

- **Client.h:** Used to define the functions and the variable used in the server along with its class defination.

- **Client.cc:** The client.cc file represents the behavior of a client node. It defines functions for initializing the client, sending the messages, handling incoming messages, sending gossip messages to clients.

  - Initialization :
The `initialize()` function sets up the client node with parameters such as ID and generating the task i.e. array. It also create an array that store the connections of that client. It then `calls send_array_to_client()` to begin the simulation.

  - Message Sending : The `send_array_to_client()` responsible for dividing the into `x` subtasks, where each subtask should have a length of at least 2. We took it 3.

  - Message Handling :
Incoming messages are processed by `handleMessage()`. The client can recieve 3 types of messages : 
1. **Mode = -1** :  The message conatins the subarray. In this case, client will check if the destination clientId matches with itself: a. If Yes then calculates the max and make a new message and send it to the client which sended the subarray. b. if No, then simply forwards the message according the CHORD protocol
2. **Mode = -2** :  The message conatins max element calculated for of any subarray. In this case, client will check if the destination clientId matches with itself: a. If Yes, then extact the max elemet and store it in a vector. b. if No, then simply forwards the message according the CHORD protocol.
3. **Mode = -3** :  It is gossip message, check if gossip is present in hash table. If not then propagate to other clients.

In short : 
  - **Mode = -1** -> subtask
  - **Mode = -2** -> answer of the subtask
  - **Mode = -3** -> Gossip Message

After a cleint recieves the result(max elements) of all subtasks(subarrays) it generated, it will find the max of all the results. This is final maximum of whole array it has initially.
 
- **Network.ned:** The network simulation file, depicts a basic network model comprising all the clients based on CHORD protocol. It includes parameters for adjusting the number of clients. The layout is visually represented with specific coordinates, and connections are established between clients and servers as well as among clients themselves. Here we have used the CHORD distributed hash table protocol for transmission of message in log(N) time complexity. The clients nth node simply makes connection with log(N) number of clients, where the ith connection is represented by ((n + 2^i)%nClients), here n denotes the client whose connections are being made.

- **topo.txt:** Contains the value of number of client nodes.

- **write_ned.py:** This particular file is used to dynamically create "Network.ned" file using "topo.txt" file. 

## Message Protocol 
1. Sending Subarrray = <Subarray_values : Subarray id : Destination clientId : Source clientId : Mode = -1>
1. Sending Max Value of Subarray = <Max_Value : Subarray id : Destination clientId : Source clientId : Mode = -2>
1. Gossip message = <self.timestamp : self.ClientID : self.result : Mode = -3>

## Input Format
User should give total number of clients to be used in network in "topo.txt" file.

## How To Execute
1. Provide total number of clients in network through topo.txt file.
1. Run the python script write_ned.py for dynamically creating .ned file 
2. Build and run the project 
3. Click on play simulation button and observe the message passing from client-to-client.

## Output Format
Output of each client is shown on the console as well as separate output files are generated for each client in client_outputs