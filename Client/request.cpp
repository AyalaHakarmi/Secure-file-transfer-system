#include "request.h"

//this function returns the code of the request
int Request::getCode() {
	int code_as_int = 0;
	std::memcpy(&code_as_int, _code, sizeof(_code));
	return code_as_int;
}

//this function set client ID (in the header)
void Request::setClientID(std::string id) {
	for (int i = 0; i < CLIENT_ID_SIZE; i++) /*insert id to the vector will be sent to server*/
	{
		_client_id[i]  = id[i];
	}
}
//this function set the code (in the header)
void Request::setCode(int code) {
	boost::endian::store_little_u16(_code, (uint16_t)code);
}
//this function set the payload size ( in the header)
void Request::setPayloadSize(int size) {
	boost::endian::store_little_u32(_payload_size, (uint32_t)size);
}
// appenf string or bytes to the payload
void Request::appendStringPayload(std::string payload, int wanted_size) {
	for (int i = 0; i < payload.size(); i++) {
		_payload.push_back(payload[i]);
	}
	for (int i = payload.size(); i < wanted_size; i++) {
		_payload.push_back('\0');// padding with \0
	}
}

//this function send the request to the server
void Request::send_to_server(tcp::socket& s){
	std::vector <uint8_t> massage;// the whole request
	//all the numbers is in little endian
	for (int i = 0; i < CLIENT_ID_SIZE; i++) {
		massage.push_back(_client_id[i]);
	}
	for (int i = 0; i < VERSION_SIZE; i++) {
		massage.push_back(_version[i]);
	}
	for (int i = 0; i < CODE_SIZE; i++) {
		massage.push_back(_code[i]);
	}
	for (int i = 0; i < PAYLOAD_SIZE_SIZE; i++) {
		massage.push_back(_payload_size[i]);
	}
	for (int i = 0; i < _payload.size(); i++) {
		massage.push_back(_payload[i]);
	}
	try
	{
		//write to server
		boost::asio::write(s, boost::asio::buffer(massage, REQUEST_HEADER_SIZE + _payload.size())); /*send vector to server*/
	}
	catch (std::exception& e)
	{
		std::cerr << "Exception in client: " << e.what() << " exit\n";
	}
}
//this function returns the number of times that this request send to server and got error in response
int  Request::getNumFailurs() {
	return _num_of_failures;
}
//add failure to the failures counter
void Request::addFailure() {
	_num_of_failures++;
}
//this function set the number of failures to 0
void Request::restartFailures() {
	_num_of_failures = 0;
}
//this function clear the payload of the request
void Request::clearPayload() {
	_payload = std::vector<uint8_t>();  // Assign an empty vector
}
//this function append number to the pauload in little endian
void Request::appendNumberPayload(int num,  int size) {
	std::vector<uint8_t> content(size);
	switch (size)
	{
	case SIZE_32_BITS:// 4 bytes
		boost::endian::store_little_u32(content.data(), (uint32_t)num);
		break;  
	case SIZE_16_BITS:// 2 bytes
		boost::endian::store_little_u16(content.data(), (uint16_t)num);
		break;
	default:
		break;
	}
	for (int i = 0; i < size; i++)
	{
		_payload.push_back(content[i]);
	}
}
// this function add the massage content to the payload
void Request::appendFileSlice(std::string encrypted_file, int begin, int end) {
	for (int i = begin; i < end; i++)
	{
		_payload.push_back(encrypted_file[i]);
	}
}
