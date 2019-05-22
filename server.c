#include <stdio.h>
#include <netdb.h>
#include <unistd.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <time.h>

#define MAX 255
#define PORT 8080
#define SA struct sockaddr
#define NUM_PRODUCTS 10

typedef struct Products {
    char name[50];
    int price;
    int quantity;
} Prod;

const char * const songs[5] = {"Metallica-Unforgiven", "Iron Maiden - Fear of the Dark", "Rainbow - Cant let you go", "Whitesnake - Cryin in the rain", "Guns n roses - novermber rain"};

Prod productsArray[NUM_PRODUCTS];
int availableProducts = 10;
int oborot = 0;

void initData()
{
    strcpy(productsArray[0].name, "water");
    productsArray[0].price = 1;
    productsArray[0].quantity = 20;

    strcpy(productsArray[1].name, "juice");
    productsArray[1].price = 2;
    productsArray[1].quantity = 20;

    strcpy(productsArray[2].name, "cola");
    productsArray[2].price = 3;
    productsArray[2].quantity = 20;

    strcpy(productsArray[3].name, "fanta");
    productsArray[3].price = 3;
    productsArray[3].quantity = 20;

    strcpy(productsArray[4].name, "sprite");
    productsArray[4].price = 3;
    productsArray[4].quantity = 20;

    strcpy(productsArray[5].name, "soda");
    productsArray[5].price = 2;
    productsArray[5].quantity = 20;

    strcpy(productsArray[6].name, "rakia");
    productsArray[6].price = 5;
    productsArray[6].quantity = 20;

    strcpy(productsArray[7].name, "wine");
    productsArray[7].price = 4;
    productsArray[7].quantity = 20;

    strcpy(productsArray[8].name, "whiskey");
    productsArray[8].price = 5;
    productsArray[8].quantity = 20;

    strcpy(productsArray[9].name, "vodka");
    productsArray[9].price = 5;
    productsArray[9].quantity = 20;

}

void func(int sockfd)
{
    initData();
    char buffer[MAX];

    for (;;) {
        bzero(buffer, MAX);

        read(sockfd, buffer, sizeof(buffer));

        // get request code
        char *token;
        token = strtok (buffer,",");
        int requestCode = atoi(token);
        token = strtok (NULL, ",");

        // process request
        switch (requestCode)
        {
        case 1:
            {
            // parse remaining part
            int productIndex;
            int quantityOrdered;

            productIndex = atoi(token);
            token = strtok (NULL, ",");
            quantityOrdered = atoi(token);
            bzero(buffer, MAX);

            // check quantity
            if ((productsArray[productIndex].quantity - quantityOrdered) <= 0)
            {
                int amountToServe = productsArray[productIndex].quantity;
                // construct response for when running out of products
                char failResponse[MAX];
                sprintf(failResponse, "Waiter served only %d bottles of %s for %d BGN. Out of %s.\n",
                        productsArray[productIndex].quantity,
                        productsArray[productIndex].name,
                        productsArray[productIndex].price*amountToServe,
                        productsArray[productIndex].name);

                snprintf(buffer, MAX, "%d,%d,%s", 1, productIndex, failResponse);

                // do calculations
                productsArray[productIndex].quantity = 0;
                oborot += productsArray[productIndex].price*productsArray[productIndex].quantity;
                availableProducts--;

                write(sockfd, buffer, sizeof(buffer));
            }
            else
            {
                // construct successful response(when enough quantity left)
                char successResponse[MAX];
                sprintf(successResponse, "Waiter served %d bottles of %s for %d BGN.\n",
                        quantityOrdered,
                        productsArray[productIndex].name,
                        productsArray[productIndex].price*quantityOrdered);
                snprintf(buffer, MAX, "%d,%s", 0, successResponse);

                // do calculations
                productsArray[productIndex].quantity -= quantityOrdered;
                oborot += productsArray[productIndex].price*quantityOrdered;

                write(sockfd, buffer, sizeof(buffer));
            }
        }
            break;
        case 2:
            {
                // in case of a tax inspection, return date and oborot
                char taxInfo[MAX];
                time_t t;   // not a primitive datatype
                time(&t);
                snprintf(buffer, MAX, "Date: %s Current turnover is %d BGN\n",
                                        ctime(&t),
                                        oborot);
                write(sockfd, buffer, sizeof(buffer));
            }
            break;
        case 3:
            {
                // process song request
                int songIndex;
                songIndex = atoi(token);

                int songAvailable = rand() % 5;
                int tip = (rand() % 5) + 1;
                if (songAvailable < 3)
                {
                    snprintf(buffer, MAX, "Disco is playing %s for %d\n", songs[songIndex], tip);
                    oborot += tip;
                }
                else
                {
                    sprintf(buffer, "Sorry, but there are gangsters(mutri) around and they want to listen to Mile Kitic so we wont play your song.\n");
                }

                write(sockfd, buffer, sizeof(buffer));

            }
            break;
        }

        // check for termination(out of products)
        bzero(buffer, MAX);
        if (availableProducts == 0)
        {
            sprintf(buffer, "stop");
            write(sockfd, buffer, sizeof(buffer));
            break;
        }
        else
        {
            sprintf(buffer, "continue");
            write(sockfd, buffer, sizeof(buffer));
        }
    }
}

int main()
{
    int sockfd, connfd, len;
    struct sockaddr_in servaddr, cli;

    // socket create and verification
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
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(PORT);

    // Binding newly created socket to given IP and verification
    if ((bind(sockfd, (SA*)&servaddr, sizeof(servaddr))) != 0) {
        printf("socket bind failed...\n");
        exit(0);
    }
    else
        printf("Socket successfully binded..\n");

    // Now server is ready to listen and verification
    if ((listen(sockfd, 5)) != 0) {
        printf("Listen failed...\n");
        exit(0);
    }
    else
        printf("Server listening..\n");
    len = sizeof(cli);

    // Accept the data packet from client and verification
    connfd = accept(sockfd, (SA*)&cli, &len);
    if (connfd < 0) {
        printf("server acccept failed...\n");
        exit(0);
    }
    else
        printf("server acccept the client...\n");

    // Function for chatting between client and server
    func(connfd);
    printf("The disco is closing. We've made %d BGN today.", oborot);
    // After chatting close the socket
    close(sockfd);
}


