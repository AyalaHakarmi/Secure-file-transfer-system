#include "client.h"



//the main function of the client
int main(int argc, char* argv[])
{
	//create client object and the first request to the server
	Client client;
	Request request;
	//Initializes the variable to this answer, which may change later
	Answer_codes answer = VALID_RECONNECT_1605;
	read_transfer(client);
	// Checking whether to register or reconnect by checking if me file exists
	std::ifstream me_info_check("me.info");
	if (me_info_check.good())//me file exists- so reconnect
	{ 
		answer = reconnection(client, request);
	}
	if (!me_info_check.good() || answer == INVALID_RECONNECT_1606) { //me file not exists or reconnection failed so registering...
		if (me_info_check.is_open())  // Just in case, close if still open
		{
			me_info_check.close();
		}
		registering(client, request);
	}
	
}

//this function execute the registering proceess (and calls the forward functions that execute the program)
void registering(Client& client,Request& request) {
	std::string fake_id(32, '\0');
	request.setClientID(fake_id);
	request.setCode(REGISTERING_825);
	request.setPayloadSize(NAME_OF_CLIENT_SIZE);
	request.appendStringPayload(client.getClientName(), NAME_OF_CLIENT_SIZE);
	request.send_to_server(s);
	Answer answer;
	answer.get_header_from_server(s);
	if (answer.getCode() == GENERAL_ERROR_1607 || answer.getCode() == REGISTERING_FAILED_1601) {
		std::cout << "registering failed\n";
		if (general_error_handle(answer, request) == TRY_AGAIN) {
			request.clearPayload();
			registering(client, request);
			return;
		}
	}
	else if (answer.getCode() != REGISTERING_DONE_1600) {
		std::cout << "code " << answer.getCode() << " is not recognized.exit\n";
		exit(1);
	}
	client.setClientID(answer.getClientID(s));
	Request send_public_key_request;
	send_public_key_get_aes(client, send_public_key_request);
	Request send_file_request;
	send_file(client, send_file_request);
}

//this function create the RSA keys, send the public key to the server and get the aes key from the server
void send_public_key_get_aes(Client& client, Request& request) {
	/*create keys*/
	Base64Wrapper base64;
	RSAPrivateWrapper priv_key_ob;  
	client.setPrivateKey(priv_key_ob.getPrivateKey());
	create_me_info(base64, client.getClientName(), client.getPrivateKey(), client.getClientID());
	create_priv_key_file(base64, client.getPrivateKey());
	const std::string public_key = priv_key_ob.getPublicKey();
	request.setClientID(client.getClientID());
	request.setCode(SEND_PUBLIC_KEY_826);
	request.setPayloadSize(NAME_OF_CLIENT_SIZE + public_key.size() );
	request.appendStringPayload(client.getClientName(), NAME_OF_CLIENT_SIZE);
	request.appendStringPayload(public_key, public_key.size());
	request.send_to_server(s);
	Answer answer;
	answer.get_header_from_server(s);
	if (answer.getCode() == GENERAL_ERROR_1607) {
		if (general_error_handle(answer, request) == TRY_AGAIN) {
			request.clearPayload();
			send_public_key_get_aes(client, request);
			return;
		}
	}
	else if (answer.getCode() != GET_PUBLIC_KEY_SEND_AES_1602) {
		std::cout << "code " << answer.getCode() << " is not recognized.exit\n";
		exit(1);
	}
	std::cout << "Public key sent to server\n";
	get_aes_key_from_server(client, answer);
}

// This function read the information from "transfer.info" and make connection with the server
void read_transfer(Client& client)
{
	std::ifstream transfer;
	std::string connection_details, ip_address, port, file_name, client_name,path_to_file;
	transfer.open("transfer.info");
	if (transfer.is_open()) /*succeed to open transfer file*/
	{
		getline(transfer, connection_details);
		getline(transfer, client_name);
		//check the client's name length
		if (client_name.length() > NAME_OF_CLIENT_MAX_SIZE)
		{
			std::cerr << "Error: 'name' exceeds 100 characters. Please check the transfer.info file." << std::endl;
			transfer.close();
			exit(1); 
		}
		client.setClientName(client_name);
		getline(transfer, path_to_file);
		file_name = find_file_name(path_to_file);
		transfer.close();
		//checks that its size + \0 is not more then FILE_NAME_SIZE
		if (file_name.size() == FILE_NAME_SIZE) {
			std::cerr << "Error: 'file name' is too long. Please check the transfer.info file." << std::endl;
			exit(1);  
		}
		client.setFileName(file_name);
		client.setPath(path_to_file);
		//get the ip and port from the first line
		size_t index = connection_details.find(':');
		ip_address = connection_details.substr(0, index);
		port = connection_details.substr(index + 1);
	}
	else
	{
		std::cout << "'transfer.info' not exists. exit.\n";
		exit(1);
	}
	boost::asio::connect(s, resolver.resolve(ip_address, port)); /*connect with ip and port*/
	std::cout << client_name<< " connected to the server.\n";
}

//reconnect to server  (and calls the forward functions that execute the program)
Answer_codes reconnection(Client& client, Request& request) {
	//read the key from priv.key
	client.setPrivateKey(read_priv_key());
	std::ifstream me_info;
	std::string client_name, client_id_hex;
	me_info.open("me.info"); /*me exists (checked in main function)*/
	getline(me_info, client_name);
	if (std::string(client.getClientName()) != client_name) {
		std::cout << "Client's name in me.info does not match Client's name in transfer.info.\n";
		me_info.close();
		exit(1);
	}
	getline(me_info, client_id_hex);
	me_info.close();
	client.setClientID(convert_hex_to_bytes(client_id_hex));
	//prepare the request header values
	request.setClientID(client.getClientID());
	request.setCode(RECONNECT_827);
	request.setPayloadSize(NAME_OF_CLIENT_SIZE);
	//prepare the request payload
	request.appendStringPayload(client.getClientName(), NAME_OF_CLIENT_SIZE);
	request.send_to_server(s);
	Answer answer;
	//get the header of the server response
	answer.get_header_from_server(s);
	if (answer.getCode() == GENERAL_ERROR_1607){
		if (general_error_handle(answer, request) == TRY_AGAIN) {
			//try to reconnect again
			request.clearPayload();
			return reconnection(client, request);
		}
	}
	else if (answer.getCode() == INVALID_RECONNECT_1606){
		//The payload is null...
		uint8_t buffer[CLIENT_ID_SIZE];
		size_t length = boost::asio::read(s, boost::asio::buffer(buffer, CLIENT_ID_SIZE));
		std::cerr << "Reconnection failed. Try to regist\n";
		request.clearPayload();
		request.restartFailures(); /*new request (registering) so there is 0 failures */
		return (Answer_codes)answer.getCode();
	}
	else if (answer.getCode() != VALID_RECONNECT_1605) {
		std::cout << "code " << answer.getCode() <<" is not recognized.exit\n";
		exit(1);
	}
	get_aes_key_from_server(client, answer);
	Request send_file_request;
	send_file(client, send_file_request);
	return VALID_RECONNECT_1605;
}
//this function get the aes key from the server 
void get_aes_key_from_server(Client& client, Answer& answer) {
	//checks that the ID match my ID
	answer.read_string_and_check(s,client.getClientID(), CLIENT_ID_SIZE);
	//get the AES
	client.setEncryptedAes(answer.getEncryptedAes(s));
	std::cout << "AES key accepted\n";
}

//this function handle the file sending process (with packets deviding)
void send_file(Client& client, Request& request) {
	//read the file
	std::vector<char> original_file = file_by_path(client.getPath());
	client.setCRC(memcrc(original_file.data(), original_file.size())); /*claculate crc of file*/
	// encrypt the file according to the aes key
	std::string encrypted_file = encrypt_file(client.getEncryptedAes(), client.getPrivateKey(), original_file);
	request.setClientID(client.getClientID());
	request.setCode(SEND_FILE_828);
	//calculating the payload size exclude the field "massage content"
	int payload_size_without_massage = CONTENT_SIZE_SIZE + ORIG_FILE_SIZE_SIZE + PACKET_NUMBER_SIZE + TOTAL_PACKETS_SIZE + FILE_NAME_SIZE;
	int packet_number = 1;
	int total_packets = calculate_total_packets(encrypted_file.size());
	int encrypted_file_size_counter = encrypted_file.size();
	//The indexes store the start and end point of this packet 
	int begin = 0;
	int end = PACKET_MAX_SIZE;
	while(encrypted_file_size_counter > 0){
		request.setPayloadSize(payload_size_without_massage + encrypted_file_size_counter);// in the last packet
		if (encrypted_file_size_counter > PACKET_MAX_SIZE) {
			// If the amount left to send is bigger then PACKET_MAX_SIZE , we will overwrite the payload size
			request.setPayloadSize(payload_size_without_massage + PACKET_MAX_SIZE);
		}
		request.appendNumberPayload((int)encrypted_file.size(), CONTENT_SIZE_SIZE);
		request.appendNumberPayload((int)original_file.size(), ORIG_FILE_SIZE_SIZE);
		request.appendNumberPayload(packet_number, PACKET_NUMBER_SIZE);
		request.appendNumberPayload(total_packets, TOTAL_PACKETS_SIZE);
		request.appendStringPayload(client.getFileName(), NAME_OF_CLIENT_SIZE);
		//Writing the appropriate file part
		if (encrypted_file_size_counter > PACKET_MAX_SIZE) {
			request.appendFileSlice(encrypted_file, begin, end);
		}
		else {
			// in the last packet
			request.appendFileSlice(encrypted_file, begin, begin + encrypted_file_size_counter);
		}
		//sent this packet to the server
		request.send_to_server(s);
		std::cout << "Packet number " << packet_number << " of " << total_packets << " sent\n";
		request.clearPayload();// clear before the next packet
		packet_number++;
		begin += PACKET_MAX_SIZE;//update the indexes
		end += PACKET_MAX_SIZE;
		encrypted_file_size_counter -= PACKET_MAX_SIZE;
	}
	//finish request is the request the client will send after the response from the server about the CRC
	Request finish_request;
	int finish_code = crc_checking(client, request, (int)encrypted_file.size(), finish_request);
	if(finish_code == WRONG_CRC_TRY_AGAIN_901 || finish_code == TRY_AGAIN ) {
		request.clearPayload();
	    send_file(client, request);
	}
	else {//result is wrong crc 4th (902) or valid crc (900)...
		the_end(client, finish_code, finish_request);
	}
}

//this function send the last request ( valid or invalid crc) and gets the last response
void the_end(Client& client, int result, Request& finish_request) {
	Answer answer;
	answer.get_header_from_server(s);
	if (answer.getCode() == GENERAL_ERROR_1607) {
		if (general_error_handle(answer, finish_request) == TRY_AGAIN) {
			//try again...
			finish_request.send_to_server(s);
			the_end(client, result, finish_request);
			return;
		}
	}
	else if (answer.getCode() != THANK_YOU_1604) {
		std::cout << "code " << answer.getCode() << " is not recognized.exit\n";
		exit(1);
	}
	// checks if the ID from server is same as client's ID
	answer.read_string_and_check(s, client.getClientID(), CLIENT_ID_SIZE);
	if (result == VALID_CRC_900) {
		std::cout << "The proccess end succesfuly.\n";
	}
	else if (result == WRONG_CRC_4TH_902){
		std::cout << "Client tried to send the file 4 times. the proccess end\n";
	}
}

//this function gets the answer with the crc value and checks if the crc is valid
int crc_checking(Client& client, Request& crc_packet_request , int encrypted_file_size, Request& finish_request) {
	Answer answer; 
	answer.get_header_from_server(s);
	if (answer.getCode() == GENERAL_ERROR_1607) {
		return general_error_handle(answer, crc_packet_request);
	}
	else if (answer.getCode() != VALID_FILE_AND_CRC_1603) {
		std::cout << "code " << answer.getCode() << " is not recognized.exit\n";
		exit(1);
	}
	else { // if (answer.getCode() == VALID_FILE_AND_CRC_1603)
		answer.read_string_and_check(s, client.getClientID(), CLIENT_ID_SIZE);
		if (encrypted_file_size != answer.getNumber(s, CONTENT_SIZE_SIZE)) {
			std::cerr << "server sent wrong information (content size dont match my content size). exit \n";
			exit(1);
		}
		// check if the file name from the server match to the client's file name
		answer.read_string_and_check(s, client.getFileName(), FILE_NAME_SIZE);
		finish_request.setClientID(client.getClientID());
		int finish_code = 0; // will change later...
		if (client.getCRC() != answer.getNumber(s, CRC_SIZE)) {// checks the CRC
			std::cout << "CRC value sent by server is wrong .\n";
			crc_packet_request.addFailure();
			if (crc_packet_request.getNumFailurs() == MAX_TRIES) {
				finish_code = WRONG_CRC_4TH_902;//Already tried 4 times
			}
			else {
				//So we still didnt tried to send MAX_TRIES
				std::cout << "Client tried to send the file " << crc_packet_request.getNumFailurs() << " times.try again\n";
				finish_code = WRONG_CRC_TRY_AGAIN_901;
			}
		}
		else {
			std::cout << "valid CRC\n";
			finish_code = VALID_CRC_900;
		}
		finish_request.setCode(finish_code);
		finish_request.setPayloadSize(NAME_OF_FILE_SIZE);
		finish_request.appendStringPayload(client.getFileName(), NAME_OF_CLIENT_SIZE);
		finish_request.send_to_server(s);
		return finish_code;
	}
}

// this function handle situation of general error answer from the server ( try again or exit..)
int general_error_handle(Answer& answer, Request& request) {
	std::cout << "server responded with an error (code is: " << answer.getCode() << ").\n";
	request.addFailure();
	if (request.getNumFailurs() == MAX_TRIES) {
		std::cout << "Client tried to send request number "<< request.getCode() << " " << MAX_TRIES << " times.exit\n";
		exit(1);
	}
	std::cout << "Client tried to send request number " << request.getCode() << " " << request.getNumFailurs() << " times.try again\n";
	return TRY_AGAIN;
}



