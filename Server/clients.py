from datetime import datetime
import uuid
from general import *
import os
class Client:
    def __init__(self, client_name): # new client
        self.client_id = uuid.uuid4().bytes #generate uuid
        self.client_name = client_name
        self.public_key = ""
        self.last_seen_date = str(datetime.now())
        self.aes_key = "" # the key is not encrypted!!!!
        self.files_list = [] # list of files

        #create folder to this client
        path = folder + '/' + self.client_name
        if not os.path.exists(path): 
            os.mkdir(path)

    #this function is called from the db reconstruction in the main
    def set_values(self,client_id,public_key,last_seen_date,aes_key): 
        self.client_id = client_id
        self.public_key = public_key
        self.last_seen_date = last_seen_date
        self.aes_key = aes_key # the key is not encrypted!!!! 

