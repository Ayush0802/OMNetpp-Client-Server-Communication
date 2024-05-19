#ifndef _ASSIGNMENT2_CLIENT_H
#define _ASSIGNMENT2_CLIENT_H

#include <omnetpp.h>
#include <bits/stdc++.h>
using namespace std;

using namespace omnetpp;

class Client : public cSimpleModule
{
  private:
    int array_size;
    int clientId;
    int n_server;
    int n_subarray;
    int n_connected_server;
    int n_clients;
    int subarray_index_range;
    int count_gsp_rcv;
    int count_prpg_rcv;
    int round;
    int r_copy;
    std::ofstream clientout;

    int globalArray[1000];
    vector<int> serverIndices;
    vector<int> subarrayIndices;
    vector<int> subarray_rand_indices;
    vector<vector<pair<int, int>>> res_task;
    vector<int> subarray_max;
    vector<int> score;
    vector<int> avg_score;
    vector<int> priority;
    vector<size_t> hashList;

    int* originalArray;


    cMessage* createArrayMessage(int* array, int size);

  protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
    virtual void updateScore();
    virtual void send_array_to_server();

    virtual void sendGossip();
    virtual void handleGossipMessage(cMessage *msg);

    virtual void finalize_avg_score();
    virtual string createGossipMessage();
    virtual void start();

    virtual vector<int> generate_server_indexes();
    virtual vector<int> generate_rand_subarray_indexes(int n, int m);
    virtual vector<int> getPriorityVector(const vector<int>& originalVector);
    virtual ~Client();
};

#endif
