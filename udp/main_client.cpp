#include <array>
#include <iostream>

#include <WinSock2.h>
#include <ws2ipdef.h>
#include <WS2tcpip.h>

#ifdef __unix__
using SOCKET = int;
#endif

int main() {
#ifdef WINDOWS_PLATFORM
    //On Windows only you need to initialize the Socket Library in version 2.2
    WSAData wsa;
    WSAStartup(MAKEWORD(2,2), &wsa);
#endif

    //socket function takes 3 params
    // AF : Address Family IPv4/IPv6 => AF_INET | AF_INET6
    //Type : STREAM => TCP | DGRAM => UDP
    //Protocol : IPPROTO_UDP = UDP | IPPROTO_TCP = TCP  || 0 = auto
    SOCKET s = socket(AF_INET, SOCK_DGRAM, 0);

    //Since we are a client we just send data to someone
    //We need a buffer to receive data into
    const char* data_to_send = "Hello World!!";
    size_t data_length = strlen(data_to_send);

    //Server is listening on 127.0.0.1:45000
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(45'000);
    serverAddr.sin_addr.S_un.S_addr = INADDR_LOOPBACK;

    //We send back the message to the client
    int byteSent = sendto(s,
                          data_to_send,
                          data_length,
                          0,
                          reinterpret_cast<sockaddr*>(&serverAddr),
                          sizeof(serverAddr));

    if(byteSent < 0)
    {
        std::cout << "We failed to SEND TO" << std::endl;
        closesocket(s);
        return EXIT_FAILURE;
    }

    //Now we receive the serve response
    std::array<char, 65535> buffer;
    sockaddr_in peerAddr;
    int peerAddrLength = sizeof peerAddr;
    //memset = Memory Set, we are setting all the memory of clientAddr at 0
    memset(&peerAddr, 0, sizeof peerAddr);

    //recvfrom receive data from a client and returns the size of the data received
    int recvLength = recvfrom(s,
                              buffer.data(),
                              65535,
                              0,
                              reinterpret_cast<sockaddr*>(&peerAddr),
                              &peerAddrLength);

    if(recvLength < 0)
    {
        int error = WSAGetLastError();
        std::cout << "We failed to RECV FROM" << std::endl;
        closesocket(s);
        return EXIT_FAILURE;
    }

    //We need to convert Client Addr to string for print
    std::array<char, 256> ip;
    //inet_ntop returns the string representation of clientAddr
    const char* ipClient = inet_ntop(AF_INET, &peerAddr.sin_addr, ip.data(), 256);
    //We print everything
    std::cout << "We received : " <<
              recvLength << " bytes from : "
              << ipClient << ":" << ntohs(peerAddr.sin_port) << std::endl;
    std::string msg(buffer.data(), recvLength);
    std::cout << "Message from server : {}" << msg << std::endl;

    //We finished our job so close everything
    closesocket(s);

#ifdef WINDOWS_PLATFORM
    WSACleanup();
#endif
}
