#include "various_functions.h"

//this function calculate the number of the packets to be send according to the size of the file
int calculate_total_packets(int file_size) {
	int total_packets = (int)file_size / PACKET_MAX_SIZE;
	if ((file_size % PACKET_MAX_SIZE) != 0) {// So we need extra packet
		total_packets++;
	}
	if (total_packets > std::numeric_limits<std::uint16_t>::max()) {
		// If the total packets is too big for representing in 2 bytes so the file is too big
		std::cout << "File size is too big: " << file_size << ". Exiting.\n";
		exit(1);
		
	}
	return total_packets;
}
// This function find the file name in the path from the transfer.info file
std::string find_file_name(std::string path_to_file) {
	size_t pos_file_name = path_to_file.find_last_of('\\');
	if (pos_file_name == std::string::npos) /*path name is simply the file name*/
	{
		return path_to_file;
	}
	else
	{
		//take the name of the file only
		return path_to_file.substr(pos_file_name + 1);
	}
}

//this function read private key from 'priv.key' file
std::string read_priv_key()
{
	// Check if the file exists
	if (!std::filesystem::exists("priv.key")) {
		std::cerr << "Error: 'priv.key' file does not exist. Exiting" << std::endl;
		exit(1);
	}
	std::ifstream priv_key;
	priv_key.open("priv.key");
	std::string res, private_key;
	while (getline(priv_key, res))
	{
		private_key += res;
	}
	priv_key.close();
	Base64Wrapper b64wrapper;
	return b64wrapper.decode(private_key);

}
//this function convert hex string to bytes string
std::string convert_hex_to_bytes(const std::string& hex)
{
	std::string res;
	for (auto i = 0u; i < hex.length(); i += 2)
	{
		std::string byteString = hex.substr(i, 2);
		char byte = (char)strtol(byteString.c_str(), NULL, CLIENT_ID_SIZE);
		res += byte;
	}
	return res;
}


//this file gets the encrypted aes key from server and the private key, decrypt the aes key and encrypt the file that need to be encrypted
std::string encrypt_file(std::string encrypted_aes, std::string priv_key, std::vector<char> file)
{
	RSAPrivateWrapper priv_key_ob(priv_key);
	std::string decrypted_aes = priv_key_ob.decrypt(encrypted_aes); /*open the aes key with the private key*/

	AESWrapper aes_key_ob((unsigned char*)decrypted_aes.data(), (unsigned int)size(decrypted_aes));
	std::string encrypted_file = aes_key_ob.encrypt(file.data(), file.size()); /*encrypt file*/

	return encrypted_file;
}

//Get the content of the file 
std::vector<char> file_by_path(const std::string& path) {
	// Open the file in binary mode
	std::ifstream file(path, std::ios::binary);
	if (!file.is_open()) {
		std::cout << "File " << path << " does not exist. Exiting.\n";
		exit(1);
	}

	// Move to the end of the file to determine its size
	file.seekg(0, std::ios::end);
	std::streamsize size = file.tellg();
	file.seekg(0, std::ios::beg);

	// Read the file content into a vector
	std::vector<char> buffer(size);
	if (!file.read(buffer.data(), size)) {
		std::cout << "Error reading file " << path << ".\n";
		exit(1);
	}

	file.close();
	return buffer;
}

//this function creates 'me.info' file and write to the file the client name, the uuid get from server and the private key in base 64
void create_me_info(Base64Wrapper base64, std::string client_name, std::string private_key, std::string client_id)
{
	std::ofstream me_info;
	me_info.open("me.info");
	me_info << client_name << std::endl;
	me_info << convert_string_to_hex(client_id) << std::endl;
	me_info << base64.encode(private_key) << std::endl;
	me_info.close();
}

//this function converts string to hex representation 
std::string convert_string_to_hex(const std::string& string)
{
	try
	{
		std::stringstream result;
		result << std::hex;
		for (size_t i = 0; i < string.size(); ++i)
		{
			result << std::setw(2) << std::setfill('0') << (int)(uint8_t)string[i];
		}
		return result.str();
	}
	catch (...)
	{
		std::cout << "Error converting to hex" << std::endl;
		exit(1);
	}
}

//this function creates 'priv.key' file and write to the file the private key in base 64
void create_priv_key_file(Base64Wrapper base64,std::string private_key)
{
	std::ofstream priv_key;
	priv_key.open("priv.key");
	priv_key << base64.encode(private_key) << std::endl;
	priv_key.close();
}


