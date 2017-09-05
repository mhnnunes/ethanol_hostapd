#ifndef SSL_COMMON_H
#define SSL_COMMON_H

#define FAIL    -1

/** defines a SSL connection */
struct ssl_connection {
    SSL *ssl;
    SSL_CTX *ctx;
    int server;
};

/**
 * decodes and prints the certificate current in use by SSL
 */
void ShowCerts(SSL* ssl);

/*
  create a connection to a server (hostname:portnum)
  return in h, the ssl context, the connection e ssl handlers

  calling example:
  ================
  struct ssl_connection h_ssl;
  get_ssl_connection(hostname, portnum, &h_ssl);
 */
int get_ssl_connection(char * hostname, int portnum, struct ssl_connection * h);

/*
  disconnected from a server 
  frees ssl context and closes the connection

  calling example:
  ================
  struct ssl_connection h_ssl;
  ...
  close_ssl_connection(&h_ssl);
 */
void close_ssl_connection(struct ssl_connection * h);

#endif