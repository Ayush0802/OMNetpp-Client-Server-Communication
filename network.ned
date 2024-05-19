network Network
{
    parameters:
        int num_servers = 6;
        int num_clients = 4;
        int round = 2;
        @display("bgb=1269.8201,883.736");

    submodules:
        server[num_servers]: Server {
            parameters:
                server_id = index;
                n = parent.num_servers;
                x = parent.num_clients;
                @display("p=240,295,m,50,120;i=device/server;is=l");
        }

        client[num_clients]: Client {
            parameters:
                client_id = index;
                n = parent.num_servers;
                x = parent.num_clients;
                r = parent.round;
                @display("p=380,540,m,100,120;i=device/laptop;is=l");
        }

    connections:
        for i=0..num_servers-1, for j=0..num_clients-1 {
            client[j].client_out++ --> server[i].server_in++;
           server[i].server_out++ --> client[j].client_in++;
       }

        client[0].client_out++ --> client[1].client_in++;
        client[1].client_out++ --> client[0].client_in++;

        client[1].client_out++ --> client[2].client_in++;
        client[2].client_out++ --> client[1].client_in++;

        client[2].client_out++ --> client[3].client_in++;
        client[3].client_out++ --> client[2].client_in++;

}