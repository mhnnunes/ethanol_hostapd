// ----------------------------------------------------------------------------
/** \file nl80211_socket.h
   \verbatim
   System:         Linux
   Component Name: Ethanol, Netlink, getmacaddress
   Status:         Version 1.0 Release 1
   Language: C

   License: GNU Public License

   Description: Este módulo permite manipular os sockets do netlink
   	   	   	    provê ainda um procedimento padronizado (start_socket) que cria socket, mensagem e callbacks

   Limitations: funciona somente em ambiente linux com sockets
                pode ser obtido também lendo /sys/class/net/<interface>/address

   Function: 1) nl_socket_free
			 2) nl_socket_set_buffer_size
			 3) error_handler
			 4) finish_handler
			 5) ack_handler
			 6) start_socket

   Thread Safe: NO

   Compiler Options: none

   Change History:                  (Sometimes called "Revisions")
   Date         Author       Description
   10/03/2015   Henrique     primeiro release
   \endverbatim
*/
#ifndef __NL80211_SOCKET_H
#define __NL80211_SOCKET_H

/* SOCKET */

/** cria o socket */
struct nl_handle *nl_socket_alloc(void);
/** libera o socket */
void nl_socket_free(struct nl_handle *h);
/** determina o tamanho do buffer para o socket */
int nl_socket_set_buffer_size(struct nl_handle *sk, int rxbuf, int txbuf);


/* DEFAULT HANDLERS */
/** handler padronizado para erro */
int error_handler(struct sockaddr_nl *nla, struct nlmsgerr *err, void *arg);
/** handler padronizado para procedimento de fim */
int finish_handler(struct nl_msg *msg, void *arg);
/** handler padronizado para procedimento de ack */
int ack_handler(struct nl_msg *msg, void *arg);

#define ERR_STSCKT_NETLINKSOC_ALLOC -1
#define ERR_STSCKT_NETLINKSOC_CONNECT -2
#define ERR_STSCKT_NL80211_NOTFOUND -3
#define ERR_STSCKT_NLMSG_ALLOC -4
#define ERR_STSCKT_CALLBACK_ALLOC -5

/** procedimento padronizado para criar socket, mensagem e callbacks */
int start_socket(struct nl_handle ** sock,
				 int * nl80211_id,
				 struct nl_cb ** cb,
				 struct nl_cb ** s_cb,
				 struct nl_msg ** msg);


#endif /* __NL80211_SOCKET_H */
