#include "client.h"
#include <iostream>
#include <algorithm>
#include <sstream>
#include <functional>
#include <chrono>
#include <fstream>
#include <random>
using namespace std;

Define_Module(Client);

void Client::initialize()
{
    n_server=par("n");
    clientId = par("client_id");
    n_subarray = n_server;
    n_connected_server = n_server / 2 + 1;
    n_clients = par("x");
    subarray_index_range = 50;
    round = par("r");
    r_copy = round;
    string filePath = "client_outputs/client" + to_string(getIndex()) + ".txt";
    clientout.open(filePath);

    priority.assign(n_server,0);

    start();
}

void Client::start()
{
    if(round!=0){
        cout<<"for client : "<<clientId<<", array is : ";
        clientout<<"for client : "<<clientId<<", array is : ";
        for(int i=0;i<3*n_server;i++){
            globalArray[i] = rand() % 100;
            cout<<globalArray[i]<<" ";
            clientout<<globalArray[i]<<" ";
        }
        cout<<endl;
        clientout<<endl;

        array_size = 3*n_server;

        cout<<endl<<"for client : "<<clientId<<" rounds left : "<<round<<endl;
        clientout<<endl<<"for client : "<<clientId<<" rounds left : "<<round<<endl;

        res_task.assign(subarray_index_range+1 , vector<pair<int, int>>());

        subarray_max.assign(subarray_index_range+1 , -1);

        score.assign(n_server ,0);

        avg_score.assign(n_server ,0);

        count_gsp_rcv = 0;
        count_prpg_rcv = 0;

        send_array_to_server();

        round--;
    }
    else
    {
        cout<<endl<<"All rounds completed for client : "<<clientId<<endl<<endl;
        clientout<<endl<<"All rounds completed for client : "<<clientId<<endl<<endl;
    }

}


void Client::send_array_to_server()
{
        int subarraySize = array_size / n_server;
        int startIndex = 0;

        if(round == r_copy){
            serverIndices = generate_server_indexes();
        }
        else{
            serverIndices = priority;
        }

        subarray_rand_indices = generate_rand_subarray_indexes(n_subarray,subarray_index_range);
        subarrayIndices = subarray_rand_indices;

        for (int i = 0; i < n_subarray; i++) {
            int subarrayIndex = subarrayIndices[i];

            int* subarray = new int[subarraySize+1];                // Create a subarray for this server
            std::copy(globalArray + startIndex, globalArray + startIndex + subarraySize, subarray);
            subarray[subarraySize] = subarrayIndex;


            for (int j = 0; j < n_connected_server; j++) {          // Loop through each connected server
                int serverIndex = serverIndices[j];
                send( createArrayMessage(subarray, subarraySize+1), "client_out", serverIndex);

                cout<<"client send : ";                         // printing the subarray sended
                clientout<<"client send : ";
                for(int k=0;k<subarraySize+1;k++){
                    cout<<subarray[k]<<" ";
                    clientout<<subarray[k]<<" ";
                }  cout<<endl; clientout<<endl;
            }

            startIndex += subarraySize;                         // Update the start index for the next subarray
        }

}


void Client::handleMessage(cMessage *msg)
{
    if (msg->getArrivalGate()->getIndex()<n_server){
        int* receivedArray = static_cast<int*>(msg->getContextPointer());
        int receivedSize = msg->getKind();

        int serverId = msg->getArrivalGate()->getIndex();
        int maxValue = *receivedArray;
        int subarrayId = *(receivedArray+1);

        cout<<"client "<<clientId<<", subarray rcv : "<<maxValue<<" "<<subarrayId<<endl;
        clientout<<"client "<<clientId<<", subarray rcv : "<<maxValue<<" "<<subarrayId<<endl;

        res_task[subarrayId].push_back(std::make_pair(maxValue, serverId));     // storing the max elements along with server id in res_task

        if(maxValue > subarray_max[subarrayId]){                    // storing max value for each subarray in subarray_max
           subarray_max[subarrayId] = maxValue;
        }

        int count = 0;                                      // counting subarrays for all connected sever are recvd or not, if yes then update score
        for(int i=0;i<n_subarray;i++){
            int k=subarrayIndices[i];
           if(res_task[k].size()==n_connected_server){
               count++;
           }
           else{
               break;
           }
        }

        if(count == n_subarray)                 // whenever any subarray rcv, check whetehr all subarray rcvd for all conn. server
        {
           cout << "client "<< clientId << " : all msg recvd" << endl;
           clientout << "client "<< clientId << " : all msg recvd" << endl;
           auto mx = max_element(subarray_max.begin(), subarray_max.end());
           int max_ans = *mx;
           cout<<"Final Answer : Max Element of array : "<<max_ans<<endl;
           clientout<<"Final Answer : Max Element of array : "<<max_ans<<endl;

           updateScore();          // update the score for servers

           sendGossip();           // start sending the score to other clients
        }
    }
    else
    {
        handleGossipMessage(msg);
    }


}

void Client::sendGossip()
{
    string messageStr = createGossipMessage();
    size_t hash = std::hash<std::string>{}(messageStr);             // Calculate hash of the message string
    hashList.push_back(hash);

    int l = 0;
    for (int i = n_server; i<gateSize("client_out"); i++) {      // Send the gossip message to all adjacent nodes (clients)
            cMessage *message = new cMessage("StringMessage");
            message->addPar("Array").setStringValue(messageStr.c_str());
            send(message, "client_out", i);
            l++;
    }
}

void Client::handleGossipMessage(cMessage *msg) {
    string receivedString = msg->par("Array").stringValue();
    size_t hash = std::hash<std::string>{}(receivedString);

    if (find(hashList.begin(), hashList.end(), hash) == hashList.end()) {           // condition if gsp not present in hash
        cout<<"client : "<<clientId<<endl<<"gsp rcv : "<<receivedString<<endl;
        clientout<<"client : "<<clientId<<endl<<"gsp rcv : "<<receivedString<<endl;

        count_gsp_rcv++;        // store how many gsp rcvd by a client

        hashList.push_back(hash);       // add gsp msg in hash

        cout<<"updating total score for client : ";
        clientout<<"updating total score for client : ";
        int x=0;
        bool t = false;
        for(int j=0;j<receivedString.length();j++){             // updating total score
            if(receivedString[j]=='>'){
                break;
            }
            else if(receivedString[j]=='['){
                t=true;
            }
            else if(t==true){
                avg_score[x] = avg_score[x]+int(receivedString[j]-'0');
                cout<<avg_score[x]<<" ";
                clientout<<avg_score[x]<<" ";
                x++;
                j++;
            }
            else{
                continue;
            }
        }   cout<<endl; clientout<<endl;


        if(count_gsp_rcv == 3){             // checking if all clients have recieved gsp msg from all other clients
            finalize_avg_score();
        }
        else{
            cout<<endl; clientout<<endl;
        }


        cout<<"prpg send : ";
        clientout<<"prpg send : ";
        int gateid = msg->getArrivalGate()->getIndex();
        for (int i = n_server; i < gateSize("client_out"); i++) {           // send the propagation to other clients
            if(i!= gateid){
                cMessage *mess = new cMessage("StringMessage");
                mess->addPar("Array").setStringValue(receivedString.c_str());
                send(mess, "client_out", i);
                cout<<i-n_server<<" ";
                clientout<<i-n_server<<" ";
            }
        }   cout<<endl; clientout<<endl;

    }
    else
    {
        count_prpg_rcv++;                   // 1 propagation msg rcvd
        cout<<"client prpg rcv : "<<clientId<<" count : "<<count_prpg_rcv<<endl;
        clientout<<"client prpg rcv : "<<clientId<<" count : "<<count_prpg_rcv<<endl;
    }
}


void Client::finalize_avg_score()
{
    cout<<"final avg score : ";
    clientout<<"final avg score : ";
    for(int i=0;i<n_server;i++){
        avg_score[i] = avg_score[i]/n_clients;          // divide the score to get avg score
        cout << avg_score[i]<<" ";
        clientout << avg_score[i]<<" ";
    }
    cout<<endl;
    clientout<<endl;

    priority = getPriorityVector(avg_score);
    cout<<"priority : ";
    clientout<<"priority : ";
    for(int i=0;i<n_server;i++){
        cout<<priority[i]<<" ";
        clientout<<priority[i]<<" ";
    }
    cout<<endl<<endl;
    clientout<<endl<<endl;

    start();                             // start new round
}

void Client::updateScore()
{
    cout<<"res task : "<<endl;
    clientout<<"res task : "<<endl;
    for(int i=0;i<n_subarray;i++){
        int k = subarrayIndices[i];
        for(int j=0;j<n_connected_server;j++){

           if(res_task[k][j].first == subarray_max[k]){
               cout<<res_task[k][j].first<<"-"<<res_task[k][j].second<<" ";
               clientout<<res_task[k][j].first<<"-"<<res_task[k][j].second<<" ";
               score[res_task[k][j].second]++;
           }
        }  cout<<endl;   clientout<<endl;
    }

   cout << "score updated by : ";
   clientout << "score updated by : ";
   for(int i=0;i<n_server;i++){
       cout<<score[i]<<" ";
       clientout<<score[i]<<" ";
   }

   for(int i=0;i<n_server;i++){             // storing score in average score vector
     avg_score[i]=score[i];
   }
   cout<<endl<<endl;
   clientout<<endl<<endl;
}


std::string Client::createGossipMessage() {
    auto now = chrono::system_clock::now();
    time_t timestamp = chrono::system_clock::to_time_t(now);
    tm* localTime = localtime(&timestamp);

    std::stringstream ss;
    ss << "<";
    ss << "'" << put_time(localTime, "%H:%M:%S")<<"'" << ":"; // Timestamp
    ss << clientId << ":"; // Client ID
    ss << "[";
    for (int i = 0; i < n_server; ++i) {
        ss << score[i];
        if (i < n_server - 1) {
            ss << ",";
        }
    }
    ss << "]>";
    return ss.str();
}

cMessage* Client::createArrayMessage(int* array, int size)
{
   cMessage* msg = new cMessage("ArrayMessage");
   msg->setContextPointer(array);
   msg->setKind(size);
   return msg;
}

vector<int> Client::generate_server_indexes(){
    std::random_device rd;
    std::mt19937 g(rd());


    vector<int> v(n_server,0);
    for (int i = 0; i < n_server; i++) {
        v[i] = i;
    }
    std::shuffle(v.begin(), v.end(), g);

    cout<<"server_ind : ";
    clientout<<"server_ind : ";
    for(int i=0;i<n_server;i++){
            cout<<v[i]<<" ";
            clientout<<v[i]<<" ";
        }cout<<endl;
        clientout<<endl;

    return v;
}

vector<int> Client::generate_rand_subarray_indexes(int n, int m) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, m); // Uniform distribution between 0 and m

    cout<<"subarray_rand_ind : ";
    clientout<<"subarray_rand_ind : ";
    vector<int> randomNumbers(n,0);
    int i=0;
    while (i< n) {
        int num = dis(gen);
        if (std::find(randomNumbers.begin(), randomNumbers.end(), num) == randomNumbers.end()) {
            randomNumbers[i++]= num;
            cout<<num<<" ";
            clientout<<num<<" ";
        }
    }
    cout<<endl;
    clientout<<endl;

    return randomNumbers;
}

vector<int> Client::getPriorityVector(const vector<int>& originalVector) {
    vector<pair<int, int> > indexedValues;
    for (size_t i = 0; i < originalVector.size(); ++i) {
        indexedValues.push_back(std::make_pair(originalVector[i], i));
    }
    sort(indexedValues.begin(), indexedValues.end());
    reverse(indexedValues.begin(),indexedValues.end());

    vector<int> priorityVector(originalVector.size());
    for (size_t i = 0; i < indexedValues.size(); ++i) {
        priorityVector[i] = indexedValues[i].second;
    }
    return priorityVector;
}
Client::~Client()
{
   delete[] originalArray;
}
