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
#include <string>
#include <cstdlib>
using namespace std;
// s is the tcp socket
// u is the udp socket
string displayMenu(int s, int u, char& menuInput) {
    cout << "\n*****************  WELCOME! *****************\n";
    cout << "Select from the following options:\n1 -- Split text (basic encoding)\n2 -- Merge text (basic encoding)\n3 -- Split text (advanced encoding)\n4 -- Merge text (advanced encoding)\n5 -- Quit\n";

    cin >> menuInput;
    cin.ignore();

    if (menuInput == '5') {
        close(s);
        close(u);
        exit(0);
    }
    else if(menuInput == '2' || menuInput == '4'){
        // Process merge menu options
        cout << "Enter your consonants: ";
        string consonantsInput, vowelsInput;
        getline(cin, consonantsInput);
        cout << "Enter your vowels: ";
        getline(cin, vowelsInput);
        string userInput = consonantsInput + "~" + vowelsInput;  
        return userInput;
    }
    else{
        //Process split menu options
        string userInput;
        cout << "Enter your string to split: ";
        getline(cin, userInput);
        return userInput;
    }
}

int main(int argc, char* argv[]) {

    // *** Read commandline arguments ***
    string portStr = argv[1];
    int udpPort = stoi(portStr);

    string tcpStrPort = argv[2];
    int tcpPort = stoi(tcpStrPort);

    const char* ipAddress = argv[3];

    cout << "UDP PORT: " << udpPort << endl << "TCP PORT: " << tcpPort << endl << "IP: " << ipAddress << endl << "****************************************" << endl;

    while(1){
        // Create a UDP socket
        int udpSocket = socket(AF_INET, SOCK_DGRAM, 0);
        if (udpSocket == -1) {
            cerr << "Failed to create UDP socket" << endl;
            return -1;
        }
        // Create a TCP socket
        int tcpSocket = socket(AF_INET, SOCK_STREAM, 0);
        if (tcpSocket == -1){
            cerr << "Failed to create TCP socket" << endl;
            return -1;
        }
        // UDP Server address and port
        struct sockaddr_in serverAddress;
        memset(&serverAddress, '\0', sizeof(serverAddress));
        serverAddress.sin_family = AF_INET;
        serverAddress.sin_port = htons(udpPort);  // Replace with the server's port number
        serverAddress.sin_addr.s_addr = inet_addr(ipAddress); // Replace with the server's IP address

        // TCP Server address and port
        struct sockaddr_in tcpServerAddress;
        tcpServerAddress.sin_family = AF_INET;
        tcpServerAddress.sin_port = htons(tcpPort);  // Replace with the server's port number
        tcpServerAddress.sin_addr.s_addr = inet_addr(ipAddress); // Replace with the server's IP address

        // Configure the UDP socket, including setting non-blocking mode if needed.
        // For timeout implementation, non-blocking mode can be helpful.
        struct timeval timeout;
        timeout.tv_sec = 5; // Set a timeout of 5 seconds
        timeout.tv_usec = 0;
        setsockopt(udpSocket, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));

        // Connect TCP socket to the server
        if (connect(tcpSocket, (struct sockaddr *)&tcpServerAddress, sizeof(tcpServerAddress)) < 0) {
            perror("Connection Failed");
            return -1;
        }
        cout << "Connection successful" << endl;
        
        char menuInput;
        memset(&menuInput, 0, sizeof(menuInput));
        string userInputString = displayMenu(tcpSocket, udpSocket, menuInput);
        string vowels, consonants;
        size_t pos = userInputString.find("~");
        if (pos != string::npos){
            consonants = userInputString.substr(0, pos);
            vowels = userInputString.substr(pos+1);
        }

         // Send menu input to server
        ssize_t menuTCP = send(tcpSocket, &menuInput, sizeof(char), 0);
        if (menuTCP == -1) {
            cerr << "Failed to send menu input data to the server" << endl;
            return -1;
        }
        const char* tcpMessage;
        if(menuInput == '1' || menuInput == '3'){
            tcpMessage = userInputString.c_str();
        }
        else{
            tcpMessage = consonants.c_str();
        }
        // Send consonants to the server using TCP
        ssize_t bytesSentTCP = send(tcpSocket, tcpMessage, strlen(tcpMessage), 0);
        if (bytesSentTCP == -1) {
            cerr << "Failed to send data to the server" << endl;
            return -1;
        }
        else{
            cout << "Message sent to server over TCP: " << tcpMessage << endl;
        }
        
        // Send vowels to the server using UDP
        if (!vowels.empty()){
            const char* vow = vowels.c_str();
            ssize_t bytesSentUDP = sendto(udpSocket, vow, strlen(vow), 0, (struct sockaddr*)&serverAddress, sizeof(serverAddress));
            if (bytesSentUDP == -1) {
                cerr << "Failed to send data to the server" << endl;
            }
            else{
                cout << "Message sent to server over UDP: " << vow << endl;
            }
        }
        else{
            const char* txt = "UDP Test";
            ssize_t bytesSentUDP = sendto(udpSocket, txt, strlen(txt), 0, (struct sockaddr*)&serverAddress, sizeof(serverAddress));
        }

        // Receive response from the server using TCP
        char tcpBuffer[1024];
        bzero(tcpBuffer, 1024);
        ssize_t bytesRead = recv(tcpSocket, tcpBuffer, sizeof(tcpBuffer), 0);
        if (bytesRead == -1) {
            std::cerr << "Error reading from server by TCP" << std::endl;
        } else if (bytesRead == 0) {
            std::cout << "Server closed the connection" << std::endl;
        } else {
            tcpBuffer[bytesRead] = '\0';  // Null-terminate the received data
        }

        // Receive response from the server using UDP
        char udpBuffer[1024];
        memset(udpBuffer, '\0', sizeof(udpBuffer));
        socklen_t serverAddressLength = sizeof(serverAddress);
        ssize_t bytesReceived = recvfrom(udpSocket, udpBuffer, sizeof(udpBuffer), 0, (struct sockaddr*)&serverAddress, &serverAddressLength);
        if (bytesReceived == -1){
            cout << "UDP timed out. Will return to main menu now. " << endl; 
            continue;
        }
        else{
            udpBuffer[bytesReceived] = '\0'; // Null-terminate the received data
        }

        cout << endl <<  "Server's UDP response is :" << udpBuffer << endl;
        cout << endl <<  "Server's TCP response is :" << tcpBuffer << endl;
        close(tcpSocket);
        // Close the UDP socket
        close(udpSocket);
    }
}