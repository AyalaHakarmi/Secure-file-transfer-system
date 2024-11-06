
# README for File Transfer Project

## Project Overview

This project implements a secure file transfer system using a client-server architecture. The server is developed in Python, while the client is implemented in C++. The primary goal of this project is to facilitate the secure transmission of files between clients and the server, ensuring data integrity and confidentiality through encryption.

## important points

1. **Unique Client Names**: Each client must have a unique name. If a client attempts to register with a name that is already in use on the server, an error will be returned. The client will need to change their name to proceed with the file transfer.

2. **File Overwriting**: When a client sends a file to the server, if a file with the same name already exists, the new file will overwrite the previous one. This ensures that clients can update their files without needing to delete the old ones manually.

3. **CRC Validation**: The system employs a CRC (Cyclic Redundancy Check) mechanism to verify the integrity of the files being transferred. If the CRC is not valid after four attempts, the server will delete the file. This feature helps maintain the reliability of the data stored on the server.

4. **Packet Size Limitation**: Each packet has a maximum size of 4096 bytes (4KB) to the "massage content" field. we have just 2 bytes to represent the number of the total packets. Consequently, the maximum file size that can be transmitted is limited to 4096 * (2^16 - 1) bytes. This limit can be adjusted by modifying the `MAX_PACKET_SIZE` constant in the `global.h` file.

5. **Version Compatibility**: Both the server and client must be running version 3 of the respective software. This ensures compatibility and smooth operation between the two components of the system.

6. **Client File Organization**: Files uploaded by each client will be stored on the server in a dedicated folder. A main directory called `clients_files` will contain individual subfolders for each client, named after the respective client's username. This ensures that each client's files are organized separately.

## Getting Started

To run the project, ensure that you have the necessary dependencies installed for both the server and client. Follow the instructions provided in the project documentation to set up the environment and execute the server and client applications.

## Video Demonstration

For a visual demonstration of the project in action, please refer to the following link: [Project Running Video](https://drive.google.com/file/d/182oYeoFrmD-m2yzXWk7TEXe5-szzJm70/view?usp=sharing).

## Conclusion

This file transfer project showcases the implementation of secure communication protocols and file management techniques. It is designed to be user-friendly while ensuring data integrity and security. 

Thanks a lot!!!!
