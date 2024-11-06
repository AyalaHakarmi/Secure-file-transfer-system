#include "client_ob.h"

//this function set the client name
void Client::setClientName(std::string name) {
	_name_client = name;
}
//this function set the file name
void Client::setFileName(std::string name) {
	char file_name[NAME_OF_FILE_SIZE];
	strcpy_s(file_name, name.data());
	for (int i = name.size(); i < NAME_OF_FILE_SIZE; i++) /*padding _file_name to be size of 255 with character '\0'*/
	{
		file_name[i] = '\0';
	}
	_file_name = std::string(file_name);
}

//this function set the file path
void Client::setPath(std::string path) {
	_path_to_file = path;
}

//this function set the private key
void Client::setPrivateKey(std::string privKey) {
	_priv_key = privKey;
}

//this function set the client id
void Client::setClientID(std::string id) {
	if (id.size() != CLIENT_ID_SIZE) {
		std::cout << "Client's id is not in the suitable size. exit\n";
		exit(1);
	}
	_client_id = id;
}

//this function returns the client name
std::string Client::getClientName() {
	return _name_client;
}
//this function returns the client ID
std::string Client::getClientID() {
	return _client_id;
}
//this function set the encrypted AES from the server
void Client::setEncryptedAes(std::string aes) {
	_encrypted_aes = aes;
}
//this function returns the encrypted AES from the server
std::string Client::getEncryptedAes() {
	return _encrypted_aes;
}
//this function returns the private key
std::string Client::getPrivateKey() {
	return _priv_key;
}
//this function returns the file path
std::string Client::getPath() {
	return _path_to_file;
}
//this function returns the file name
std::string Client::getFileName(){
	return _file_name;
}
//this function set the crc value of the file
void Client::setCRC(unsigned long crc_value) {
	_crc_value = crc_value;
}
//this function returns the crc value of the file
unsigned long Client::getCRC() {
	return _crc_value;
}







