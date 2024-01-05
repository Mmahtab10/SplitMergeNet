# Socket-programming

### Overview

#### Simple Client-Server Socket Programming

This program demonstrates socket programming in a client-server architecture. It offers the functionality to split or merge text based on vowels and consonants. The program leverages UDP (User Datagram Protocol) for transmitting vowels and TCP (Transmission Control Protocol) for transmitting consonants or merged text. Testing was conducted on a personal MacOS computer and the CPSC servers, ensuring all functionalities perform as expected. Any non-functioning features are noted in the Assumptions section.

### Features

1. **Split Text** (Basic Encoding)
2. **Merge Text** (Basic Encoding)
3. **Split Text** (Advanced Encoding)
4. **Merge Text** (Advanced Encoding)

### Usage

#### Server Compilation
- Compile the server program using the command below, specifying UDP and TCP port numbers:
  ```
  g++ udpServer.cpp -o server
  ./server <UDP port> <TCP port>
  ```

#### Client Compilation
- Compile the client program using the command below, specifying UDP and TCP port numbers and the localhost IP address:
  ```
  g++ udpClient.cpp -o client
  ./client <UDP port> <TCP port> <IP address>
  ```
  Ensure port numbers match on both the client and server sides.

#### Menu Options
- After launching, choose an operation (1 to 5) from the console menu.

#### Text Operations
- For **Split Text** (Options 1 or 3), enter a single-line sentence to split.
- For **Merge Text** (Options 2 or 4), enter consonants and vowels as prompted.
  - Basic Encoding Example:
    ```
    Consonants: H ll  th r
    Vowels: e  o   e e
    ```
  - Advanced Encoding Example:
    ```
    Consonants: Hll thr
    Vowels: 1e2o3e1e
    ```

#### Results and Termination
- Results are displayed on the console with messages indicating the server's UDP and TCP responses.
- To exit, choose option 5 on the client and use Ctrl (or Cmd) + C on the server.

### File Structure

- `udpServer.cpp`: Server-side source code.
- `udpClient.cpp`: Client-side source code.

### Dependencies

The program uses standard C++ libraries for socket programming and string manipulation.

### Assumptions

1. Client-side input error checks are implemented. In practice, server-side checks should also be present.
2. User input is assumed to be single-line, without newline characters.
3. Menu input should not include spaces. For example, use "5" instead of " 5" or "5  ".
4. Port number and IP address must be specified as command-line arguments.
5. The user input should not include the '~' character.
6. In case of UDP transmission failure, the program times out after 5 seconds, displaying an error message and returning to the main menu.
7. Test messages are sent via UDP to prevent crashes during split operations, as UDP is not used for data transmission to the server in these cases.
8. For basic encoding merge operations, the lengths of consonants and vowels inputs must match.
9. A limited buffer size of 1024 is used based on the instructor's indication of a small input size for testing.

---

*Created by Mahtab Khan*
