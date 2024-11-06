import os
import Crypto.Random
import struct
import selectors
import socket
from Crypto.Cipher import AES, PKCS1_OAEP
from Crypto.PublicKey import RSA
from sql import *
from base64 import b64decode
from Crypto.Util import Padding
import cksum
from datetime import datetime
from general import * 
from various_functions import * 
from request import *
from files import *
from clients import *

sel = selectors.DefaultSelector()

# new client
def accept(sock, mask):
    conn, addr = sock.accept()
    print('accepted from', addr)
    conn.setblocking(False)
    sel.register(conn, selectors.EVENT_READ, read)

# handle new request from client
def read(conn, mask):
    request = Request()
    try: 
        if (request.getHeader(conn) == END): # the client ended its program.
            sel.unregister(conn)
            conn.close()  
            return  
        request.header_validation(conn) # checks the version
        if request.code == REGISTRATION_REQUEST_825:  
            try:       
                request.registration_request_825(conn)
            except:
                print("registration failed, send 1601 code")
                clean_buffer(conn)
                Answer(REGISTRATION_FAILED_1601,0).send_header_to_client(conn)        
        elif request.code == RECONNECTION_REQUEST_827:      
            request.reconnection_request_827(conn)
        elif request.code == SEND_PUBLIC_KEY_REQUEST_826:
            request.send_public_key_request_826(conn)
        elif request.code == SEND_FILE_REQUEST_828:
            request.send_file_request_828(conn)
        elif request.code == VALID_CRC_REQUEST_900 or request.code == INVALID_CRC_REQUEST_901 or request.code == INVALID_CRC_4TH_REQUEST_902: 
            request.crc_response_900_901_902(conn)  
        else:# if the code is not one of the above
            print("Unrecognized code.")   
            raise ValueError()  
    except:# If an error occurred
        clean_buffer(conn)
        print("Error accured. send general error")
        Answer(GENERAL_ERROR_1607,0).send_header_to_client(conn)

#create new db if needed or gets all the clients and files from the exists db
def data_base_handle():        
    if not os.path.exists('defensive.db'):        
        sql.create_data_base()
    for client in get_list("clients"):
        #create the list of the clients
        new_client = Client(client[1])
        new_client.set_values(client[0] ,client[2],client[3],client[4])
        clients_list.append(new_client)
    for file in get_list("files"):
        for client in clients_list:
            if client.client_id == file[0]:
                # create the list of the files
                client.files_list.append(File(file[0],file[1],file[2],file[3]))
    if not os.path.exists(folder):  
        # the folder to the files          
        os.mkdir(folder) 

if __name__ == "__main__":
    if os.path.exists('port.info'):    
        with open('port.info', 'r') as f:
            port = f.readlines()    
    if not os.path.exists('port.info') or port == '':# invalid port. listening on default port
        print("Warning: The server is listening on default port- number 1234")
        port = DEFAULT_PORT_NUMBER
    data_base_handle()        
    try:
        sock = socket.socket()
        sock.bind(('', int("".join(port))))
        sock.listen(100)
        sock.setblocking(False)
        sel.register(sock, selectors.EVENT_READ, accept)
        while True:
            events = sel.select()
            for key, mask in events:
                callback = key.data
                callback(key.fileobj, mask)
    except:
        print('Socket closed suddenly. exit.')
        exit(1)
    finally:
        sock.close




