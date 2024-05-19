network Network
{
    parameters:
        int num_clients = 6;
        @display("bgb=1269.8201,883.736");

    submodules:
        client[num_clients]: Client {
            parameters:
                client_id = index;
                x = parent.num_clients;
                @display("p=,,ri,100,100;i=device/laptop");
        }

    connections:

        client[0].client_out++ --> {delay = 100ms; } --> client[1].client_in++;

        client[0].client_out++ --> {delay = 100ms; } --> client[2].client_in++;

        client[0].client_out++ --> {delay = 100ms; } --> client[4].client_in++;

        client[1].client_out++ --> {delay = 100ms; } --> client[2].client_in++;

        client[1].client_out++ --> {delay = 100ms; } --> client[3].client_in++;

        client[1].client_out++ --> {delay = 100ms; } --> client[5].client_in++;

        client[2].client_out++ --> {delay = 100ms; } --> client[3].client_in++;

        client[2].client_out++ --> {delay = 100ms; } --> client[4].client_in++;

        client[2].client_out++ --> {delay = 100ms; } --> client[0].client_in++;

        client[3].client_out++ --> {delay = 100ms; } --> client[4].client_in++;

        client[3].client_out++ --> {delay = 100ms; } --> client[5].client_in++;

        client[3].client_out++ --> {delay = 100ms; } --> client[1].client_in++;

        client[4].client_out++ --> {delay = 100ms; } --> client[5].client_in++;

        client[4].client_out++ --> {delay = 100ms; } --> client[0].client_in++;

        client[4].client_out++ --> {delay = 100ms; } --> client[2].client_in++;

        client[5].client_out++ --> {delay = 100ms; } --> client[0].client_in++;

        client[5].client_out++ --> {delay = 100ms; } --> client[1].client_in++;

        client[5].client_out++ --> {delay = 100ms; } --> client[3].client_in++;

}