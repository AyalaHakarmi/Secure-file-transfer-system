from general import *
from various_functions import *
from answer import *
from sql import *
from clients import *
from files import *

# create and send AES key to the server
def create_send_aes(conn, code, client):
    client.aes_key = Crypto.Random.get_random_bytes(AES_KEY_SIZE)  # create aes key
    set_aes_key(client.aes_key,client.client_id)
    encrypted_aes_key = encrypt_by_public_key(client.public_key,client.aes_key) # encrypt aes key with the public key
    Answer(code ,CLIENT_ID_SIZE + len(encrypted_aes_key)).send_header_to_client(conn)
    conn.sendall(client.client_id)
    conn.sendall(encrypted_aes_key)
    print(f"AES key sent to {client.client_name}")
 

# this function updates the last seen date for the client
def last_seen(client):
    set_lastseen_date(client.client_id)# in db
    client.last_seen_date = str(datetime.now())#in the object

# this function handle the operations after the last packet is accepted
def last_packet(file ,client,content_size,orig_file_size,conn):
    # read the encrypted content, decrypt it, override the temporary file with the decrypted content
    decrypted_file_content = create_decrypted_file(file.path,client)# get the decrypted_file_content from the temporary file
    if orig_file_size != len(decrypted_file_content):# checking
        print(f'{client.client_name}, original file size not equals to the file size the server got.') 
        raise ValueError() 
    crc = cksum.memcrc(decrypted_file_content) #calculate CRC
    Answer(FILE_RECEIVED_CRC_1603, CLIENT_ID_SIZE+CONTENT_SIZE_SIZE+FILE_NAME_SIZE+CRC_SIZE).send_header_to_client(conn)
    conn.sendall(client.client_id)
    conn.sendall(convert_int_to_byte(content_size,CONTENT_SIZE_SIZE))
    # send the file name to the client
    byte_array = bytearray(file.file_name, 'utf-8')
    padded_array = byte_array.ljust(FILE_NAME_SIZE, b'\0') 
    conn.sendall(padded_array)
    # send the CRC value
    conn.sendall(convert_int_to_byte(crc, CRC_SIZE))
    print(client.client_name + ", the server create your file")

#this function send the last answer to the client
def thank_you_answer(client_id, conn):
    Answer(THANK_YOU_CLIENT_1604, CLIENT_ID_SIZE).send_header_to_client(conn)
    conn.sendall(client_id)


class Request:
    def __init__(self):
        self.client_id =""
        self.version = 0
        self.code = 0
        self.payload_size = 0

    #this function check that the hader is valid (the version is appropriate)
    def header_validation(self,conn):# checks if the vertion is 3
        if (self.version != VERSION):
            print ("client's version is not" + VERSION + ". send general error")
            raise ValueError()
        
    #this function gets the haeder of the requset from the client
    def getHeader(self,conn):# get the header 
        byte_id_for_check = conn.recv(CLIENT_ID_SIZE)
        if not byte_id_for_check: # means client didnt send anything, client end its program
            print ("Communication with this client has ended")
            return END
        self.client_id = byte_id_for_check # so the client id is fine
        self.version = convert_byte_to_int(conn.recv(VERSION_SIZE)) 
        self.code = convert_byte_to_int(conn.recv(CODE_SIZE))
        self.payload_size = convert_byte_to_int(conn.recv(PAYLOAD_SIZE_SIZE)) 
    #this function handle request number 825  (add the client to the db and the clients list)  
    def registration_request_825(self,conn):
        new_client_name = conn.recv(CLIENT_NAME_SIZE).decode('utf-8').replace('\0', '')  # get name of client
        for client in clients_list:
            if client.client_name == new_client_name: # its error in the registration
                print(f'{new_client_name} already exists as a client, your request to register as a new user not accepted.')
                last_seen(client)
                raise ValueError()
        new_client = Client(new_client_name)# bulid new client object
        clients_list.append(new_client) #add the client to the list
        add_client_db(new_client)# add the client to the DB
        Answer(REGISTRATION_DONE_1600,CLIENT_ID_SIZE).send_header_to_client(conn)
        conn.sendall(new_client.client_id)
        print(f"{new_client.client_name} is registered to the server")
          
          
    #this function handle request number 827  (reconnection)  
    def reconnection_request_827(self,conn):
        new_client_name = conn.recv(CLIENT_NAME_SIZE).decode('utf-8').replace('\0', '')  # get name of client
        found = False # check if the client exists
        for client in clients_list:
            if client.client_name == new_client_name:
                # check that the id matchs the client's ID
                check_id(self.client_id, client.client_id,new_client_name)
                print(f'{client.client_name}, you reconnected with the server.')
                last_seen(client)
                found = True
                try:
                    rsa_key = RSA.import_key(client.public_key)  # check if the public key valid
                except:    
                    print(f'{client.client_name}, invalid public key.')
                    Answer(RECONNECTION_FAILED_1606,CLIENT_ID_SIZE).send_header_to_client(conn)
                    conn.sendall(client.client_id)
                    return
                create_send_aes(conn,VALID_RECONNECTION_AES_1605,client)
                break
        if not found:
            print(f'{new_client_name}, you asked to reconnect but you are not registered with the server.')
            Answer(RECONNECTION_FAILED_1606,CLIENT_ID_SIZE).send_header_to_client(conn)
            conn.sendall(self.client_id)
    #this function handle request number 826  (get the bublic key and send AES to the client)  
    def send_public_key_request_826(self,conn):
        new_client_name = conn.recv(CLIENT_NAME_SIZE).decode('utf-8').replace('\0', '')  # get name of client
        found = False # check if the client exists
        for client in clients_list:
            if client.client_name == new_client_name:
                found= True
                # check that the id matchs the client's ID
                check_id(self.client_id, client.client_id,new_client_name)
                client.public_key = conn.recv(PUBLIC_KEY_SIZE)
                set_public_key(client.public_key,client.client_id)#set in the DB
                print(f"Public key accepted from {new_client_name}")
                last_seen(client)
                create_send_aes(conn,GOT_PUBLIC_SEND_AES_1602,client)
                break
        if not found: # there is no client with this name
            print(f'{new_client_name}, you send public key but you are not registered with the server.')
            raise ValueError()        
    #this function handle request number 828  (gets packet of file , append to the temporary file, and sent crc if its the last packet)              
    def send_file_request_828(self,conn):   
        content_size = convert_byte_to_int(conn.recv(CONTENT_SIZE_SIZE))
        orig_file_size = convert_byte_to_int(conn.recv(ORIG_FILE_SIZE_SIZE))
        packet_number = convert_byte_to_int(conn.recv(PUCKET_NUMBER_SIZE))
        total_packets = convert_byte_to_int(conn.recv(TOTAL_PUCKETS_SIZE))
        file_name = conn.recv(FILE_NAME_SIZE).decode('utf-8').replace('\0', '')
        # calculate the size of the massage content field according to the payload size
        current_pucket_size = self.payload_size - CONTENT_SIZE_SIZE - ORIG_FILE_SIZE_SIZE-PUCKET_NUMBER_SIZE-TOTAL_PUCKETS_SIZE-FILE_NAME_SIZE
        encrypted_packet_content = conn.recv(current_pucket_size)
        found_client = False
        found_file = False
        for client in clients_list:
            if client.client_id == self.client_id:
                found_client = True
                path = folder + '/' + client.client_name + '/' + file_name
                if packet_number == 1:
                    client_file = open(path, "wb")# overrride or create new file
                else:
                    client_file = open(path, "ab") #append to the file
                client_file.write(encrypted_packet_content) # write the content of the packet to the file
                client_file.close()
                last_seen(client)
                for file in client.files_list:
                        if file.file_name == file_name:
                            found_file = True
                            if packet_number == 1:#override case - thhe file exists but its the first packet
                                print(f'{client.client_name}, Note, your file "{file_name}" will be overwritten.')
                                file.valid_crc = False
                                set_valid_crc(client.client_id, False)
                            if packet_number == total_packets: #so this is the last packet
                                last_packet(file,client,content_size,orig_file_size,conn)              
                            break
                if not found_file :  # so we actually opened new file and we dont have object file
                    if packet_number != 1: # its error- if the file not exists the packet must be 1
                        print(f' ID : {self.client_id}, you send packet number {packet_number}, but your file {file_name} not exists.')
                        raise ValueError() 
                    new_file = File(client.client_id,file_name,path,False)
                    add_file_db(new_file)# add to DB
                    client.files_list.append(new_file)# add to the list
                    if packet_number == total_packets:  # in this case-there is just one packet- the first packet is the last packet
                       last_packet(new_file,client,content_size,orig_file_size,conn) 
                break       
        if not found_client:
            print(f' ID : {self.client_id}, you send file but you are not registered with the server.')
            raise ValueError()                  
    #this function handle requests numbers 900,901,902  (the client's response to the crc value)  
    def crc_response_900_901_902(self,conn): 
        file_name = conn.recv(FILE_NAME_SIZE).decode('utf-8').replace('\0', '')
        found_client = False
        found_file = False
        for client in clients_list:
            if client.client_id == self.client_id:
                found_client= True
                last_seen(client)
                for file in client.files_list:
                    if file.file_name == file_name:
                        found_file = True
                        if file.valid_crc == True:# its error- if the crc is valid we are not supposed to get 900,901, or 902
                            print(f'{client.client_name}, Note, your file "{file_name}" already verified.') 
                            raise ValueError() 
                        if (self.code == VALID_CRC_REQUEST_900):
                            file.valid_crc = True
                            set_valid_crc(client.client_id, True)
                            print("The crc of " + file_name + " verified")
                            thank_you_answer(client.client_id,conn)#send 1604
                        if(self.code == INVALID_CRC_REQUEST_901):
                            print(f' ID : {self.client_id}, you send invalid CRC request (code 901). send your file again')
                        elif(self.code == INVALID_CRC_4TH_REQUEST_902):
                            print(f' ID : {self.client_id}, you send invalid CRC request 4th (code 902). deleting yout file')
                            os.remove(file.path) #delete the file
                            client.files_list.remove(file)  #remove from the list
                            delete_file(file_name) #remove from DB
                            thank_you_answer(client.client_id,conn)  #send 1604
                        break
                break                                    
        if not found_client:
            print(f' ID : {self.client_id}, you are not registered with the server.')  
            raise ValueError()  
        if not found_file:
            print(f' ID : {self.client_id}, your file {file_name} not exists.')
            raise ValueError()                                       


