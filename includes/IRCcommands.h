/*************************************************************
 * Proyecto: REDES II
 * Fichero: IRCcommands.h
 * Fecha: 04/02/2016
 * Descripcion: Declaracion de las funciones auxiliares que se invocan desde ejecutarAccion
 * Integrantes: Francisco Andreu Sanz, Javier Martínez Hernández
 *************************************************************/

#include "cabeceras.h"

#ifndef IRCCOMMANDS_H_
#define IRCCOMMANDS_H_  

void join_case(char *prefix, char* channel, char* key, char* msg, char** nick, const int sd, char* command);
void nick_case(char* prefix, char* newnick, char* msg, char** nick, int sd);
void user_case( char* prefix, char* username, char* hostname, char* servername, char* realname, char** nick, int sd);
void list_case(char *prefix, char* channel, char* target, char** nick, const int sd);
void names_case(char* prefix, char* channel, char* target,char** nick, const int sd);
void whois_case(char *prefix, char* target, char* apodo, char** nick, const int sd);
void who_case(char *prefix, char* mask, char* oppar, char** nick, const int sd);
void privmsg_case(char *prefix, char* msgtarget, char* msg, char** nick, const int sd);
void ping_case(char *prefix, char* server1, char* server2, char* msg, char** nick, const int sd);
void part_case(char* prefix, char* channel, char* msg, char** nick, const int sd);
void topic_case(char *prefix, char* channel, char* topic, char** nick, const int sd);
void kick_case(char *prefix, char* channel, char* user, char* comment, char** nick, const int sd);
void away_case(char *prefix, char* msg, char** nick, const int sd);
void quit_case(char *prefix, char* msg, char** nick, int sd);
void mode_case(char *prefix, char* channel, char* mode, char* user, char** nick, const int sd);
void motd_case(char *prefix, char* target, char** nick, const int sd);
void unknown_case(char *command, char** nick, const int sd);

#endif /* IRCCOMMANDS_H_ */
