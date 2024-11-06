#include "answer.h"

//this function gets all the header fiels from the server and store it in the object
void Answer::get_header_from_server(tcp::socket& s) {
	boost::endian::little_uint8_buf_t buffer_version;
	boost::endian::little_uint16_buf_t buffer_code;
	boost::endian::little_uint32_buf_t buffer_payload_size;
	try
	{
		size_t version_size = boost::asio::read(s, boost::asio::buffer(buffer_version.data(), VERSION_SIZE));
		size_t code_size = boost::asio::read(s, boost::asio::buffer(buffer_code.data(), CODE_SIZE));
		size_t payload_size_size = boost::asio::read(s, boost::asio::buffer(buffer_payload_size.data(), PAYLOAD_SIZE_SIZE));
	}
	catch (std::exception& e)
	{
		std::cerr << "Exception while reading from server: " << e.what() << " exit.\n";
		exit(1);
	}
	_version = buffer_version.value();
	//version checking
	if (_version != VERSION){
		std::cerr << "Error: server's version is not " << VERSION << ".exit.\n";
		exit(1);
	}
	_code = buffer_code.value();
	_payload_size = buffer_payload_size.value();
}

//this function returns the code from the header
int Answer::getCode() {
	return _code;
}

//this function gets string type from the server and make sure it matchs the expected_string
void Answer::read_string_and_check(tcp::socket& s, std::string expected_string, int size) {
	try {
		// Create a string of the specified size, filled with null characters ('\0')
		std::string received_string(size, '\0');

		// Read data directly into the string's internal buffer
		boost::asio::read(s, boost::asio::buffer(received_string.data(), received_string.size()));

		// Trim off any trailing null characters
		received_string = received_string.substr(0, received_string.find_first_of('\0'));

		// Check if the received string matches the expected string
		if (received_string != expected_string) {
			std::cerr << "Server sent wrong information: (" << received_string << " instead of " << expected_string<< " ).Exiting.\n";
			exit(1);
		}
	}
	catch (const std::exception& e) {
		std::cerr << "Exception: " << e.what() << " Exiting.\n";
		exit(1);
	}
}


//this function gets the AES key from the server and return it as is 
std::string Answer::getEncryptedAes(tcp::socket& s) {
	try
	{
		std::string encrypted_aes(_payload_size - CLIENT_ID_SIZE, '\0');
		size_t reply_length2 = boost::asio::read(s, boost::asio::buffer(encrypted_aes.data(), encrypted_aes.size()));
		return encrypted_aes;
	}
	catch (std::exception& e)
	{
		std::cerr << "Exception: " << e.what() << " exit.\n";
		exit(1);
	}
}

//this function gets number type from the server (little endian) and return it as int
unsigned long Answer::getNumber(tcp::socket& s, int num_of_bytes) {
	 boost::endian::little_uint32_buf_t buffer;
	 try
	 {
		 size_t size = boost::asio::read(s, boost::asio::buffer(buffer.data(), num_of_bytes));
	 }
	 catch (std::exception& e)
	 {
		 std::cerr << "Exception: " << e.what() << " exit.\n";
		 exit(1);
	 }
	 return buffer.value();
}
//this function gets the client id from the server
std::string Answer::getClientID(tcp::socket& s) {
	try {
		// Create a string of the appropriate size, filled with null bytes ('\0')
		std::string client_id(CLIENT_ID_SIZE, '\0');

		// Read data directly into the string's internal buffer
		boost::asio::read(s, boost::asio::buffer(client_id.data(), client_id.size()));

		std::cout << "Registration succeeded. UUID accepted." << std::endl;

		return client_id;  // Return the client ID as a string
	}
	catch (const std::exception& e) {
		std::cerr << "Exception: " << e.what() << " exit.\n";
		exit(1);
	}
}


