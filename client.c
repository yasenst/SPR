#include <netdb.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

#define MAX 255
#define PORT 8080
#define SA struct sockaddr
#define NUM_PRODUCTS 10

int menuIndexes[NUM_PRODUCTS] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
const char * const menuItemNames[NUM_PRODUCTS] =  {"water","juice","cola","fanta","sprite","soda","rakia","wine","whiskey","vodka"};

const char * const songs[5] = {"Metallica-Unforgiven", "Iron Maiden - Fear of the Dark", "Rainbow - Cant let you go", "Whitesnake - Cryin in the rain", "Guns n roses - novermber rain"};


void func(int sockfd)
{
    char buffer[MAX];

    for (;;) {
        bzero(buffer, sizeof(buffer));

        // get random integer to switch requests(from client perspective)
        int requestCode = rand() % 10;

        if (requestCode <= 5)
        {
            // make order containing product and quantity
            int productIndex = rand() % 10;

            while (menuIndexes[productIndex] <= 0) {
                productIndex = rand() % 10;
            }

            int quantityToOrder = rand() % 10 + 1;

            snprintf(buffer, MAX, "%d,%d,%d", 1, productIndex, quantityToOrder);
            printf("Table ordered %d bottles of %s\n",quantityToOrder, menuItemNames[productIndex]);
            write(sockfd, buffer, sizeof(buffer));
            bzero(buffer, sizeof(buffer));
            read(sockfd, buffer, sizeof(buffer));

            // process response from server
            char *token;
            token = strtok (buffer,",");
            int responseCode = atoi(token);
            token = strtok (NULL, ",");

            // if responseCode is 1, it means the product has ran out of stock
            if (responseCode == 1)
            {
                int productIntexToDelete;
                productIntexToDelete = atoi(token);
                token = strtok (NULL, ",");
                printf("%s\n", token);

                menuIndexes[productIntexToDelete] = 0;

            }
            else if (responseCode == 0)
            {
                printf("%s\n", token);
            }
        }
        else if (requestCode > 5 && requestCode <= 7)
        {
            // set request(for server)
            snprintf(buffer, MAX, "%d", 2);
            printf("TAX INSPECTION!\n");
            write(sockfd, buffer, sizeof(buffer));

            bzero(buffer, sizeof(buffer));

            read(sockfd, buffer, sizeof(buffer));
            printf("%s\n", buffer);
        }
        else if (requestCode > 7)
        {
            int songChoice = rand() % 5;
            printf("Table orders song %s\n", songs[songChoice]);
            snprintf(buffer, MAX, "%d,%d",3, songChoice);
            write(sockfd, buffer, sizeof(buffer));

            bzero(buffer, sizeof(buffer));
            read(sockfd, buffer, sizeof(buffer));
            printf("%s\n", buffer);
        }

        bzero(buffer, sizeof(buffer));

        read(sockfd, buffer, sizeof(buffer));
        if (strcmp(buffer, "stop") == 0)
            break;

        sleep(1);
    }
}

int main()
{
    int sockfd, connfd;
    struct sockaddr_in servaddr, cli;

    // socket create and varification
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        printf("socket creation failed...\n");
        exit(0);
    }
    else
        printf("Socket successfully created..\n");
    bzero(&servaddr, sizeof(servaddr));

    // assign IP, PORT
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    servaddr.sin_port = htons(PORT);

    // connect the client socket to server socket
    if (connect(sockfd, (SA*)&servaddr, sizeof(servaddr)) != 0) {
        printf("connection with the server failed...\n");
        exit(0);
    }
    else
        printf("connected to the server..\n");

    // function for chat
    func(sockfd);
    printf("The disco is closing. Come again tomorrow!\n");

    // close the socket
    close(sockfd);
}
