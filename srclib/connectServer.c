/*************************************************************
 * Proyecto: REDES II
 * Fichero: connectServer.c
 * Fecha: 04/02/2016
 * Descripcion: Implementacion de las funciones que inician la conexion al servidor
 * Integrantes: Francisco Andreu Sanz, Javier Martínez Hernández
 *************************************************************/

#include "../includes/connectServer.h"

/**
 * @page do_daemon \b do_daemon
 *
 * @brief Genera un demonio.
 *
 * @section SYNOPSIS
 * 	\b #include \b <connectServer.h>
 *
 *	\b int \b do_daemon \b (\b char\b *\b identificador\b )
 * 
 * @section descripcion DESCRIPCIÓN
 *
 * Genera un demonio para correr la aplicación en segundo plano.
 * 
 * Recibe como parámetro un identificador que es el nombre del servidor.
 *
 * @section retorno RETORNO
 * 
 * 0: si todo es correcto.
 * -1: en caso de error.
 * 
 * @section authors AUTOR
 * Francisco Andreu Sanz (francisco.andreu@estudiante.uam.es)
 * Javier Martínez Hernández (javier.maritnez@estudiante.uam.es)
*/
int do_daemon(char *identificador) {
    pid_t pid;
    pid = fork();
    int i;

    if (pid < 0) exit(EXIT_FAILURE);
    if (pid > 0) exit(EXIT_SUCCESS);

    umask(0);
    setlogmask(LOG_UPTO(LOG_INFO));
    openlog(identificador, LOG_CONS | LOG_PID | LOG_NDELAY, LOG_DAEMON);
    syslog(LOG_ERR, "Initiating new server.");

    if (setsid() < 0) {
        syslog(LOG_ERR, "Error creating a new SID for the child process.");
        exit(EXIT_FAILURE);
    }

    if ((chdir("/")) < 0) {
        syslog(LOG_ERR, "Error changing the current working directory = \"/\"");
        exit(EXIT_FAILURE);
    }

    syslog(LOG_ERR, "Closing standard file descriptors");

    syslog(LOG_INFO, "Closing all file descriptors");
    for (i = 0; i < getdtablesize(); i++) {
        close(i);
    }

    return 0;
}

/**
 * @page initiate_server \b initiate_server
 *
 * @brief Inicia el servidor.
 *
 * @section SYNOPSIS
 * 	\b #include \b <connectServer.h>
 *
 *	\b int \b initiate_server \b (\b char\b *\b nombre\b )
 * 
 * @section descripcion DESCRIPCIÓN
 *
 * Inicia el servidor abriendo socket, bind y listen.
 * 
 * Recibe como parámetro el nombre del servidor.
 *
 * @section retorno RETORNO
 * 
 * int: este será el valor del socket.
 * 
 * @section authors AUTOR
 * Francisco Andreu Sanz (francisco.andreu@estudiante.uam.es)
 * Javier Martínez Hernández (javier.maritnez@estudiante.uam.es)
*/
int initiate_server(char *nombre) {
    int sockval;
    SERVER_NOMBRE = nombre;

    do_daemon(nombre);


    if ((sockval = abrirSocket()) < 0)
        exit(EXIT_FAILURE);

    if (abrirBind(sockval, PORT) < 0)
        exit(EXIT_FAILURE);

    if (abrirListen(sockval, MAX_CONNECTIONS) < 0)
        exit(EXIT_FAILURE);
    return sockval;
}

/**
 * @page abrirSocket \b abrirSocket
 *
 * @brief Crea el socket.
 *
 * @section SYNOPSIS
 * 	\b #include \b <connectServer.h>
 *
 *	\b int \b abrirSocket \b (\b )
 * 
 * @section descripcion DESCRIPCIÓN
 *
 * Crea el socket
 * 
 * No recibe ningún parámetro.
 *
 * @section retorno RETORNO
 * 
 * sockval:  valor del socket.
 * -1: en caso de error.
 * 
 * @section authors AUTOR
 * Francisco Andreu Sanz (francisco.andreu@estudiante.uam.es)
 * Javier Martínez Hernández (javier.maritnez@estudiante.uam.es)
*/
int abrirSocket() {
    syslog(LOG_INFO, "Creating_socket");
    int sockval = socket(AF_INET, SOCK_STREAM, 0);
    if (sockval < 0) {
        syslog(LOG_ERR, "Error_creando_socket");
        return -1;
    }
    return sockval;
}

/**
 * @page abrirBind \b abrirBind
 *
 * @brief Abre el bind.
 *
 * @section SYNOPSIS
 * 	\b #include \b <connectServer.h>
 *
 *	\b int \b abrirBind \b (\b char\b *\b sockval\b, \b int\b puerto\b )
 * 
 * @section descripcion DESCRIPCIÓN
 *
 * Abrimos el bind para la conexión.
 * 
 * Recibe como parámetros el valor del socket y el puerto.
 *
 * @section retorno RETORNO
 * 
 * 0: ok.
 * -1: error.
 * 
 * @section authors AUTOR
 * Francisco Andreu Sanz (francisco.andreu@estudiante.uam.es)
 * Javier Martínez Hernández (javier.maritnez@estudiante.uam.es)
*/
int abrirBind(int sockval, int puerto) {
    syslog(LOG_INFO, "Binding_socket");
    struct sockaddr_in Direccion;
    Direccion.sin_family = AF_INET;
    Direccion.sin_port = htons(puerto);
    Direccion.sin_addr.s_addr = INADDR_ANY;
    bzero((void *) &(Direccion.sin_zero), 8);
    if (bind(sockval, (struct sockaddr*) &Direccion, sizeof (Direccion)) == -1) {
        syslog(LOG_ERR, "Error_binding");
        return -1;
    }
    return 0;
}

/**
 * @page abrirListen \b abrirListen
 *
 * @brief Abre listen.
 *
 * @section SYNOPSIS
 * 	\b #include \b <connectServer.h>
 *
 *	\b void \b abrirListen \b (\b char\b *\b sockval\b,\b int\b tam\b )
 * 
 * @section descripcion DESCRIPCIÓN
 *
 * Abrimos la conexión de escucha.
 * 
 * Recibe como parámetros el valor del socket y el tamaño máximo de escucha.
 *
 * @section retorno RETORNO
 * 
 * 0: ok.
 * -1: error.
 * 
 * @section authors AUTOR
 * Francisco Andreu Sanz (francisco.andreu@estudiante.uam.es)
 * Javier Martínez Hernández (javier.maritnez@estudiante.uam.es)
*/
int abrirListen(int sockval, int tam) {
    syslog(LOG_INFO, "Listening_connections");
    int lstn = listen(sockval, tam);
    if (lstn == -1) {
        syslog(LOG_ERR, "Error_listening");
        return -1;
    }
    return 0;
}

/**
 * @page ejecutarAccion \b ejecutarAccion
 *
 * @brief Parsea los distintos comandos IRC.
 *
 * @section SYNOPSIS
 * 	\b #include \b <connectServer.h>
 *
 *	\b void \b ejecutarAccion \b (\b long\b option\b, \b char\b *\b command\b, \b char\b **\b nick\b, const\b int\b sd\b )
 * 
 * @section descripcion DESCRIPCIÓN
 *
 * Parsea los distintos comando IRC desarrollados para así ejecutar su acción correspondiente.
 * 
 * Recibe como parámetros la opción IRC del comando desconocido, la cadena del comando, el nick del usuario que envia 
 * el mensaje y su socket correspondiente.
 *
 * @section retorno RETORNO
 * 
 * 0: ok.
 * 
 * @section authors AUTOR
 * Francisco Andreu Sanz (francisco.andreu@estudiante.uam.es)
 * Javier Martínez Hernández (javier.maritnez@estudiante.uam.es)
*/
long ejecutarAccion(long option, char *command, char** nick, int sd) {

    switch (option) {

        case JOIN:
        {
            syslog(LOG_ERR, "Parseo de JOIN.");
            char *prefix = NULL;
            char *channel = NULL;
            char *key = NULL;
            char *msg;

            IRCParse_Join(command, &prefix, &channel, &key, &msg);

            join_case(prefix, channel, key, msg, nick, sd, command);
            syslog(LOG_ERR, "Salimos de JOIN.");
            break;
        }


        case NICK:
        {
            syslog(LOG_ERR, "Parseo de NICK.");
            ;
            char *newnick = NULL;
            char *prefix = NULL;
            char* msg = NULL;

            IRCParse_Nick(command, &prefix, &newnick, &msg);
            nick_case(prefix, newnick, msg, nick, sd);
            syslog(LOG_ERR, "Salimos de NICK.");
            break;
        }

        case USER:
        {
            syslog(LOG_ERR, "Parseo de USER.");
            char *prefix;
            char* username = NULL;
            char* realname = NULL;
            char* servername = NULL;
            char* hostname = NULL;



            IRCParse_User(command, &prefix, &username, &hostname, &servername, &realname);

            user_case(prefix, username, hostname, servername, realname, nick, sd);
            syslog(LOG_ERR, "Salimos de USER.");
            break;
        }
        case LIST:
        {
            syslog(LOG_ERR, "Parseo de LIST.");
            char *prefix;
            char *channel;
            char *target;

            IRCParse_List(command, &prefix, &channel, &target);

            list_case(prefix, channel, target, nick, sd);
            syslog(LOG_ERR, "Salimos de LIST.");
            break;
        }
        case WHOIS:
        {
            syslog(LOG_ERR, "Parseo de WHOIS.");
            char* target;
            char* prefix;
            char* apodo;

            IRCParse_Whois(command, &prefix, &target, &apodo);

            whois_case(prefix, target, apodo, nick, sd);
            syslog(LOG_ERR, "Salimos de WHOIS.");
            break;
        }
        case NAMES:
        {
            syslog(LOG_ERR, "Parseo de NAMES.");
            char* target;

            char* prefix;
            char* channel;

            IRCParse_Names(command, &prefix, &channel, &target);

            names_case(prefix, channel, target, nick, sd);
            syslog(LOG_ERR, "Salimos de NAMES.");
            break;
        }

        case WHO:
        {
            syslog(LOG_ERR, "Parseo de WHO.");

            char * mask;
            char * prefix;
            char * oppar;

            IRCParse_Who(command, &prefix, &mask, &oppar);

            who_case(prefix, mask, oppar, nick, sd);
            syslog(LOG_ERR, "Salimos de WHO.");
            break;
        }
        case PRIVMSG:
        {
            syslog(LOG_ERR, "Parseo de PRIVMSG.");

            char* prefix;
            char *msgtarget;
            char *msg;

            IRCParse_Privmsg(command, &prefix, &msgtarget, &msg);

            privmsg_case(prefix, msgtarget, msg, nick, sd);
            syslog(LOG_ERR, "Salimos de PRIVMSG.");
            break;
        }

        case PING:
        {
            syslog(LOG_ERR, "Parseo de PING.");
            char* prefix;
            char* server1;
            char* server2;
            char* comando;
            char* msg;

            IRCParse_Ping(command, &prefix, &server1, &server2, &msg);

            ping_case(prefix, server1, server2, msg, nick, sd);
            syslog(LOG_ERR, "Salimos de PING.");
            break;
        }
        case PART:
        {
            syslog(LOG_ERR, "Parseo de PART.");
            char* prefix;
            char* channel;
            char* msg;

            IRCParse_Part(command, &prefix, &channel, &msg);

            part_case(prefix, channel, msg, nick, sd);
            syslog(LOG_ERR, "Salimos de PART.");
            break;
        }
        case TOPIC:
        {
            syslog(LOG_ERR, "Parseo de TOPIC.");
            char* prefix = NULL;
            char* channel = NULL;
            char* topic = NULL;

            IRCParse_Topic(command, &prefix, &channel, &topic);

            topic_case(prefix, channel, topic, nick, sd);
            syslog(LOG_ERR, "Salimos de TOPIC.");
            break;
        }
        case KICK:
        {
            syslog(LOG_ERR, "Parseo de KICK.");
            char* prefix;
            char* channel;
            char* user;
            char* comment;

            IRCParse_Kick(command, &prefix, &channel, &user, &comment);
            kick_case(prefix, channel, user, comment, nick, sd);
            syslog(LOG_ERR, "Salimos de KICK.");
            break;
        }
        case AWAY:
        {
            syslog(LOG_ERR, "Parseo de AWAY.");
            char* prefix;
            char* msg;

            IRCParse_Away(command, &prefix, &msg);
            away_case(prefix, msg, nick, sd);
            syslog(LOG_ERR, "Salimos de AWAY.");

            break;
        }

        case QUIT:
        {
            syslog(LOG_ERR, "Parseo de QUIT.");
            char* prefix;
            char* msg;
            int i;

            IRCParse_Quit(command, &prefix, &msg);
            quit_case(prefix, msg, nick, sd);
            syslog(LOG_ERR, "Salimos de QUIT.");
            break;
        }

        case MODE:
        {
            syslog(LOG_ERR, "Parseo de MODE.");
            char* prefix;
            char* channel;
            char* mode;
            char* user;

            IRCParse_Mode(command, &prefix, &channel, &mode, &user);

            mode_case(prefix, channel, mode, user, nick, sd);
            syslog(LOG_ERR, "Salimos de MODE.");
            break;
        }

        case MOTD:
        {
            syslog(LOG_ERR, "Parseo de MOTD.");
            char* prefix;
            char* target;

            IRCParse_Motd(command, &prefix, &target);

            motd_case(prefix, target, nick, sd);
            syslog(LOG_ERR, "Salimos de MOTD.");
            break;
        }

        case IRCERR_UNKNOWNCOMMAND:
        {
            syslog(LOG_ERR, "Parseo de UNKNOWNCOMMAND.");
            unknown_case(command, nick, sd);
            syslog(LOG_ERR, "Salimos de UNKNOWNCOMMAND.");
            break;
        }
    }
    return 0;
}

