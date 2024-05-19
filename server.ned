simple Server
{
    parameters :
        int server_id;
        int x;
        int n;
        @display("i=device/server");
        
        gates :
            input server_in[];
            output server_out[];
}