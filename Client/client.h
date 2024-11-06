#pragma once

#include "client_ob.h"
#include "various_functions.h"
#include "request.h"
#include "answer.h"
#include "global.h"
#include <string>
#include <fstream>
#include <iostream>
#include <boost/endian.hpp>
#include "RSAWrapper.h"
#include "Base64Wrapper.h"


//The communication components
boost::asio::io_context io_context;
tcp::socket s(io_context);
tcp::resolver resolver(io_context);


int general_error_handle(Answer& answer, Request& request);
void read_transfer(Client& client);
Answer_codes reconnection(Client& client, Request& request); 
void get_aes_key_from_server(Client& client, Answer& answer);
int crc_checking(Client& client, Request& request, int encrypted_file_size,Request& finish_request);
void the_end(Client& client, int result, Request& finish_request);
void send_file(Client& client, Request& request);
void send_public_key_get_aes(Client& client, Request& request);
void registering(Client& client, Request& request);


