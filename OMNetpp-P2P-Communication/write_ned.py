import sys
import textwrap
import argparse
from config import *
import re
import os
import math
import random
import numpy as np

def write_ned_file(topo_filename, output_filename):
    
    topo_file = open(topo_filename).readlines() 
    outfile = open(output_filename, "w")
    
    line_1 = topo_file[0].split()
    
    nClients = int(line_1[0])
    
    
    BASE_NED = (
'network Network\n'
'{\n'
'    parameters:\n'
f'        int num_clients = {nClients};\n'
f'        @display("bgb=1269.8201,883.736");\n'
'\n'
'    submodules:\n'
'        client[num_clients]: Client {\n'
'            parameters:\n'
'                client_id = index;\n'
'                x = parent.num_clients;\n'
'                @display("p=,,ri,100,100;i=device/laptop");\n'
'        }\n'
'\n'
'    connections:\n'
'\n'
)

    outfile.write(BASE_NED)

    m = nClients
    p = math.ceil(math.log2(m))
    for n in range(nClients):
        for i in range(1,p+1):
            t = (n+2**(i-1))%(m)
            
            node_1 = f"client[{n}]"
            node_2 = f"client[{t}]"
            delay = 100
        
            w_line_1 = f"        {node_1}.client_out++ --> {{delay = {delay}ms; }} --> {node_2}.client_in++;\n"
        
            outfile.write(w_line_1)
            outfile.write("\n")

    outfile.write("}")
    
write_ned_file("topo.txt", "network.ned")