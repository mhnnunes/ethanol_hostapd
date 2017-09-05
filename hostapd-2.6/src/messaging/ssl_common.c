#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <malloc.h>
#include <string.h>
#include <sys/socket.h>
#include <resolv.h>
#include <netdb.h>

#include <openssl/ssl.h>
#include <openssl/err.h>

#include "ssl_common.h"
#include "msg_common.h"

/** initialize context parameter for the ssl connection */
SSL_CTX * InitCTX(void) {
    SSL_CTX * ctx = NULL;

    OpenSSL_add_all_algorithms();   /* Load cryptos, et.al. */
    SSL_load_error_strings();       /* Bring in and register error messages */
    const SSL_METHOD * method = SSLv3_client_method(); /* Create new client-method instance */
    /* Create new context
       NULL:The creation of a new SSL_CTX object failed */
    ctx = SSL_CTX_new(method);
    if ( ctx == NULL ) {
        ERR_print_errors_fp(stderr);
    }
    return ctx;
}

// cria a conexão com o servidor
int OpenConnection(const char *hostname, int port) {
    int sd;
    struct hostent *host;
    struct sockaddr_in addr;

    if ( (host = gethostbyname(hostname)) == NULL ) {
        perror(hostname);
        return FAIL;
    }
    sd = socket(PF_INET, SOCK_STREAM, 0);
    bzero(&addr, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = *(long*)(host->h_addr);
    if ( connect(sd, (struct sockaddr*)&addr, sizeof(addr)) != 0 ) {
        close(sd);
        perror(hostname);
        return FAIL;
    }
    return sd;
}

/**
 * show the certificate used in communication
 */
void ShowCerts(SSL* ssl) {
    X509 *cert;
    char *line;

    cert = SSL_get_peer_certificate(ssl); /* get the server's certificate */
    if ( cert != NULL ) {
        line = X509_NAME_oneline(X509_get_subject_name(cert), 0, 0);
        #ifdef DEBUG
          printf( "Server certificate:\n");
          printf( "Subject: %s\n", line);
        #endif
        free(line);          /* free the malloc'ed string */
        line = X509_NAME_oneline(X509_get_issuer_name(cert), 0, 0);
        #ifdef DEBUG
          printf( "Issuer: %s\n", line);
        #endif
        free(line);          /* free the malloc'ed string */
        X509_free(cert);     /* free the malloc'ed certificate copy */
    }
    #ifdef DEBUG
    else printf( "Certificate not supplied.\n");
    #endif
}

/** creates the connection using SSL socket
    returns a negative value if an error occurs
 */
int get_ssl_connection(char * hostname, int portnum, struct ssl_connection * h) {
  h->ssl = NULL; // cannot connect
  h->server = FAIL;
  h->ctx = NULL;

  SSL_library_init();    // 1) initiate parameters  (SSL_library_init() always returns "1")
  h->ctx = InitCTX();    // 2) CTX
  if (NULL == h->ctx) return -1;
  h->server = OpenConnection(hostname, portnum); // 3) get connection
  if (FAIL == h->server) return -2;
  h->ssl = SSL_new(h->ctx);                     // 4) create new SSL connection state
  SSL_set_fd(h->ssl, h->server);                // 5) attach the socket descriptor
  if ( SSL_connect(h->ssl) == FAIL ) {          // 6) perform the connection
    ERR_print_errors_fp(stderr);
  }
  #ifdef DEBUG
    printf( "get_ssl_connection: returned a ssl_connection structure\n");
  #endif
  return 0;
}

/** disconnects SSL socket */
void close_ssl_connection(struct ssl_connection * h) {
  if (h == NULL) return;
  if (NULL != h->ssl) SSL_free(h->ssl);       //  8) release connection state
  if (h->server != FAIL) close(h->server);    //  9) close socket
  if (NULL != h->ctx) SSL_CTX_free(h->ctx);   // 10) release context CTX
  #ifdef DEBUG
    printf( "SSL connection closed.\n");
  #endif
}