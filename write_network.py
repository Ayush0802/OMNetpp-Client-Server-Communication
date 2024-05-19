import sys
import textwrap
import argparse
from config import *
import re
import os
import math
import random
import numpy as np


def parse_node_name(node_name):
    return f"client[{int(node_name[:-1])}]"


def write_ned_file(topo_filename, output_filename):
    
    topo_file = open(topo_filename).readlines() 
    outfile = open(output_filename, "w")
    
    line_1 = topo_file[0].split()
    
    nServers = int(line_1[0])
    nClients = int(line_1[1])
    nRounds = int(line_1[2])
    
    
    BASE_NED = (
'network Network\n'
'{\n'
'    parameters:\n'
f'        int num_servers = {nServers};\n'
f'        int num_clients = {nClients};\n'
f'        int round = {nRounds};\n'
f'        @display("bgb=1269.8201,883.736");\n'
'\n'
'    submodules:\n'
'        server[num_servers]: Server {\n'
'            parameters:\n'
'                server_id = index;\n'
'                n = parent.num_servers;\n'
'                x = parent.num_clients;\n'
'                @display("p=240,295,m,50,120;i=device/server;is=l");\n'
'        }\n'
'\n'
'        client[num_clients]: Client {\n'
'            parameters:\n'
'                client_id = index;\n'
'                n = parent.num_servers;\n'
'                x = parent.num_clients;\n'
'                r = parent.round;\n'
'                @display("p=380,540,m,100,120;i=device/laptop;is=l");\n'
'        }\n'
'\n'
'    connections:\n'
'        for i=0..num_servers-1, for j=0..num_clients-1 {\n'
'            client[j].client_out++ --> server[i].server_in++;\n'
'           server[i].server_out++ --> client[j].client_in++;\n'
'       }\n'
'\n'
)

    outfile.write(BASE_NED)
    line_num = 1
    
    del topo_file[0]
    
    for line in topo_file:

        if line == "\n":
            line_num += 1
            continue
        
        node_1 = parse_node_name(line.split()[0])
        node_2 = parse_node_name(line.split()[1])
                
        delay_1 = float(line.split()[2])
        delay_2 = float(line.split()[3])
        
        w_line_1 = f"        {node_1}.client_out++ --> {node_2}.client_in++;\n"
        w_line_2 = f"        {node_2}.client_out++ --> {node_1}.client_in++;\n"
        
        outfile.write(w_line_1)
        outfile.write(w_line_2)
        outfile.write("\n")

    outfile.write("}")
    
write_ned_file("topo.txt", "network.ned")