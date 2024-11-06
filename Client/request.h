#pragma once

#include <string>
#include "global.h"
#include <boost/endian.hpp>
#include <iostream>
#include "RSAWrapper.h"
#include "Base64Wrapper.h"
#include "various_functions.h"


//this class represent the request that the client send to the server.
class Request {
    int _num_of_failures; // the numbers of times that this request sent to the server and got error
    // the header fields
    uint8_t _client_id[CLIENT_ID_SIZE];
    uint8_t _version[VERSION_SIZE];
    uint8_t _code[CODE_SIZE];
    uint8_t _payload_size[PAYLOAD_SIZE_SIZE];
    //payload
    std::vector <uint8_t> _payload;
public:
  
    Request()
        : _num_of_failures(0),
        _client_id{ 0 },
        _code{ 0 },
        _payload_size{ 0 },
        _payload() // std::vector initialized to empty
    {
        _version[0] = VERSION;
    }

    void setClientID(std::string id);
    void restartFailures();
    void setCode(int code);
    int getCode();
    void setPayloadSize(int size);
    void appendStringPayload(std::string payload, int wanted_size);
    void send_to_server(tcp::socket& s);
    int getNumFailurs();
    void addFailure();
    void appendNumberPayload(int num, int size);
    void appendFileSlice(std::string encrypted_file, int begin, int end);
    void clearPayload();





};
