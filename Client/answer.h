#pragma once
#include <string>
#include "global.h"
#include <boost/endian.hpp>
#include <iostream>
#include "RSAWrapper.h"
#include "Base64Wrapper.h"

//This class represent the answer (response) that the client gets from the server

class Answer {
    // the header fields
    int _version;
    int _code;
    int _payload_size;
    //payload
    std::vector <uint8_t> _payload;
public:

    Answer()
        :
        _version( 0 ),
        _code(0),
        _payload_size(0),
        _payload() // std::vector initialized to empty
    {}

    unsigned long getNumber(tcp::socket& s, int size);
    int getCode();
    void get_header_from_server(tcp::socket& s);
    void read_string_and_check(tcp::socket& s, std::string string , int size);
    std::string getEncryptedAes(tcp::socket& s);
    std::string getClientID(tcp::socket& s);

};
