#include <netlink/netlink.h> // nla_put_u32
#include <netlink/genl/genl.h>
#include <netlink/genl/family.h>
#include <netlink/genl/ctrl.h>
#include <netlink/msg.h>
#include <netlink/attr.h>
//#include <endian.h>

#include "nl80211.h"
#include "ieee80211.h"
#include "utils.h"
#include "nl80211_socket.h"

#include <net/if.h> // if_nametoindex

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h> // read, close

#include <stdbool.h> // boolean

/** cria o socket */
struct nl_handle *nl_socket_alloc(void) {
	return nl_handle_alloc();
}

/* libera o socket */
void nl_socket_free(struct nl_handle *h) {
	nl_handle_destroy(h);
}

/* determina o tamanho do buffer para o socket */
int nl_socket_set_buffer_size(struct nl_handle *sk, int rxbuf, int txbuf) {
	return nl_set_buffer_size(sk, rxbuf, txbuf);
}

/* handler padronizado para erro */
int error_handler(struct sockaddr_nl *nla, struct nlmsgerr *err, void *arg) {
	int *ret = arg;
	*ret = err->error;
	return NL_STOP;
}

/* handler padronizado para procedimento de fim */
int finish_handler(struct nl_msg *msg, void *arg) {
	int *ret = arg;
	*ret = 0;
	return NL_SKIP;
}
/* handler padronizado para procedimento de ack */
int ack_handler(struct nl_msg *msg, void *arg) {
	int *ret = arg;
	*ret = 0;
	return NL_STOP;
}

int start_socket(struct nl_handle ** sock,
				 int * nl80211_id,
				 struct nl_cb ** cb,
				 struct nl_cb ** s_cb,
				 struct nl_msg ** msg) {

	// tenta criar o socket
	*sock = nl_socket_alloc();
	if (!*sock) {
		fprintf(stderr, "Não conseguiu alocar o socket netlink.\n");
		return ERR_STSCKT_NETLINKSOC_ALLOC;
	}
	// conecta no socket criado
	// int err = nl_socket_set_buffer_size(*sock, 0,0); // 8192, 8192);
	// fprintf(stderr, "nl_socket_set_buffer_size %d\n", err);

	if (genl_connect(*sock)) {
		fprintf(stderr, "Não conseguiu conectar ao socket netlink genérico.\n");
		nl_socket_free(*sock);
		return ERR_STSCKT_NETLINKSOC_CONNECT;
	}

	// verifica se existe nl80211
	*nl80211_id = genl_ctrl_resolve(*sock, "nl80211");
	if (*nl80211_id < 0) {
		fprintf(stderr, "Não localizou nl80211 %d.\n", *nl80211_id);
		nl_socket_free(*sock);
		return ERR_STSCKT_NL80211_NOTFOUND;
	}
	// aloca mensagem
	*msg = nlmsg_alloc();
	if (!*msg) {
		fprintf(stderr, "Falha na alocação de mensagem netlink\n");
		nl_socket_free(*sock);
		return ERR_STSCKT_NLMSG_ALLOC;
	}
#ifdef DEBUG
	*cb = nl_cb_alloc(NL_CB_DEBUG);
	*s_cb = nl_cb_alloc(NL_CB_DEBUG);
#else
	*cb = nl_cb_alloc(NL_CB_DEFAULT);
	*s_cb = nl_cb_alloc(NL_CB_DEFAULT);
#endif
	if (!*cb || !*s_cb) {
		fprintf(stderr, "Falha na alocação de callbacks netlink\n");
		// libera msg e sock
		nlmsg_free(*msg);
		nl_socket_free(*sock);
		return ERR_STSCKT_CALLBACK_ALLOC;
	}
  return 0;
}
