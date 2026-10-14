
#include <stdio.h>
#include <string.h>
#include <winsock2.h>
#include <ws2tcpip.h>

void sendFile(SOCKET client, const char *filename, const char *type) {
    FILE *file = fopen(filename, "rb");

    if (file == NULL) {
        const char *msg =
            "HTTP/1.1 404 Not Found\r\n"
            "Content-Length: 9\r\n"
            "Connection: close\r\n\r\n"
            "Not Found";

        send(client, msg, (int)strlen(msg), 0);
        return;
    }

    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    rewind(file);

    char header[512];

    int headerLength = snprintf(header, sizeof(header),
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: %s\r\n"
        "Content-Length: %ld\r\n"
        "Connection: close\r\n\r\n", type, size);

    send(client, header, headerLength, 0);

    char buffer[4096];
    size_t bytesRead;

    while ((bytesRead = fread(buffer, 1, sizeof(buffer), file)) > 0) {
        size_t sent = 0;

        while (sent < bytesRead) {
            int result = send(client, buffer + sent,
                              (int)(bytesRead - sent), 0);

            if (result == SOCKET_ERROR || result == 0) {
                fclose(file);
                return;
            }

            sent += result;
        }
    }

    fclose(file);
}

int main() {
    WSADATA wsa;
    SOCKET server, client;
    struct sockaddr_in address;
    char request[2048];

    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        printf("Winsock startup failed\n");
        return 1;
    }

    server = socket(AF_INET, SOCK_STREAM, 0);

    if (server == INVALID_SOCKET) {
        printf("Socket creation failed\n");
        WSACleanup();
        return 1;
    }

    address.sin_family = AF_INET;
    address.sin_port = htons(8080);
    address.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(server, (struct sockaddr *)&address, sizeof(address))
        == SOCKET_ERROR) {
        printf("Bind failed: %d\n", WSAGetLastError());
        closesocket(server);
        WSACleanup();
        return 1;
    }

    if (listen(server, 5) == SOCKET_ERROR) {
        printf("Listen failed\n");
        closesocket(server);
        WSACleanup();
        return 1;
    }

    printf("HostelHub running at http://localhost:8080\n");

    while (1) {
        client = accept(server, NULL, NULL);

        if (client == INVALID_SOCKET) {
            continue;
        }

        int received = recv(client, request, sizeof(request) - 1, 0);

        if (received > 0) {
            request[received] = '\0';

           
if (strncmp(request, "GET /style.css ", 15) == 0) {
    sendFile(client, "public/style.css", "text/css");
} else if (strncmp(request, "GET /mess.html ", 15) == 0) {
    sendFile(client, "public/mess.html", "text/html; charset=UTF-8");
} else if (strncmp(request, "GET / ", 5) == 0 ||
           strncmp(request, "GET /index.html ", 16) == 0) {
    sendFile(client, "public/index.html", "text/html; charset=UTF-8");
} else {

                const char *msg =
                    "HTTP/1.1 404 Not Found\r\n"
                    "Content-Length: 9\r\n"
                    "Connection: close\r\n\r\n"
                    "Not Found";

                send(client, msg, (int)strlen(msg), 0);
            }
        }

        shutdown(client, SD_SEND);
        closesocket(client);
    }

    closesocket(server);
    WSACleanup();

    return 0;
}
