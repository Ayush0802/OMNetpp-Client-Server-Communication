#ifndef _ASSIGNMENT2_SERVER_H
#define _ASSIGNMENT2_SERVER_H

#include <omnetpp.h>
#include <bits/stdc++.h>
using namespace std;

using namespace omnetpp;

class Server : public cSimpleModule
{
  private:
    int serverId;
    int numClients;
    int n_server;
    int numserver;
    int server_no;
    std::ofstream serverout;


    cMessage* createMessage(int* array, int size);

  protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
};

#endif
