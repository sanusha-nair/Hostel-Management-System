
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
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

void decodeValue(char *value) {
    char *src = value;
    char *dest = value;

    while (*src) {
        if (*src == '+') {
            *dest++ = ' ';
            src++;
        } else if (*src == '%' && src[1] && src[2]) {
            char hex[3] = {src[1], src[2], '\0'};
            *dest++ = (char)strtol(hex, NULL, 16);
            src += 3;
        } else {
            *dest++ = *src++;
        }
    }

    *dest = '\0';
}

void saveFeedback(SOCKET client, const char *body) {
    char student[151] = "";
    char meal[31] = "";
    char rating[10] = "";
    char feedback[901] = "";

    char data[4096];

    snprintf(data, sizeof(data), "%s", body);

    char *field = strtok(data, "&");

    while (field != NULL) {
        char *value = strchr(field, '=');

        if (value != NULL) {
            *value = '\0';
            value++;

            decodeValue(value);

            if (strcmp(field, "student") == 0) {
                snprintf(student, sizeof(student), "%s", value);
            } else if (strcmp(field, "meal") == 0) {
                snprintf(meal, sizeof(meal), "%s", value);
            } else if (strcmp(field, "rating") == 0) {
                snprintf(rating, sizeof(rating), "%s", value);
            } else if (strcmp(field, "feedback") == 0) {
                snprintf(feedback, sizeof(feedback), "%s", value);
            }
        }

        field = strtok(NULL, "&");
    }

    FILE *file = fopen("data/feedback.txt", "a");

    if (file == NULL) {
        const char *msg =
            "HTTP/1.1 500 Internal Server Error\r\n"
            "Content-Length: 19\r\n"
            "Connection: close\r\n\r\n"
            "Could not save data";

        send(client, msg, (int)strlen(msg), 0);
        return;
    }

    fprintf(file,
        "Student: %s | Meal: %s | Rating: %s | Feedback: %s\n",
        student, meal, rating, feedback);

    fclose(file);

    const char *html =
        "<!DOCTYPE html><html><body "
        "style='font-family:Arial;padding:50px'>"
        "<h1>Feedback submitted!</h1>"
        "<p>Your feedback has been saved.</p>"
        "<a href='/mess.html'>Back to Mess Management</a>"
        "</body></html>";

    char response[1024];

    int length = snprintf(response, sizeof(response),
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/html; charset=UTF-8\r\n"
        "Content-Length: %d\r\n"
        "Connection: close\r\n\r\n%s",
        (int)strlen(html), html);

    send(client, response, length, 0);
}

void saveParcel(SOCKET client, const char *body) {
    char student[151] = "";
    char parcelId[91] = "";
    char courier[151] = "";
    char arrivalDate[31] = "";
    char status[31] = "";

    char data[4096];
    snprintf(data, sizeof(data), "%s", body);

    char *field = strtok(data, "&");

    while (field != NULL) {
        char *value = strchr(field, '=');

        if (value != NULL) {
            *value = '\0';
            value++;

            decodeValue(value);

            if (strcmp(field, "student") == 0) {
                snprintf(student, sizeof(student), "%s", value);
            } else if (strcmp(field, "parcelId") == 0) {
                snprintf(parcelId, sizeof(parcelId), "%s", value);
            } else if (strcmp(field, "courier") == 0) {
                snprintf(courier, sizeof(courier), "%s", value);
            } else if (strcmp(field, "arrivalDate") == 0) {
                snprintf(arrivalDate, sizeof(arrivalDate), "%s", value);
            } else if (strcmp(field, "status") == 0) {
                snprintf(status, sizeof(status), "%s", value);
            }
        }

        field = strtok(NULL, "&");
    }

    FILE *file = fopen("data/parcels.txt", "a");

    if (file == NULL) {
        const char *msg =
            "HTTP/1.1 500 Internal Server Error\r\n"
            "Content-Length: 19\r\n"
            "Connection: close\r\n\r\n"
            "Could not save data";

        send(client, msg, (int)strlen(msg), 0);
        return;
    }

    fprintf(file,
        "Student: %s | Parcel ID: %s | Courier: %s | Date: %s | Status: %s\n",
        student, parcelId, courier, arrivalDate, status);

    fclose(file);

    const char *html =
        "<!DOCTYPE html><html><body "
        "style='font-family:Arial;padding:50px'>"
        "<h1>Parcel Saved!</h1>"
        "<p>Parcel details have been saved successfully.</p>"
        "<a href='/parcel.html'>Add Another Parcel</a><br><br>"
        "<a href='/'>Back to Dashboard</a>"
        "</body></html>";

    char response[1024];

    int length = snprintf(response, sizeof(response),
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/html; charset=UTF-8\r\n"
        "Content-Length: %d\r\n"
        "Connection: close\r\n\r\n%s",
        (int)strlen(html), html);

    send(client, response, length, 0);
}

void saveComplaint(SOCKET client, const char *body) {
    char student[151] = "";
    char room[61] = "";
    char category[61] = "";
    char description[901] = "";
    char priority[31] = "";

    char data[4096];
    snprintf(data, sizeof(data), "%s", body);

    char *field = strtok(data, "&");

    while (field != NULL) {
        char *value = strchr(field, '=');

        if (value != NULL) {
            *value = '\0';
            value++;

            decodeValue(value);

            if (strcmp(field, "student") == 0) {
                snprintf(student, sizeof(student), "%s", value);
            } else if (strcmp(field, "room") == 0) {
                snprintf(room, sizeof(room), "%s", value);
            } else if (strcmp(field, "category") == 0) {
                snprintf(category, sizeof(category), "%s", value);
            } else if (strcmp(field, "description") == 0) {
                snprintf(description, sizeof(description), "%s", value);
            } else if (strcmp(field, "priority") == 0) {
                snprintf(priority, sizeof(priority), "%s", value);
            }
        }

        field = strtok(NULL, "&");
    }

    FILE *file = fopen("data/complaints.txt", "a");

    if (file == NULL) {
        const char *msg =
            "HTTP/1.1 500 Internal Server Error\r\n"
            "Content-Length: 19\r\n"
            "Connection: close\r\n\r\n"
            "Could not save data";

        send(client, msg, (int)strlen(msg), 0);
        return;
    }

    fprintf(file,
        "Student: %s | Room: %s | Category: %s | Description: %s | Priority: %s | Status: Pending\n",
        student, room, category, description, priority);

    fclose(file);

    const char *html =
        "<!DOCTYPE html><html><body "
        "style='font-family:Arial;padding:50px'>"
        "<h1>Complaint Submitted!</h1>"
        "<p>Your complaint has been saved successfully.</p>"
        "<a href='/complaints.html'>Submit Another Complaint</a><br><br>"
        "<a href='/'>Back to Dashboard</a>"
        "</body></html>";

    char response[1024];

    int length = snprintf(response, sizeof(response),
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/html; charset=UTF-8\r\n"
        "Content-Length: %d\r\n"
        "Connection: close\r\n\r\n%s",
        (int)strlen(html), html);

    send(client, response, length, 0);
}


int main() {
    WSADATA wsa;
    SOCKET server, client;
    struct sockaddr_in address;

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

        char request[8192];
        int total = 0;
        int received;
        int contentLength = 0;
        int headerSize = 0;

        request[0] = '\0';

        while (total < (int)sizeof(request) - 1) {
            received = recv(client, request + total,
                            (int)sizeof(request) - 1 - total, 0);

            if (received <= 0) {
                break;
            }

            total += received;
            request[total] = '\0';

            char *headerEnd = strstr(request, "\r\n\r\n");

            if (headerEnd != NULL) {
                headerSize = (int)(headerEnd + 4 - request);

                char *lengthHeader = strstr(request, "Content-Length:");

                if (lengthHeader != NULL && lengthHeader < headerEnd) {
                    contentLength = atoi(lengthHeader + 15);
                }

                int bodySize = total - headerSize;

                if (bodySize >= contentLength) {
                    break;
                }
            }
        }

        if (total > 0) {
            request[total] = '\0';

           if (strncmp(request, "POST /submit-feedback ", 22) == 0) {
    char *body = strstr(request, "\r\n\r\n");

    if (body != NULL) {
        body += 4;
        saveFeedback(client, body);
    }
} else if (strncmp(request, "POST /submit-parcel ", 20) == 0) {
    char *body = strstr(request, "\r\n\r\n");

    if (body != NULL) {
        body += 4;
        saveParcel(client, body);
    }
}
else if (strncmp(request, "POST /submit-complaint ", 23) == 0) {
    char *body = strstr(request, "\r\n\r\n");

    if (body != NULL) {
        body += 4;
        saveComplaint(client, body);
    }
}
             else if (strncmp(request, "GET /style.css ", 15) == 0) {
                sendFile(client, "public/style.css", "text/css");
            } else if (strncmp(request, "GET /mess.html ", 15) == 0) {
                sendFile(client, "public/mess.html",
                         "text/html; charset=UTF-8");
            } else if (strncmp(request, "GET /parcel.html ", 17) == 0) {
    sendFile(client, "public/parcel.html",
             "text/html; charset=UTF-8");
    }else if (strncmp(request, "GET /complaint.html ", 20) == 0) {
    sendFile(client, "public/complaint.html",
             "text/html; charset=UTF-8");

            } else if (strncmp(request, "GET / ", 5) == 0 ||
                       strncmp(request, "GET /index.html ", 16) == 0) {
                sendFile(client, "public/index.html",
                         "text/html; charset=UTF-8");
            
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
