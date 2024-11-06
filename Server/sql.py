
from datetime import datetime
import sqlite3
import clients
from files import *


#this function update the last seen of client
def set_lastseen_date(client_id):
    conn = sqlite3.connect('defensive.db')
    c = conn.cursor()
    c.execute("UPDATE clients SET last_seen_date = ? WHERE client_id = ?", (str(datetime.now()), client_id))
    conn.commit()
    c.close()
    conn.close() 

#this function update the crc value of the file
def set_valid_crc(client_id,boolean):
    conn = sqlite3.connect('defensive.db')
    c = conn.cursor()
    c.execute("UPDATE files SET valid_crc = ? WHERE client_id = ?", (boolean, client_id))
    conn.commit()
    c.close()
    conn.close()

#this function creates the tables of clients and files 
def create_data_base():
    conn = sqlite3.connect('defensive.db')
    c = conn.cursor()
    c.execute(
        'CREATE TABLE clients (client_id text, client_name text, public_key blob, last_seen_date text, aes_key blob, primary key(client_id))')
    c.execute('CREATE TABLE files (client_id text, file_name text, path text, valid_crc bool, primary key(client_id, file_name))')
    conn.commit()
    conn.close()


#this function delete file in case of invalid crc in the 4th time
def delete_file(file_name):
    # Connect to the database
    conn = sqlite3.connect('defensive.db')
    c = conn.cursor()

    # Execute the DELETE statement
    c.execute('DELETE FROM files WHERE file_name = ?', (file_name,))

    # Check if any row was deleted
    if c.rowcount == 0:
        print(f"No file with name '{file_name}' found.")
        raise ValueError() 
    else:
        print(f"File '{file_name}' successfully deleted.")

    # Commit the transaction
    conn.commit()
    conn.close()

#this function execute all files or all lients (according to the target), to insert them to the lists
def get_list(target):
    conn = sqlite3.connect('defensive.db')
    c = conn.cursor()
    query = c.execute(f"SELECT * FROM {target}")
    results = query.fetchall()
    conn.commit()
    c.close()
    conn.close()
    return results

#this function insert new client to the DB
def add_client_db(new_client):
    new_client_tuple = [
        (new_client.client_id, new_client.client_name, new_client.public_key, new_client.last_seen_date, new_client.aes_key)
    ]
    conn = sqlite3.connect('defensive.db')
    c = conn.cursor()
    c.executemany("INSERT INTO clients VALUES(?, ?, ?, ?, ?)", new_client_tuple)
    conn.commit()
    c.close()
    conn.close()

#this function insert new file to the DB
def add_file_db(new_file):
    conn = sqlite3.connect('defensive.db')
    c = conn.cursor()
    file = [
        (new_file.client_id, new_file.file_name, new_file.path, new_file.valid_crc)
    ]
    c.executemany("INSERT INTO files VALUES(?, ?, ?, ?)", file)
    conn.commit()
    c.close()
    conn.close()   

#this function add aes key to client
def set_aes_key(aes_key, client_id):
    conn = sqlite3.connect('defensive.db')
    c = conn.cursor()
    c.execute("UPDATE clients SET aes_key = ? WHERE client_id = ?", (aes_key, client_id))
    conn.commit()
    c.close()
    conn.close()

#this function add public key to client
def set_public_key(public_key, client_id):
    conn = sqlite3.connect('defensive.db')
    c = conn.cursor()
    c.execute("UPDATE clients SET public_key = ? WHERE client_id = ?", (public_key, client_id))
    conn.commit()
    c.close()
    conn.close()    

