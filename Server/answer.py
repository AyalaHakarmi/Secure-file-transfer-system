from general import *
from various_functions import *

class Answer:
    def __init__(self, code, payload_size ):
        self.version = VERSION
        self.code = code
        self.payload_size = payload_size
        
    # send the header of the answer to the client 
    def send_header_to_client(self,conn):
        conn.sendall(convert_int_to_byte(self.version, VERSION_SIZE))
        conn.sendall(convert_int_to_byte(self.code, CODE_SIZE))
        conn.sendall(convert_int_to_byte(self.payload_size, PAYLOAD_SIZE_SIZE))
      





