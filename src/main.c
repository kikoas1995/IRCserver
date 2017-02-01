/*************************************************************
 * Proyecto: REDES II
 * Fichero: main.c
 * Fecha: 04/02/2016
 * Descripcion: Implementacion de la funcion principal que controla el inicio del servidor
 * Integrantes: Francisco Andreu Sanz, Javier Martínez Hernández
 *************************************************************/


#include "../includes/connectServer.h"
#include "../includes/cabeceras.h"
#include <errno.h>

pthread_mutex_t mutex[MAX_CLIENTS];
pthread_mutex_t mutex2[MAX_CLIENTS];

//int client_socket[MAX_CLIENTS];
//char* nickList[MAX_CLIENTS];

void* thr_func(void* thr_info) {

    char buffer[1025]; //data buffer of 1K
    int i, j;
    int sd;
    int valread;
    int addrlen;
    struct sockaddr_in address;
    char* next;
    char* resultado;

    thread_info* info = (thread_info*) thr_info;
    i = info->client_id;
    sd = info->sd;
    addrlen = info->addrlen;
    address = info->address;
    free(info);
    info = NULL;

    pthread_mutex_lock(&mutex[i]);
    valread = read(sd, buffer, 8192);

    //Check if it was for closing , and also read the incoming message
    if (valread == 0) {
        pthread_mutex_lock(&mutex2[i]);
        for (j = 0; j < MAX_CLIENTS; j++)
            if (client_socket[j] == sd)
                if (nickList[j]) {
                    printf("\nBORRO A %d - %s\n", sd, nickList[j]);
                    IRCTAD_Quit(IRCTADUser_GetUserByNick(nickList[j]));
                    nickList[j] = NULL;
                    break;
                }


        //Somebody disconnected , get his details and print
        getpeername(sd, (struct sockaddr*) &address, (socklen_t*) & addrlen);
        printf("Host disconnected , ip %s , port %d, socket %i \n", inet_ntoa(address.sin_addr), ntohs(address.sin_port), sd);



        //Close the socket and mark as 0 in list for reuse
        closed[i]=1;
        //nickList[i]=NULL;
        pthread_mutex_unlock(&mutex2[i]);


    } else if (valread > 0){
        /*Anyadimos fin de cadena para que lo que reciba la siguiente funcion no
         contenga caracteres 
         * basura*/
        buffer[valread] = '\0';
        next = IRC_UnPipelineCommands(buffer, &resultado, NULL);;
        ejecutarAccion(IRC_CommandQuery(resultado), resultado, &nickList[i], sd);
        syslog(LOG_ERR, "Ejecutar1.");
	free(resultado);
	while (next != NULL) {
	    next = IRC_UnPipelineCommands(NULL, &resultado, next);
	    ejecutarAccion(IRC_CommandQuery(resultado), resultado, &nickList[i], sd);
            syslog(LOG_ERR, "Ejecutar2.");
	    free(resultado);
	}
    }
pthread_mutex_unlock(&mutex[i]);
    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    pthread_t thread1;
    int master_socket, addrlen, new_socket, max_clients = MAX_CLIENTS, activity, i, sd;
    int max_sd;
    struct sockaddr_in address;
    thread_info* info = NULL;
    char server[] = "Pareja02";

    //set of socket descriptors
    fd_set readfds;

    //a message
    char *message = "ECHO Daemon v1.0 \r\n";

    if (argc != 1){
        printf("Numero de argumentos invalido. Solo se puede ejecutar con un param.\n");
        printf("Ejecutar como: ./<nombre ejecutable>.\n");
        return -1;
    }
        
    //initialise all client_socket[] to 0 so not checked
    for (i = 0; i < max_clients; i++) {
        client_socket[i] = 0;
        pthread_mutex_init(&mutex[i], NULL);
        pthread_mutex_init(&mutex2[i], NULL);
        nickList[i] = NULL;
    }


    //create a master socket
    master_socket = initiate_server(server); //iniciamos server, return socket

    //accept the incoming connection
    addrlen = sizeof (address);
    puts("Waiting for connections ...");

    while (TRUE) {
        FD_ZERO(&readfds);

        //add master socket to set
        FD_SET(master_socket, &readfds);
        max_sd = master_socket; //nº se socket

        for(i = 0; i < MAX_CLIENTS; i++) 
            if(closed[i]==1){
                close(client_socket[i]);
                client_socket[i]=0;
                closed[i]=0;
            }
                
        //add child sockets to set
        for (i = 0; i < max_clients; i++) {
            //socket descriptor
            sd = client_socket[i];
            //if valid socket descriptor then add to read list
            if (sd > 0)
                FD_SET(sd, &readfds);
            
            //highest file descriptor number, need it for the select function
            if (sd > max_sd)
                max_sd = sd;
        }

        //wait for an activity on one of the sockets , timeout is NULL , so wait indefinitely
        activity = select(max_sd + 1, &readfds, NULL, NULL, NULL); //1.-rango para probar. 2.-Archivo listo para leer. 3.- Archivo listo para escribir. 4.-
        if ((activity < 0) && (errno != EINTR)) {
            syslog(LOG_ERR, "SELECT ERROR.");
        }

        //If something happened on the master socket , then its an incoming connection
        if (FD_ISSET(master_socket, &readfds)) {
            if ((new_socket = accept(master_socket, (struct sockaddr *) &address, (socklen_t*) & addrlen)) < 0)//valor del socket
            {
                perror("accept");
                exit(EXIT_FAILURE);
            }

            //inform user of socket number - used in send and receive commands
            printf("New connection , socket fd is %d , ip is : %s , port : %d \n", new_socket, inet_ntoa(address.sin_addr), ntohs(address.sin_port));

            //send new connection greeting message
            if (send(new_socket, message, strlen(message), 0) != strlen(message)) {
                perror("send");
            }

            puts("Welcome message sent successfully");

            //add new socket to array of sockets
            for (i = 0; i < max_clients; i++) {
                //if position is empty
                if (client_socket[i] == 0) {
                    client_socket[i] = new_socket; //añadimos a la lista de socket. Por cada cliente conectado.
                    printf("Adding to list of sockets as %d\n", i);
                    printf("Socket en pos %d: %d\n", i, client_socket[i]);
                    break;
                }
            }
        }

        //else its some IO operation on some other socket :)
        for (i = 0; i < max_clients; i++) {
            sd = client_socket[i];
            //printf("valor de client_socket: %i\n",client_socket[i]);           
            if (FD_ISSET(sd, &readfds)) {
                info = (thread_info*) malloc(sizeof (thread_info));

                info->client_id = i;
                info->sd = sd;
                info->address = address;
                info->addrlen = addrlen;
                pthread_create(&thread1, NULL, thr_func, (void *) info);
              
            }
        }
    }

    return 0;
} 
