
#object of file
class File:
    def __init__(self, client_id, file_name, path, valid_crc):
        self.client_id = client_id
        self.file_name = file_name
        self.path = path
        self.valid_crc = valid_crc
