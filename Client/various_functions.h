#pragma once
#include <iostream>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <vector>
#include <filesystem>
#include <boost/endian.hpp>
#include "Base64Wrapper.h"
#include "RSAWrapper.h"
#include "AESWrapper.h"
#include "cksum_new.h"
#include "global.h"
#include <cstdint>  
#include <cstring> 


std::string read_priv_key(); 
std::string convert_hex_to_bytes(const std::string& hex);
std::string find_file_name(std::string path_to_file);
std::string encrypt_file(std::string encrypted_aes, std::string priv_key, std::vector<char> file);
std::vector<char> file_by_path(const std::string& path);
void create_me_info(Base64Wrapper base64,std::string name, std::string private_key, std::string id);
void create_priv_key_file(Base64Wrapper base64, std::string private_key);
std::string convert_string_to_hex(const std::string& string);
int calculate_total_packets(int file_size);



