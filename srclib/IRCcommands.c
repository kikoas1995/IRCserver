/*************************************************************
 * Proyecto: REDES II
 * Fichero: IRCcommands.c
 * Fecha: 04/02/2016
 * Descripcion: Implementacion de las funciones que inician la conexion al servidor
 * Integrantes: Francisco Andreu Sanz, Javier Martínez Hernández
 *************************************************************/

#include "../includes/IRCcommands.h"

/**
 * @page join_case \b join_case
 *
 * @brief Añade un cliente a un canal.
 *
 * @section SYNOPSIS
 * 	\b #include \b <IRCcommands.h>
 *
 *	\b void \b join_case \b (\b char\b *\b prefix\b, \b char\b *\b newnick\b, \b char\b *\b msg\b, \b char\b **\b nick\b, const\b int\b sd\b )
 * 
 * @section descripcion DESCRIPCIÓN
 *
 * 
 * Añade un cliente a un canal. Si este no existe lo crea. Se puede incluir contraseña para la creación, esta será necesaria
 * para el acceso a este canal.
 * 
 * Recibe como parámetros el prefijo que nos da la funcion de parseo del comando, canal que queremos crear o unirnos, contraseña para
 * el canal, un posible mensaje, el nick del usuario que envia el mensaje, su socket correspondiente y la cadena del comando que recibimos.
 *
 * @section retorno RETORNO
 * 
 * No devuelve nada.
 * 
 * @section authors AUTOR
 * Francisco Andreu Sanz (francisco.andreu@estudiante.uam.es)
 * Javier Martínez Hernández (javier.maritnez@estudiante.uam.es)
*/
void join_case(char *prefix, char* channel, char* key, char* msg, char** nick, const int sd, char* command) {

    char *pass = NULL;
    char* user = IRCTADUser_GetUserByNick(*nick);
    char* host = IRCTADUser_GetHostByNick(*nick);
    char** listusers;
    char listnick[1024] = "";
    long num = NULL;
    char *comando1, *comando2, *comando3;
    char** chanlist;
    long numchan;
    int i, j, flag = 0;
    char *comando = NULL;

    if (!channel) {
        IRC_Prefix(&prefix, *nick, user, host, IRCTADUser_GetIPByNick(*nick));
        command[strlen(command) - 2] = 0;
        IRCMsg_ErrNeedMoreParams(&comando, prefix + 1, *nick, command);
        send(sd, comando, strlen(comando), 0);
        free(comando);
        comando = NULL;
        free(prefix);

        if (key)
            free(key);
        if (msg)
            free(msg);
    } else {
        IRCTADChan_GetList(&chanlist, &numchan, NULL);
        for (i = 0; i < numchan; i++)
            if (strcmp(chanlist[i], channel) == 0) {
                flag++;
                break;
            }
        if (IRCTADChan_GetMode(channel) == IRCMODE_CHANNELPASSWORD) {
            IRC_Prefix(&prefix, *nick, user, host, IRCTADUser_GetIPByNick(*nick));
            if (IRCTADChan_TestPassword(channel, key) != IRC_OK){
                IRCMsg_ErrBadChannelKey(&comando, prefix + 1, *nick, channel);
                send(sd, comando, strlen(comando), 0);
            } else {
                if (flag == 0){
                    IRCTAD_JoinChannel(channel, user, "o", key);
                }else{
		          IRCTAD_JoinChannel(channel, user, "", key);
		          }
            IRCMsg_Join(&comando, prefix + 1, NULL, NULL, channel);
            IRCTAD_ListUsersOnChannel(channel, &listusers, &num);
            for (i=0; i<num;i++)
                for(j=0; j<MAX_CLIENTS; j++)
                    if(nickList[j])
                        if (strcmp(nickList[j], IRCTADUser_GetNickByUser(listusers[i]))==0){
                            send(client_socket[j], comando, strlen(comando), 0);
                            break;
                        }
            IRCTAD_FreeListUsersOnChannel (listusers, num);

            }
            free(comando);
            comando = NULL;
            free(prefix);
            
            if (key)
                free(key);
            if (msg)
                free(msg);

        } else {
            IRC_Prefix(&prefix, *nick, user, host, IRCTADUser_GetIPByNick(*nick));
            if (flag == 0)
                IRCTAD_JoinChannel(channel, user, "o", key);
            else
                IRCTAD_JoinChannel(channel, user, "", key);
            IRCTAD_ListUsersOnChannel(channel, &listusers, &num);
            IRCMsg_Join(&comando, prefix + 1, NULL, pass, channel);
	    for (i=0; i<num;i++)
	        for(j=0; j<MAX_CLIENTS; j++)
	            if(nickList[j])
	                if (strcmp(nickList[j], listusers[i])==0){
	                    send(client_socket[j], comando, strlen(comando), 0);
	                    break;
	                }
    
	    for (i = 0; i < num; i++) {
	        if (i >= 1) {
	            strcat(listnick, " ");
	        }
	        if (strcmp(IRCTADChan_GetCreator(channel), listusers[i]) == 0) {
	            strcat(listnick, "@");
	        }
	        strcat(listnick, IRCTADUser_GetNickByUser(listusers[i]));
	    }
	    IRCMsg_RplNamReply(&comando1, SERVER_NOMBRE, *nick, "=", channel, listnick);

	    IRCMsg_RplEndOfNames(&comando2, SERVER_NOMBRE, *nick, channel);
            if (IRCTADChan_GetTopic(channel, NULL) == NULL)
                IRCTADChan_SetTopic(channel, "");

            IRC_PipelineCommands(&comando3, comando, comando1, comando2, NULL);
            send(sd, comando3, strlen(comando3), 0);
            free(comando);
            comando = NULL;
            free(prefix);
            free(comando3);
            free(comando1);
            free(comando2);
            free(channel);
            IRCTAD_FreeListUsersOnChannel (listusers, num);
        }
    }

}

/**
 * @page nick_case \b nick_case
 *
 * @brief Añade o modifica el nick de un usuario en TAD de usuarios.
 *
 * @section SYNOPSIS
 * 	\b #include \b <IRCcommands.h>
 *
 *	\b void \b nick_case \b (\b char\b *\b prefix\b, \b char\b *\b newnick\b, \b char\b *\b msg\b, \b char\b **\b nick\b, const\b int\b sd\b )
 * 
 * @section descripcion DESCRIPCIÓN
 *
 * 
 * Añade o modifica el nick de un usuario en TAD de usuarios.
 * 
 * Recibe como parámetros el prefijo que nos da la funcion de parseo del comando, el nick a añadir o modificar, msg parametro usado entre
 * servidores (Eliminado en RFC 2812), el nick del usuario que envia el mensaje y su socket correspondiente.
 *
 * @section retorno RETORNO
 * 
 * No devuelve nada.
 * 
 * @section authors AUTOR
 * Francisco Andreu Sanz (francisco.andreu@estudiante.uam.es)
 * Javier Martínez Hernández (javier.maritnez@estudiante.uam.es)
*/
void nick_case(char* prefix, char* newnick, char* msg, char** nick, const int sd) {

    char *comando = NULL;
    char** nicklistTAD;
    long nelements;
    int i = 0;
    int valor = TRUE;
    int flag=0;

    if (*nick != NULL) {
        if (*nick == newnick) {
            valor = FALSE;
        }
        if (valor == TRUE) {
            IRCTADUser_GetNickList(&nicklistTAD, &nelements);
            if (nelements > 0) {
                for (i = 0; i < nelements; i++) {
                    if (newnick == nicklistTAD[i]) {
                        valor = FALSE;
                    }
                    if (strcmp(*nick, nicklistTAD[i]) == 0) {

                        IRCTADUser_SetNickByUser(newnick, IRCTADUser_GetUserByNick(nicklistTAD[i]));

                        IRC_Prefix(&prefix, nicklistTAD[i], NULL, NULL, NULL);

                        strcpy(*nick, newnick);
                        IRCMsg_Nick(&comando, prefix + 1, NULL, newnick);

                        send(sd, comando, strlen(comando), 0);
                        //FALTARIA TAMBIEN CAMBIARLO EN NUESTRA LISTA GLOBAL DE NICKS Y SOCKETS
                    }
                }
            }
        }
    } else {
        free(prefix);
        *nick = (char*) malloc(strlen(newnick) * sizeof (char));
        strcpy(*nick, newnick);
        
    }

}

/**
 * @page user_case \b user_case
 *
 * @brief Añade un usuario al TAD de usuarios.
 *
 * @section SYNOPSIS
 * 	\b #include \b <IRCcommands.h>
 *
 *	\b void \b user_case \b (\b char\b *\b prefix\b, \b char\b *\b username\b, \b char\b *\b hostname\b, \b char\b *\b servername\b, \b char\b *\b realname\b, \b char\b **\b nick\b, const\b int\b sd\b )
 * 
 * @section descripcion DESCRIPCIÓN
 *
 * 
 * Añade un usuario al TAD de usuarios.
 * 
 * Recibe como parámetros el prefijo que nos da la funcion de parseo del comando, el nombre del usuario, nombre del host, nombre 
 * del servidor, nombre real, el nick del usuario que envia el mensaje y su socket correspondiente.
 *
 * @section retorno RETORNO
 * 
 * No devuelve nada.
 * 
 * @section authors AUTOR
 * Francisco Andreu Sanz (francisco.andreu@estudiante.uam.es)
 * Javier Martínez Hernández (javier.maritnez@estudiante.uam.es)
*/
void user_case(char* prefix, char* username, char* hostname, char* servername, char* realname, char** nick, int sd) {

    char *comando = NULL;
    char user[20];
    long ret = IRCTADUser_Add (username, *nick, realname, NULL, hostname, servername);


    if (ret == IRC_OK){
        sprintf(user, "~%s", IRCTADUser_GetUserByNick(*nick));
        IRCMsg_RplWelcome(&comando, SERVER_NOMBRE, *nick, *nick, user, IRCTADUser_GetHostByNick(*nick));
        send(sd, comando, strlen(comando), 0);
        free(prefix);
        free(username);
        free(realname);
        free(servername);
        free(hostname);
        free(comando);
    }

    else {//if (ret == IRCERR_REALNAMEUSED || ret == IRCERR_USERUSED || ret == IRCERR_NICKUSED){
        IRCMsg_ErrNickNameInUse (&comando, prefix, *nick, *nick);
        send(sd, comando, strlen(comando), 0);
        free(*nick);
        *nick=NULL;
        close(sd);
        sd = 0;
        pthread_exit(NULL);        
    }
}

/**
 * @page list_case \b list_case
 *
 * @brief Muestra una lista con los canales en el servidor.
 *
 * @section SYNOPSIS
 * 	\b #include \b <IRCcommands.h>
 *
 *	\b void \b list_case \b (\b char\b *\b prefix\b, \b char\b *\b channel\b, \b char\b *\b target\b, \b char\b **\b nick\b, const\b int\b sd\b )
 * 
 * @section descripcion DESCRIPCIÓN
 *
 * 
 * Muestra una lista con los canales en el servidor. Tambien se puede especificar el servidor.
 * 
 * Recibe como parámetros el prefijo que nos da la funcion de parseo del comando, el canal sobre el cual queremos la información, 
 * target nos indica un servidor especifico para solicitar la información, el nick del usuario que envia el mensaje 
 * y su socket correspondiente.
 *
 * @section retorno RETORNO
 * 
 * No devuelve nada.
 * 
 * @section authors AUTOR
 * Francisco Andreu Sanz (francisco.andreu@estudiante.uam.es)
 * Javier Martínez Hernández (javier.maritnez@estudiante.uam.es)
*/
void list_case(char *prefix, char* channel, char* target, char** nick, const int sd) {

    int i;
    char* comando;
    long num;
    char **channels;
    char* topic;
    char visible[10];

    IRCTADChan_GetList(&channels, &num, NULL);
    IRC_Prefix(&prefix, *nick, IRCTADUser_GetUserByNick(*nick), IRCTADUser_GetHostByNick(*nick), IRCTADUser_GetIPByNick(*nick));

    IRCMsg_RplListStart(&comando, prefix+1, *nick);
    send(sd, comando, strlen(comando), 0);
    free(comando);

    for (i = 0; i < num; i++) {
        if (IRCTADChan_GetMode(channels[i]) != IRCMODE_SECRET) {
            topic = IRCTADChan_GetTopic(channels[i], NULL);
            sprintf(visible, "%ld", IRCTADChan_GetNumberOfUsers(channels[i]));
            IRCMsg_RplList(&comando, prefix+1, *nick, channels[i], visible, topic);
            send(sd, comando, strlen(comando), 0);
        }
    }

    IRCMsg_RplListEnd(&comando, prefix+1, *nick);
    send(sd, comando, strlen(comando), 0);
    free(comando);
}
/**
 * @page names_case \b names_case
 *
 * @brief Muestra una lista con todos los usuarios de un canal.
 *
 * @section SYNOPSIS
 * 	\b #include \b <IRCcommands.h>
 *
 *	\b void \b names_case \b (\b char\b *\b prefix\b, \b char\b *\b channel\b, \b char\b *\b target\b, \b char\b **\b nick\b, const\b int\b sd\b )
 * 
 * @section descripcion DESCRIPCIÓN
 *
 * 
 * Muestra una lista con todos los usuarios de un canal. Tambien se puede especificar el servidor.
 * 
 * Recibe como parámetros el prefijo que nos da la funcion de parseo del comando, el canal sobre el cual queremos la información, 
 * target nos indica un servidor especifico para solicitar la información, el nick del usuario que envia el mensaje 
 * y su socket correspondiente.
 *
 * @section retorno RETORNO
 * 
 * No devuelve nada.
 * 
 * @section authors AUTOR
 * Francisco Andreu Sanz (francisco.andreu@estudiante.uam.es)
 * Javier Martínez Hernández (javier.maritnez@estudiante.uam.es)
*/
void names_case(char* prefix, char* channel, char* target, char** nick, const int sd) {

    char* comando;
    int i = 0, j = 0;
    char** listusers;
    char listnick[1024] = "";
    long num;
    char type[7] = "=";
    char* host = IRCTADUser_GetHostByNick(*nick);

    if (channel != NULL) {
        IRCTAD_ListUsersOnChannel(channel, &listusers, &num);
        for (i = 0; i < num; i++) {
            if (i >= 1) {
                strcat(listnick, " ");
            }
            if (strcmp(IRCTADChan_GetCreator(channel), listusers[i]) == 0) {
                strcat(listnick, "@");
            }
            strcat(listnick, IRCTADUser_GetNickByUser(listusers[i]));
        }
        if (strcmp(listnick, "") != 0) {
            IRCMsg_RplNamReply(&comando, SERVER_NOMBRE, *nick, type, channel, listnick);
            send(sd, comando, strlen(comando),0);
                          
        }
        IRCMsg_RplEndOfNames(&comando, SERVER_NOMBRE, *nick, channel);
        send(sd, comando, strlen(comando), 0);
    }
}

/**
 * @page whois_case \b whois_case
 *
 * @brief Muestra información sobre alguien.
 *
 * @section SYNOPSIS
 * 	\b #include \b <IRCcommands.h>
 *
 *	\b void \b whois_case \b (\b char\b *\b prefix\b, \b char\b *\b target\b, \b char\b *\b apodo\b, \b char\b **\b nick\b, const\b int\b sd\b )
 * 
 * @section descripcion DESCRIPCIÓN
 *
 * 
 * Muestra información sobre un alguien. La información depende de si este está en modo away.
 * 
 * Recibe como parámetros el prefijo que nos da la funcion de parseo del comando, target nos indica un servidor especifico para  
 * solicitar la información, el apodo del usuario, el nick del usuario que envia el mensaje y su socket correspondiente.
 *
 * @section retorno RETORNO
 * 
 * No devuelve nada.
 * 
 * @section authors AUTOR
 * Francisco Andreu Sanz (francisco.andreu@estudiante.uam.es)
 * Javier Martínez Hernández (javier.maritnez@estudiante.uam.es)
*/
void whois_case(char *prefix, char* target, char* apodo, char** nick, const int sd) {


    char** buffer;
    char* user2;
    char user[20];
    char* host;
    char* real;
    char* valorAway;
    char* server;
    char* server_info;
    char** channels;
    long numchan;
    char* list;
    int i = 0;
    char* comando;

    IRC_Prefix(&prefix, *nick, IRCTADUser_GetUserByNick(*nick), IRCTADUser_GetHostByNick(*nick), IRCTADUser_GetIPByNick(*nick));

    /* En caso de que no este bien formado el whois*/
    if (!apodo) {
        IRCMsg_ErrNoNickNameGiven(&comando, prefix + 1, *nick);
        send(sd, comando, strlen(comando), 0);
        free(comando);
    }/* En caso de que el target este en modo AWAY*/
    else if ((valorAway = IRCTAD_GetAway(IRCTADUser_GetUserByNick(*nick))) != NULL) {
    	IRCMsg_RplAway(&comando, SERVER_NOMBRE, *nick, apodo, valorAway);
        send(sd, comando, strlen(comando), 0);
        free(comando);
    } else {

        //obtenemos la lista de los canales del usuario
        IRCTAD_ListChannelsOfUser(IRCTADUser_GetUserByNick(*nick), &channels, &numchan);

        /* 311 - RPL_WHOIISUSER(optativo)*/
        user2 = IRCTADUser_GetUserByNick(*nick);
        sprintf(user, "~%s", user2);
        real = IRCTADUser_GetRealnameByNick(*nick);
        IRCMsg_RplWhoIsUser(&comando, SERVER_NOMBRE, *nick, apodo, user, IRCTADUser_GetIPByNick(*nick), real);
        send(sd, comando, strlen(comando), 0);
        free(comando);

        /* 312 - RPL_WHOISSERVER(optativo) */
        server_info = "Servidor de chat hecho por la pareja 2 del grupo 2311";
        IRCMsg_RplWhoIsServer(&comando, SERVER_NOMBRE, *nick, apodo, SERVER_NOMBRE, server_info);
        send(sd, comando, strlen(comando), 0);
        free(comando);

        /* 319 - RPL_WHOIISCHANNELS*/
        buffer = (char**) malloc(numchan * sizeof (char*));
        for (i = 0; i < numchan; i++) {
            buffer[i] = (char*) malloc((strlen(channels[i]) + 1) * sizeof (char));
            sprintf(buffer[i], "@%s", channels[i]);
        }

        IRC_CreateSpaceList(&list, buffer, numchan);
        if (list != NULL) {
            IRCMsg_RplWhoIsChannels(&comando, SERVER_NOMBRE, *nick, apodo, list, NULL);
            send(sd, comando, strlen(comando), 0);
            free(comando);
        }
        for (i = 0; i < numchan; i++) {
            free(buffer[i]);
            free(channels[i]);
            channels[i] = NULL;
            buffer[i] = NULL;
        }
        free(channels);
        free(buffer);
        free(list);

        /* 317 - RPL_WHOISIDLE*/
        IRCMsg_RplWhoIsIdle(&comando, SERVER_NOMBRE, *nick, apodo, 0, "Idle time, signon time");
        send(sd, comando, strlen(comando), 0);
        free(comando);

        /* 318 - RPL_ENDOFWHOIS */
        IRCMsg_RplEndOfWhoIs(&comando, SERVER_NOMBRE, *nick, apodo);
        send(sd, comando, strlen(comando), 0);
        free(comando);

    }
    free(prefix);
    free(target);
    free(apodo);

}

/**
 * @page who_case \b who_case
 *
 * @brief Muestra información sobre un nick o canal.
 *
 * @section SYNOPSIS
 * 	\b #include \b <IRCcommands.h>
 *
 *	\b void \b who_case \b (\b char\b *\b prefix\b, \b char\b *\b mask\b, \b char\b *\b oppar\b, \b char\b **\b nick\b, const\b int\b sd\b )
 * 
 * @section descripcion DESCRIPCIÓN
 *
 * 
 * Muestra información sobre un nick de manera mas escueta. En el caso de poner un #canal mostrará a los usuarios que esten dentro.
 * Se permite recibir un mensaje como información extra.
 * 
 * Recibe como parámetros el prefijo que nos da la funcion de parseo del comando, mask indica el nick o canal del cual queremos 
 * información, un mensaje con información extra, el nick del usuario que envia el mensaje y su socket correspondiente.
 *
 * @section retorno RETORNO
 * 
 * No devuelve nada.
 * 
 * @section authors AUTOR
 * Francisco Andreu Sanz (francisco.andreu@estudiante.uam.es)
 * Javier Martínez Hernández (javier.maritnez@estudiante.uam.es)
*/
void who_case(char *prefix, char* mask, char* oppar, char** nick, const int sd) {

    char **listusers;
    long num;
    int i;
    int j;
    char username[20];
    char * comando;

    if (mask != NULL) {
        if (mask[0] == '#') {
            IRCTAD_ListUsersOnChannel(mask, &listusers, &num);
            //MANDAMOS MSG DE WHO A TODOS LOS USUARIOS DEL CANAL PARA ACTUALIZAR
            sprintf(username, "~%s", IRCTADUser_GetUserByNick(*nick));
            for (i = 0; i < num; i++) {
                //QUE SON LOS 2 ULTIMOS PARAMETROS?
                IRCMsg_RplWhoReply(&comando, SERVER_NOMBRE, *nick, mask, username, IRCTADUser_GetIPByNick(*nick), SERVER_NOMBRE, listusers[i], "H", 0, IRCTADUser_GetRealnameByNick(*nick));
                for (j = 0; j < MAX_CLIENTS; j++) {
                    if (nickList[j] != NULL) {
                        if (strcmp(IRCTADUser_GetNickByUser(listusers[i]), nickList[j]) == 0) {
                            send(client_socket[j], comando, strlen(comando), 0);
                            send(sd, comando, strlen(comando), 0);
                        }
                    }
                }
            }
            IRCMsg_RplEndOfWho(&comando, SERVER_NOMBRE, *nick, mask);
            send(sd, comando, strlen(comando), 0);

        } else {
            //BUCLES ANIDADOS, PARA TODOS LOS CANALES TODOS LOS USUARIOS
        }
    }

}

/**
 * @page privmsg_case \b privmsg_case
 *
 * @brief Envía un mensaje via privado al usuario especificado.
 *
 * @section SYNOPSIS
 * 	\b #include \b <IRCcommands.h>
 *
 *	\b void \b privmsg_case \b (\b char\b *\b prefix\b, \b char\b *\b msgtarget\b, \b char\b *\b msg\b, \b char\b **\b nick\b, const\b int\b sd\b )
 * 
 * @section descripcion DESCRIPCIÓN
 *
 * 
 * Envía un mensaje via privado al usuario especificado. También se usa para el envio por canal, el canal se especifica en la
 * variable msgtarget.
 * 
 * Recibe como parámetros el prefijo que nos da la funcion de parseo del comando, el nick destinatario del mensaje o nombre del canal
 * donde enviamos el mensaje, un mensaje para el usuario indicado, el nick del usuario que envia el mensaje y su socket correspondiente.
 *
 * @section retorno RETORNO
 * 
 * No devuelve nada.
 * 
 * @section authors AUTOR
 * Francisco Andreu Sanz (francisco.andreu@estudiante.uam.es)
 * Javier Martínez Hernández (javier.maritnez@estudiante.uam.es)
*/
void privmsg_case(char *prefix, char* msgtarget, char* msg, char** nick, const int sd) {

    char* comando, *comando1;
    char *valorAway = NULL;
    char** listnicks;
    char** listusers;
    long num;
    int i = 0;
    int j = 0;
    int flag =0;

	if(*nick!=NULL){

	    IRC_Prefix(&prefix, *nick, IRCTADUser_GetUserByNick(*nick), NULL, IRCTADUser_GetIPByNick(*nick));
	    if (msgtarget[0] == '#') {
		IRCTAD_ListUsersOnChannel(msgtarget, &listusers, &num);
		if(num!=0){
			for (i = 0; i < num; i++) {
			    for (j = 0; j < MAX_CLIENTS; j++) {
				if (nickList[j] != NULL && (strcmp(nickList[j], *nick) != 0)) {
				    if (strcmp(IRCTADUser_GetNickByUser(listusers[i]), nickList[j]) == 0) {
					IRCMsg_Privmsg(&comando, prefix + 1, msgtarget, msg);
					send(client_socket[j], comando, strlen(comando), 0);
                    flag++;
				    }
				}
			    }
			}
		}

	    } else {
		IRCTADUser_GetNickList(&listnicks, &num);
		if(num!=0){
			for (i = 0; i < num; i++) {
			    if (strcmp((listnicks[i]), msgtarget) == 0) {
				IRCMsg_Privmsg(&comando, prefix + 1, msgtarget, msg);
				for (j = 0; j < MAX_CLIENTS; j++) {
				    if (nickList[j] != NULL) {
				        if (strcmp(msgtarget, nickList[j]) == 0) {
				            if ((valorAway = IRCTAD_GetAway(IRCTADUser_GetUserByNick(nickList[j]))) != NULL) {
				            	printf("Entro en rplaway de privmsg\n");
				                IRCMsg_RplAway(&comando1, SERVER_NOMBRE, *nick, nickList[j], valorAway);
				                send(sd, comando1, strlen(comando1),0);
				            } 
				            send(client_socket[j], comando, strlen(comando), 0);
                            flag++;
				            break;
				        }
				    }
				}
				break;
			    }
			}
		}
        if (flag==0){
	       	IRCMsg_ErrNoSuchNick(&comando, prefix + 1, *nick, msgtarget);
    		send(sd, comando, strlen(comando), 0);
	    }
        }
	}
}

/**
 * @page ping_case \b ping_case
 *
 * @brief Se envia un ping para comprobar conexión.
 *
 * @section SYNOPSIS
 * 	\b #include \b <IRCcommands.h>
 *
 *	\b void \b ping_case \b (\b char\b *\b prefix\b, \b char\b *\b server1\b, \b char\b *\b server2\b, \b char\b *\b msg\b, \b char\b **\b nick\b, const\b int\b sd\b )
 * 
 * @section descripcion DESCRIPCIÓN
 *
 * 
 * Se envia un ping para comprobar conexión. Se indica a quien se realiza el ping.
 * 
 * Recibe como parámetros el prefijo que nos da la funcion de parseo del comando,el servidor para enviar el ping, un mensaje   
 * opcional para el ping, el nick del usuario que envia el mensaje y su socket correspondiente.
 *
 * @section retorno RETORNO
 * 
 * No devuelve nada.
 * 
 * @section authors AUTOR
 * Francisco Andreu Sanz (francisco.andreu@estudiante.uam.es)
 * Javier Martínez Hernández (javier.maritnez@estudiante.uam.es)
*/
void ping_case(char *prefix, char* server1, char* server2, char* msg, char** nick, const int sd) {

    char* comando;


    IRCMsg_Pong(&comando, SERVER_NOMBRE, SERVER_NOMBRE, server2, server1);
    send(sd, comando, strlen(comando), 0);

}

/**
 * @page part_case \b part_case
 *
 * @brief Sale del canal especificado.
 *
 * @section SYNOPSIS
 * 	\b #include \b <IRCcommands.h>
 *
 *	\b void \b part_case \b (\b char\b *\b prefix\b, \b char\b *\b channel\b, \b char\b *\b msg\b, \b char\b **\b nick\b, const\b int\b sd\b )
 * 
 * @section descripcion DESCRIPCIÓN
 *
 * 
 * Sale el usuario del canal especificado. Mensaje de despedida opcinal.
 * 
 * Recibe como parámetros el prefijo que nos da la funcion de parseo del comando,el canal que queremos abandonar, el mensaje que  
 * deseamos de despedida, el nick del usuario que envia el mensaje y su socket correspondiente.
 *
 * @section retorno RETORNO
 * 
 * No devuelve nada.
 * 
 * @section authors AUTOR
 * Francisco Andreu Sanz (francisco.andreu@estudiante.uam.es)
 * Javier Martínez Hernández (javier.maritnez@estudiante.uam.es)
*/
void part_case(char* prefix, char* channel, char* msg, char** nick, const int sd) {

    char* comando,*user;
    int i,j,ret,existeCanal=0;
    char **listusers,**listchannels;
    long num,numChannels;

                
		IRCTADChan_GetList(&listchannels, &numChannels, NULL);
		for (i=0; i<numChannels; i++) {
			if (strcmp(channel, listchannels[i])==0) {
				existeCanal=1;
				break;
			}
		}

		IRCTADChan_FreeList(listchannels, numChannels);

		if (!channel) {
			free(prefix); free(channel); free(msg);	
			return;
		} else if (existeCanal != 1) {	
//			syslog(LOG_INFO, "no existe el canal");
			IRCMsg_ErrNoSuchChannel(&comando, SERVER_NOMBRE, *nick, channel);
			send(sd, comando, strlen(comando), 0);

		} else {

			sprintf(user, "~%s", IRCTADUser_GetUserByNick(*nick));
			IRC_Prefix(&prefix, *nick, user, IRCTADUser_GetHostByNick(*nick),IRCTADUser_GetIPByNick(*nick));
			
			ret=IRCTAD_PartChannel(channel, IRCTADUser_GetUserByNick(*nick));
			if ( ret == IRCERR_NOVALIDCHANNEL) {
				IRCMsg_ErrNoSuchChannel(&comando, prefix+1, *nick, channel);
				send(sd, comando, strlen(comando), 0);
			} 
			else if(ret == IRCERR_NOSTRING) {
				free(prefix); free(channel); free(msg);
			} else if (ret == IRCERR_ERRONEUSCOMMAND) {
				free(prefix); free(channel); free(msg);	
			}else {	
				if(IRCTADChan_GetNumberOfUsers(channel)<=0){
	//				syslog(LOG_INFO, "Eliminar canal %s", channel);
					IRCTADChan_Delete(channel);
				}

				IRCMsg_Part(&comando, prefix+1, channel, msg);				
				send(sd, comando, strlen(comando), 0);
				free(comando);
				free(channel);
				free(prefix);
			}
    }

}

/**
 * @page topic_case \b topic_case
 *
 * @brief Cambia el tema o mensaje explicativo del canal.
 *
 * @section SYNOPSIS
 * 	\b #include \b <IRCcommands.h>
 *
 *	\b void \b topic_case \b (\b char\b *\b prefix\b, \b char\b *\b channel\b, \b char\b *\b topic\b, \b char\b **\b nick\b, const\b int\b sd\b )
 * 
 * @section descripcion DESCRIPCIÓN
 *
 * 
 * Cambia el tema o mensaje explicativo del canal que todos los usuarios verán junto al nombre al hacer un /list, 
 * y en la barra de título de la ventana del canal
 * 
 * Recibe como parámetros el prefijo que nos da la funcion de parseo del comando,el canal donde modificaremos el tema, el mensaje que  
 * deseamos en el canal, el nick del usuario que envia el mensaje y su socket correspondiente.
 *
 * @section retorno RETORNO
 * 
 * No devuelve nada.
 * 
 * @section authors AUTOR
 * Francisco Andreu Sanz (francisco.andreu@estudiante.uam.es)
 * Javier Martínez Hernández (javier.maritnez@estudiante.uam.es)
*/
void topic_case(char *prefix, char* channel, char* topic, char** nick, const int sd) {

    char* comando;
    long mode;

    IRC_Prefix(&prefix, *nick, IRCTADUser_GetUserByNick(*nick), IRCTADUser_GetHostByNick(*nick), IRCTADUser_GetIPByNick(*nick));

    if (topic == NULL) {
        if (strcmp(IRCTADChan_GetTopic(channel, NULL), "") == 0)
            IRCMsg_RplNoTopic(&comando, prefix + 1, *nick, channel, "El canal no tiene topic");
        else
            IRCMsg_RplTopic(&comando, prefix + 1, *nick, channel, IRCTADChan_GetTopic(channel, NULL));
    } else {
        mode = IRCTAD_GetUserModeOnChannel(channel, IRCTADUser_GetUserByNick(*nick));
        if (IRCTADChan_GetMode(channel) == IRCMODE_TOPICOP) {
            if ((mode == IRCUMODE_OPERATOR) || (mode == IRCUMODE_CREATOR) || (mode == 3)) {
                IRCTADChan_SetTopic(channel, topic);
                IRCMsg_Topic(&comando, prefix + 1, channel, topic);
            } else
                IRCMsg_ErrChanOPrivsNeeded(&comando, prefix + 1, *nick, channel);
        } else {
            IRCTADChan_SetTopic(channel, topic);
            IRCMsg_Topic(&comando, prefix + 1, channel, topic);
        }
    }
    send(sd, comando, strlen(comando), 0);
    free(comando);
    comando = NULL;
    free(prefix);
    if (channel)
        free(channel);


}

/**
 * @page kick_case \b kick_case
 *
 * @brief Expulsa al usuario que le indiquemos del canal especificado. Opcionalmente añade un comentario.
 *
 * @section SYNOPSIS
 * 	\b #include \b <redes2/chat.h>
 *
 *	\b void \b kick_case \b (\b char\b *\b prefix\b, \b char\b *\b channel\b, \b char\b *\b user\b, \b char\b *\b comment\b, \b char\b **\b nick\b, const\b int\b sd\b )
 * 
 * @section descripcion DESCRIPCIÓN
 *
 * Expulsa al usuario que le indiquemos del canal especificado. Opcionalmente añade un comentario.
 * 
 * Recibe como parámetros el prefijo que nos da la funcion de parseo del comando,el canal para expulsar al usuario, el usuario que  
 * deseamos expulsar, el mensaje que explica el motivo de la expulsión, el nick del usuario que envia el 
 * mensaje y su socket correspondiente.
 *
 * @section retorno RETORNO
 * 
 * No devuelve nada.
 * 
 * @section seealso VER TAMBIÉN
 * \b
 * 
 * @section authors AUTOR
 * Francisco Andreu Sanz (francisco.andreu@estudiante.uam.es)
 * Javier Martínez Hernández (javier.maritnez@estudiante.uam.es)
*/
void kick_case(char *prefix, char* channel, char* user, char* comment, char** nick, const int sd) {

    char** listusers;
    long numberOfUsers;
    int i, j;
    char* comando;

    IRC_Prefix(&prefix, *nick, IRCTADUser_GetUserByNick(*nick), IRCTADUser_GetHostByNick(*nick), IRCTADUser_GetIPByNick(*nick));

    if (IRCTAD_GetUserModeOnChannel(channel, IRCTADUser_GetUserByNick(*nick)) < IRCUMODE_OPERATOR) {
        IRCMsg_ErrChanOPrivsNeeded(&comando, prefix + 1, *nick, channel);
        send(sd, comando, strlen(comando), 0);

    } else {
        IRCTAD_KickUserFromChannel(channel, user);
        IRCMsg_Kick(&comando, prefix + 1, channel, user, comment);

        /*Se lo enviamos al que acabamos de echar*/
        for (i = 0; i < MAX_CLIENTS; i++) {
            if (nickList[i] != NULL)
                if (strcmp(IRCTADUser_GetNickByUser(user), nickList[i]) == 0) {
                    send(client_socket[i], comando, strlen(comando), 0);
                    break;
                }
        }
        /*Tambien notificamos al resto de usuarios del canal*/
        IRCTAD_ListUsersOnChannel(channel, &listusers, &numberOfUsers);
        for (i = 0; i < numberOfUsers; i++) {
            for (j = 0; j < MAX_CLIENTS; j++)
                if (nickList[j] != NULL)
                    if (strcmp(listusers[i], nickList[j]) == 0)
                        send(client_socket[j], comando, strlen(comando), 0);
        }
    }

}
/**
 * @page away_case \b away_case
 *
 * @brief Deja un mensaje explicando que en ese momento no estamos prestando atención.
 *
 * @section SYNOPSIS
 * 	\b #include \b <redes2/chat.h>
 *
 *	\b void \b away_case \b (\b char\b *\b prefix\b, \b char\b *\b msg\b, \b char\b **\b nick\b, const\b int\b sd\b )
 * 
 * @section descripcion DESCRIPCIÓN
 *
 * Deja un mensaje explicando que en ese momento no estamos prestando atención.
 * 
 * Recibe como parámetros el prefijo que nos da la funcion de parseo del comando,el mensaje de que explica que ese 
 * momento no estamos prestando atención, el nick del usuario que envia el mensaje y su socket correspondiente.
 *
 * @section retorno RETORNO
 * 
 * No devuelve nada.
 * 
 * @section seealso VER TAMBIÉN
 * \b
 * 
 * @section authors AUTOR
 * Francisco Andreu Sanz (francisco.andreu@estudiante.uam.es)
 * Javier Martínez Hernández (javier.maritnez@estudiante.uam.es)
*/
void away_case(char *prefix, char* msg, char** nick, const int sd) {

    char* comando;
    if (msg == NULL) {
        IRCTAD_DeleteAway(IRCTADUser_GetUserByNick(*nick));

        IRCMsg_RplUnaway(&comando, SERVER_NOMBRE, *nick);
        send(sd, comando, strlen(comando), 0);
    } else {

        IRCTAD_SetAway(*nick, msg);
        IRCMsg_RplNowAway(&comando, SERVER_NOMBRE, *nick);
        send(sd, comando, strlen(comando), 0);
    }

}

/**
 * @page quit_case \b quit_case
 *
 * @brief Cierra la conexion con el servidor IRC. Opcionalmente envia un mensaje de despedida.
 *
 * @section SYNOPSIS
 * 	\b #include \b <redes2/chat.h>
 *
 *	\b void \b quit_case \b (\b char\b *\b prefix\b, \b char\b *\b msg\b, \b char\b **\b nick\b, const\b int\b sd\b )
 * 
 * @section descripcion DESCRIPCIÓN
 *
 * Cierra la conexion con el servidor IRC. Opcionalmente envia un mensaje de despedida.
 * 
 * Recibe como parámetros el prefijo que nos da la funcion de parseo del comando,el mensaje de despedida, el nick del usuario que envia el mensaje y su socket correspondiente.
 *
 * @section retorno RETORNO
 * 
 * No devuelve nada.
 * 
 * @section seealso VER TAMBIÉN
 * \b
 * 
 * @section authors AUTOR
 * Francisco Andreu Sanz (francisco.andreu@estudiante.uam.es)
 * Javier Martínez Hernández (javier.maritnez@estudiante.uam.es)
*/
void quit_case(char *prefix, char* msg, char** nick, int sd) {

    char* comando;
    int i, k;
    
            IRC_Prefix(&prefix, *nick, IRCTADUser_GetUserByNick(*nick), NULL, SERVER_NOMBRE);   
            
            IRCTAD_Quit(IRCTADUser_GetUserByNick(*nick));
            IRCMsg_Quit (&comando, prefix+1, msg);
            for(i=0; i< MAX_CLIENTS; i++)
                if(nickList[i] && (client_socket[i] != 0)){
                    send(client_socket[i], comando, strlen(comando), 0);
                }
            
            for(i=0; i < MAX_CLIENTS; i++){
            	if(nickList[i]){
                    if (strcmp(*nick, nickList[i]) == 0){
						close(client_socket[i]);
						client_socket[i] = 0;
                        nickList[i] = NULL;                        
                        break;
                    }
                }
            }

                        return;
}

/**
 * @page mode_case \b mode_case
 *
 * @brief Modifica el modo del canal
 *
 * @section SYNOPSIS
 * 	\b #include \b <redes2/chat.h>
 *
 *	\b void \b mode_case \b (\b char\b *\b prefix\b, \b char\b *\b channel\b,\b char\b *\b mode\b, \b char\b *\b user\b, \b char\b **\b nick\b, const\b int\b sd\b )
 * 
 * @section descripcion DESCRIPCIÓN
 *
 * Modifica el modo del canal si este se especifica, en caso contrario indica el modo del canal.
 * 
 * Recibe como parámetros el prefijo que nos da la funcion de parseo del comando,el canal sobre el cual se ejecuta el modo, 
 * el modo del canal, el usuario que solicita el cambio, el nick del usuario que envia el mensaje y su socket correspondiente.
 *
 * @section retorno RETORNO
 * 
 * No devuelve nada.
 * 
 * @section seealso VER TAMBIÉN
 * \b
 * 
 * @section authors AUTOR
 * Francisco Andreu Sanz (francisco.andreu@estudiante.uam.es)
 * Javier Martínez Hernández (javier.maritnez@estudiante.uam.es)
*/
void mode_case(char *prefix, char* channel, char* mode, char* user, char** nick, const int sd) {

    char* comando;
    long compare;

    if (mode != NULL) {
	    IRC_Prefix(&prefix, *nick, IRCTADUser_GetUserByNick(*nick), IRCTADUser_GetHostByNick(*nick), IRCTADUser_GetIPByNick(*nick));
        if (strcmp(mode, "+s") == 0) {
            IRCTADChan_SetMode(channel, mode);            
            IRCMsg_Mode(&comando, prefix + 1, channel, mode, user);
        } else if (strcmp(mode, "\\+k") == 0) {
            IRCTADChan_SetMode(channel, mode);
            IRCTADChan_SetPassword(channel, user);
            IRCMsg_Mode(&comando, prefix + 1, channel, mode, user);
        } else if (strcmp(mode, "+t") == 0) {
            IRCTADChan_SetMode(channel, mode);
            IRCMsg_Mode(&comando, prefix + 1, channel, mode, user);
        } else {
            IRCMsg_ErrUnknownMode(&comando, prefix + 1, *nick, mode, channel);
        }

    } else {
        compare = IRCTADChan_GetMode(channel);
        if ((compare & IRCMODE_TOPICOP) == IRCMODE_TOPICOP)
            IRCMsg_RplChannelModeIs(&comando, SERVER_NOMBRE, *nick, channel, "+t");
        else if ((compare & IRCMODE_SECRET) == IRCMODE_SECRET)
            IRCMsg_RplChannelModeIs(&comando, SERVER_NOMBRE, *nick, channel, "+s");
        else if ((compare & IRCMODE_CHANNELPASSWORD) == IRCMODE_CHANNELPASSWORD)
            IRCMsg_RplChannelModeIs(&comando, SERVER_NOMBRE, *nick, channel, "\\+k");
        else
            IRCMsg_RplChannelModeIs(&comando, SERVER_NOMBRE, *nick, channel, "+");
    }
    send(sd, comando, strlen(comando), 0);
    free(comando);
    free(prefix);
    if (channel)
        free(channel);
    if (mode)
        free(mode);
    if (user)
        free(user);

}

/**
 * @page motd_case \b motd_case
 *
 * @brief Envia un mensaje de comando desconocido.
 *
 * @section SYNOPSIS
 * 	\b #include \b <redes2/chat.h>
 *
 *	\b void \b unknown_case \b (\b char\b *\b prefix\b, \b char\b *\b target\b, \b char\b **\b nick\b, const\b int\b sd\b )
 * 
 * @section descripcion DESCRIPCIÓN
 *
 * Envia un mensaje de comando desconocido al cliente que introduce el comando.
 * 
 * Recibe como parámetros el prefijo que nos da la funcion de parseo del comando,el target, el nick del usuario 
 * que envia el mensaje y su socket correspondiente.
 *
 * @section retorno RETORNO
 * 
 * No devuelve nada.
 * 
 * @section seealso VER TAMBIÉN
 * \b
 * 
 * @section authors AUTOR
 * Francisco Andreu Sanz (francisco.andreu@estudiante.uam.es)
 * Javier Martínez Hernández (javier.maritnez@estudiante.uam.es)
*/
void motd_case(char *prefix, char* target, char** nick, const int sd) {

    char* comando;
    FILE *F_IN;
    char line[20];

    IRC_Prefix(&prefix, *nick, IRCTADUser_GetUserByNick(*nick), IRCTADUser_GetHostByNick(*nick), IRCTADUser_GetIPByNick(*nick));
    if (!target)
        IRCMsg_RplMotdStart(&comando, prefix + 1, *nick, SERVER_NOMBRE);
    else
        IRCMsg_RplMotdStart(&comando, prefix + 1, *nick, target);
    send(sd, comando, strlen(comando), 0);
    free(comando);

    F_IN = fopen("MOTD.txt", "rt");

    if (!F_IN) {
        //IRCMsg_ErrNoMotd(&comando, prefix + 1, *nick);
        //send(sd, comando, strlen(comando), 0);
        IRCMsg_RplMotd(&comando, prefix + 1, *nick, "Mensaje por defecto");
        send(sd, comando, strlen(comando), 0);
        IRCMsg_RplEndOfMotd(&comando, prefix + 1, *nick);
        send(sd, comando, strlen(comando), 0);
        free(comando);
    } else {

        while (fgets(line, 19, F_IN) != NULL) {
            IRCMsg_RplMotd(&comando, prefix + 1, *nick, line);
            comando[strlen(comando) - 1] = 0;
            send(sd, comando, strlen(comando), 0);
            free(comando);
        }
        if (F_IN)   
            fclose(F_IN);
        F_IN = NULL;
        IRCMsg_RplEndOfMotd(&comando, prefix + 1, *nick);
        send(sd, comando, strlen(comando), 0);
        free(comando);
    }


}

/**
 * @page unknown_case \b unknown_case
 *
 * @brief Envia un mensaje de comando desconocido.
 *
 * @section SYNOPSIS
 * 	\b #include \b <redes2/chat.h>
 *
 *	\b void \b unknown_case \b (\b char\b *\b command\b, \b char\b **\b nick\b, const\b int\b sd\b )
 * 
 * @section descripcion DESCRIPCIÓN
 *
 * Envia un mensaje de comando desconocido al cliente que introduce el comando.
 * 
 * Recibe como parámetros el comando desconocido, el nick del usuario que envia el mensaje y su socket correspondiente.
 *
 * @section retorno RETORNO
 * 
 * No devuelve nada.
 * 
 * @section seealso VER TAMBIÉN
 * \b
 * 
 * @section authors AUTOR
 * Francisco Andreu Sanz (francisco.andreu@estudiante.uam.es)
 * Javier Martínez Hernández (javier.maritnez@estudiante.uam.es)
*/
void unknown_case(char *command, char** nick, const int sd) {

    char* comando;
    char* prefix;

    if (!(*nick)) {
        IRC_Prefix(&prefix, "", "", "", NULL);
        IRCMsg_ErrUnKnownCommand(&comando, prefix + 1, "", command);
        send(sd, comando, strlen(comando), 0);
        free(comando);
    } else {

        IRC_Prefix(&prefix, *nick, IRCTADUser_GetUserByNick(*nick), IRCTADUser_GetHostByNick(*nick), IRCTADUser_GetIPByNick(*nick));
        IRCMsg_ErrUnKnownCommand(&comando, prefix + 1, *nick, command);
        send(sd, comando, strlen(comando), 0);
        free(comando);
    }
}
