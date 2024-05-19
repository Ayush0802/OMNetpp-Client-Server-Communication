#ifndef _ASSIGNMENT3_CLIENT_H
#define _ASSIGNMENT3_CLIENT_H

#include <omnetpp.h>
#include <bits/stdc++.h>
using namespace std;

using namespace omnetpp;

class Client : public cSimpleModule
{
  private:
    int clientId;
    int n_clients;
    int n_subarray;
    int count;
    int result;
    int array_size;
    int m;
    int p;
    std::ofstream clientout;

    int globalArray[1000];
    vector<pair<int,int>> conn_mat;
    vector<int> subarray_max;
    vector<size_t> hashList;

    int* originalArray;


    cMessage* createArrayMessage(int* array, int size);

  protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;

    virtual void send_array_to_client();
    virtual int calculate_index(int destination);

    virtual void sendGossip();

    virtual ~Client();
};

#endif
