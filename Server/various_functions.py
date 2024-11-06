from general import *
from datetime import datetime
import sql
import os
import Crypto.Random
from Crypto.Cipher import AES, PKCS1_OAEP
from Crypto.PublicKey import RSA
from base64 import b64decode
from Crypto.Util import Padding
import cksum
from clients import *

#check that the id match
def check_id(old_client_id, new_client_id, client_name):
    if old_client_id != new_client_id:
        print(f'{client_name}, you send You sent an ID that does not match the one stored in the server.')
        raise ValueError()


# this function reads the temporary file, decrypt the content and write back the decrypted content   
def create_decrypted_file(path,client):
    with open(path, "rb") as f:
        encrypted_file_content = f.read() #read the encrypted content
    cipher = AES.new(client.aes_key, AES.MODE_CBC, iv=bytes(16)) # the iv size is 16 regardless to the key length
    decrypted_content= Padding.unpad(cipher.decrypt(encrypted_file_content), AES.block_size) #decryption
    with open(path, "wb") as f: #ovveride the encrypted content
        f.write(decrypted_content)
    return decrypted_content  

# this function converts data in bytes to int (little endian)
def convert_byte_to_int(data_in_bytes):
    return int.from_bytes(data_in_bytes, byteorder='little')

# this function converts number to bytes (little endian)
def convert_int_to_byte(int_data, size):
    return int_data.to_bytes(size, byteorder='little')

#this function encrypts the AES key with the public key in oreder to send it to the client 
def encrypt_by_public_key( public_key,aes_key):
    rsa_key = RSA.import_key(public_key)  # convert pub_key bytes to key format
    cipher_rsa = Crypto.Cipher.PKCS1_OAEP.new(rsa_key)  # encrypt the key
    return cipher_rsa.encrypt(aes_key) #encrypt aes_key using the cipher_rsa

# clean the buffer in case of error
def clean_buffer(conn):
    conn.setblocking(0)  # Set the socket to non-blocking mode
    try:
        while True:
            try:
                packet = conn.recv(4096)  # Read in chunks of 4KB
                if not packet:
                    # No more data, connection may be closed
                    break
            except BlockingIOError:
                # No more data available to read, the buffer is clean
                break
    except Exception as e:
        print(f"Error while clearing buffer: {e}")
    finally:
        conn.setblocking(1)  # Reset the socket back to blocking mode
