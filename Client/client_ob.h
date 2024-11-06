#pragma once

#include <string>
#include "global.h"
#include <string>
#include <iostream>

// this class represent the client. The object contains the important details

class Client {
    std::string _name_client;
    std::string _file_name;
    std::string _client_id;
    std::string _path_to_file;
    std::string _encrypted_aes;
    std::string _priv_key;
    unsigned long _crc_value;

public:
    Client() : _crc_value(0) {}

    void setClientName(std::string name);
    void setFileName(std::string name);
    void setPath(std::string name);
    void setPrivateKey(std::string privKey);
    void setEncryptedAes(std::string aes);
    std::string getClientName();
    void setClientID(std::string id);
    std::string getClientID();
    std::string getEncryptedAes();
    std::string getPrivateKey();
    std::string getPath();
    std::string getFileName();
    unsigned long getCRC();
    void setCRC(unsigned long crc_value);



};
