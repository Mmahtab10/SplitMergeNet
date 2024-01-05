/*  Author: Mohammad Mahtab Uddin Khan
    UCID : 30102408
    Email: mohammadmahtab.khan@ucalgary.ca
    Date of Submission: Nov 15, 2023

    Citation:

    1) There are certain blocks of code for UDP socket programming taken from Sadaf Erfanmanesh's notion site found at https://sadafem.notion.site/UDP-Socket-Programming-in-C-fc4757cd1e8f4faea223b6898eafef4d
    
    2) TCP socket programming code referred from Sadaf Erfanmanesh's 
    HttpClient-exersice1.cpp and HttpServer-exercise2.cpp available via the following url: https://d2l.ucalgary.ca/d2l/le/content/542615/viewContent/6101422/View

    3) Program requirements and instructions created by Janet Leahy can be found on DL site: https://d2l.ucalgary.ca/d2l/le/content/542615/viewContent/6142196/View
*/
#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>

using namespace std;

bool isVowel(char c) {
    c = tolower(c);
    // Check if the character is a vowel
    return (c == 'a' || c == 'e' || c == 'i' || c == 'o' || c == 'u');
}

bool isDigit(char c) {
    return isdigit(static_cast<unsigned char>(c)) != 0;
}

void mergeText_basic(const string& consonants, const string& vowels, string& reconstructed) {
    // Assuming both strings are of equal length
    size_t length = consonants.length();

    // Initialize the reconstructed string with spaces
    reconstructed.resize(length, ' ');

    // Fill in consonants and vowels at their respective positions
    for (size_t i = 0; i < length; i++) {
        if (consonants[i] != ' ') {
            reconstructed[i] = consonants[i];
        }
        if (vowels[i] != ' ') {
            reconstructed[i] = vowels[i];
        }
    }
}

void mergeText_advanced(const string& consonants, const string& vowels, string& reconstructed){
    reconstructed.clear();

    size_t consonantIndex = 0;
    size_t vowelIndex = 0;
    
    while (consonantIndex < consonants.size() || vowelIndex < vowels.size()) {
        if (isDigit(vowels[vowelIndex])) {
            size_t count = 0;
            // Extract the count of spaces before the vowel
            while (vowelIndex < vowels.size() && isDigit(vowels[vowelIndex])) {
                count = count * 10 + (vowels[vowelIndex] - '0');
                ++vowelIndex;
            }
            // Add the count of spaces before the vowel
            for (size_t i = 0; i < count; ++i) {
                reconstructed += consonants[consonantIndex];
                consonantIndex++;
            }
            // Add the vowel itself
            if (vowelIndex < vowels.size()) {
                reconstructed += vowels[vowelIndex];
            }
            ++vowelIndex;
        }else {
            // Add the consonant
            if (consonantIndex < consonants.size()) {
                reconstructed += consonants[consonantIndex];
            }
            ++consonantIndex;
        }
    }
}

void splitText_basic(const string& sentence, string& consonants, string& vowels) {
    for (size_t i = 0; i < sentence.size(); ++i) {
        char c = sentence[i];
        if (isVowel(c)) {
            vowels.push_back(c);
            consonants.push_back(' ');  // Add a space in consonant sentence for each vowel
        } else {
            consonants.push_back(c);
            vowels.push_back(' ');  // Add a space in vowel sentence for each consonant
        }
    }
}

void splitText_advanced(const string& sentence, string& consonants, string& vowels){
    consonants.clear();
    vowels.clear();
    size_t count = 0;

    for (size_t i = 0; i < sentence.size(); ++i) {
        char c = sentence[i];

        if (isVowel(c)) {
            c = tolower(c);
            vowels += to_string(count);
            vowels += c;
            count = 0;
        } else{
            count++;
            consonants += c;
        }
    }
}

int main(int argc, char* argv[]) {
    string portStr = argv[1];
    int udpPort = stoi(portStr);
    cout << "UDP PORT: " << udpPort << endl;

    string tcpPortStr = argv[2];
    int tcpPort = stoi(tcpPortStr);
    cout << "TCP PORT: " << tcpPort << endl;
   
    // Create a UDP socket
    int udpSocket = socket(AF_INET, SOCK_DGRAM, 0);
    if (udpSocket == -1) {
        cerr << "Failed to create UDP socket" << endl;
        return -1;
    }
    //Create a TCP socket
    int tcpSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (tcpSocket == -1) {
        cerr << "Error creating TCP socket" << endl;
        return 1;
    }
    // Set up UDP server address structures    
    struct sockaddr_in udpServerAddress;
    memset(&udpServerAddress, '\0', sizeof(udpServerAddress));
    udpServerAddress.sin_family = AF_INET;
    udpServerAddress.sin_addr.s_addr = inet_addr("127.0.0.1"); 
    udpServerAddress.sin_port = htons(udpPort);

    // Configure the UDP socket, including setting non-blocking mode if needed.
    // For timeout implementation, non-blocking mode can be helpful.
    struct timeval timeout;
    timeout.tv_sec = 5; // Set a timeout of 5 seconds
    timeout.tv_usec = 0;
    setsockopt(udpSocket, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)); 
    // Set up server address structures    
    struct sockaddr_in tcpServerAddress;
    memset(&tcpServerAddress, '\0', sizeof(tcpServerAddress));
    tcpServerAddress.sin_family = AF_INET;
    tcpServerAddress.sin_addr.s_addr = INADDR_ANY; // Listen on all available network interfaces
    tcpServerAddress.sin_port = htons(tcpPort);

    struct sockaddr_in clientAddress;
    socklen_t clientAddressLength = sizeof(clientAddress);

    // Bind the UDP and TCP socket to an address and port
    if (bind(udpSocket, (struct sockaddr*)&udpServerAddress, sizeof(udpServerAddress)) < 0 ||
        bind(tcpSocket, (struct sockaddr*)&tcpServerAddress, sizeof(tcpServerAddress)) < 0 ) {
        cerr << "Failed to bind TCP sockets" << endl;
        return -1;
    }
    
    while(1){
        // ****************** Receive data from clients using TCP ************************
        // Listening for client connections, with a maximum of 3 pending connections
        if (listen(tcpSocket, 3) < 0) {
            perror("listen");
            exit(EXIT_FAILURE);
        }
        cout << endl << "*********************************************************" << endl;
        cout << endl << "UDP Server is listening for incoming datagrams..." << endl;
        cout << "TCP Server is listening on port: " << tcpPort << endl;
        char tcpBuffer[1024];
        bzero(tcpBuffer, 1024);
        // Accepting a client connection
        int new_socket;
        if ((new_socket = accept(tcpSocket, (struct sockaddr *)&clientAddress, &clientAddressLength)) < 0) {
            perror("accept");
            exit(EXIT_FAILURE);
        }
        // Read menu option from client
        char menuBuffer[1];
        ssize_t menuRead = recv(new_socket, menuBuffer, sizeof(menuBuffer), 0);
            if (menuRead < 0) {
            perror("recv");
            exit(EXIT_FAILURE);
        }
        // Reading the message from the client using TCP
        ssize_t bytesRead = recv(new_socket, tcpBuffer, sizeof(tcpBuffer), 0);
            if (bytesRead < 0) {
            perror("recv");
            exit(EXIT_FAILURE);
        }

        // ******************* Receive data from clients using UDP ************************
        char udpBuffer[1024];
        memset(udpBuffer, '\0', sizeof(udpBuffer));

        ssize_t bytesReceived = recvfrom(udpSocket, udpBuffer, sizeof(udpBuffer), 0, (struct sockaddr*)&clientAddress, &clientAddressLength);
        if (bytesReceived == -1){
            cout << "Failed to receive data by UDP" << endl;
        }
        else{
            udpBuffer[bytesReceived] = '\0'; // Null-terminate the received data
        }
        cout << endl <<  "Client's UDP message is :" << udpBuffer << endl;
        cout << endl <<  "Client's TCP message is :" << tcpBuffer << endl;

        // ***************** Modify Data *************************************
        string consonants, vowels, reconstructedStr;
        // Convert menu option to an integer variable
        int menuOption = stoi(menuBuffer);

        // Using a switch to perform different operations based on menu input
        switch(menuOption){
            case 1:
                // Tcp response contians the text to be split
                splitText_basic(tcpBuffer, consonants, vowels);
                break;
            case 2:
                // Assign the tcp response to consonants and udp response to vowels
                consonants = tcpBuffer;
                vowels = udpBuffer;
                mergeText_basic(consonants, vowels, reconstructedStr);
                cout << "Merged Text: " << reconstructedStr << endl;
                break;
            case 3:
                // Tcp response contians the text to be split
                splitText_advanced(tcpBuffer, consonants, vowels);
                break;
            case 4:
                // Assign the tcp response to consonants and udp response to vowels
                consonants = tcpBuffer;
                vowels = udpBuffer;
                mergeText_advanced(consonants, vowels, reconstructedStr);
                cout << "Merged Text: " << reconstructedStr << endl;
                break;
            default:
                cout << "erroneous menu input";   
                break; 
        }
        
        // ***************** Send data to clients using UDP *********************
        const char* udpMessage;
        if (!vowels.empty()){
            udpMessage = vowels.c_str();
        }
        else{
            udpMessage = "UDP Response";
        }
        ssize_t bytesSentUDP = sendto(udpSocket, udpMessage, strlen(udpMessage), 0, (struct sockaddr*)&clientAddress, sizeof(clientAddress));
        if (bytesSentUDP == -1) {
            cerr << "Failed to send data to the server" << endl;
        }
        // Send data to clients using TCP
        const char* tcpMessage; 
        if(menuOption == 1 || menuOption == 3){
            tcpMessage = consonants.c_str();
        }
        else{
            tcpMessage = reconstructedStr.c_str();
        }
        ssize_t bytesSentTCP = send(new_socket, tcpMessage, strlen(tcpMessage), 0);
        cout << endl << "Message sent over UDP to client: " << udpMessage << endl;
        cout << "Message sent over TCP to client: " << tcpMessage << endl;
        close(new_socket);
    }
    // Close the sockets
    close(udpSocket);
    close(tcpSocket);
}