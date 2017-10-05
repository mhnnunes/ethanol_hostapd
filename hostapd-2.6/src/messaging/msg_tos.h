#ifndef ____MSG_TOS
#define ____MSG_TOS

/* message structure -- MSG_TOS_CLEANALL */
typedef struct msg_tos_cleanall {
   int m_type;
   int m_id;
   char * p_version;
   int m_size;
} msg_tos_cleanall;

void process_msg_tos_cleanall(char ** input, int input_len, char ** output, int * output_len);
void send_msg_tos_cleanall(char * hostname, int portnum, int * id);
void free_msg_tos_cleanall(msg_tos_cleanall ** m);

/* message structure - MSG_TOS_ADD  and MSG_TOS_REPLACE

  wmm_class
  priority  TOS   AC      DSCP
  7         224   AC_VO   56
  6         192   AC_VO   48
  5         160   AC_VI   40
  4         128   AC_VI   32
  3         96    AC_BE   24
  2         64    AC_BK   16
  1         32    AC_BK   8
  0         0     AC_BE   0
*/
typedef struct msg_tos {
   int m_type;
   int m_id;
   char * p_version;
   int m_size;

   int rule_id;
   char * intf_name;
   char * proto; // udp or tcp
   char * sip;
   char * sport;
   char * dip;
   char * dport;
   int wmm_class;  // priority = 0 to 7
} msg_tos;

void free_msg_send_tos(msg_tos ** m);

void process_msg_tos_add(char ** input, int input_len, char ** output, int * output_len);
void send_msg_tos_add(char * hostname, int portnum, int * id,
                      char * intf_name, char * proto,
                      char * sip, char * sport, char * dip, char * dport,
                      int wmm_class);


void process_msg_tos_replace(char ** input, int input_len, char ** output, int * output_len);
void send_msg_tos_replace(char * hostname, int portnum, int * id,
                          int rule_id, char * intf_name, char * proto,
                          char * sip, char * sport, char * dip, char * dport,
                          int wmm_class);

/**
  x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x

  sip: Source specification
  dip: Destination specification.

  the string can contain >>  [!] address[/mask]
  * address: can be either a network name, a hostname (not recommended), a network IP address (with /mask), or a plain IP address.
  * mask: can be either a network mask or a plain number (/network size)
  * "!" argument before the address specification inverts the sense of the address
  examples:
  a) sip = "192.168.0.1"                    packets that come from host 192.168.0.1 (any port)
  b) sip = "192.168.0.1/32"                 same as (a)
  c) sip = "192.168.0.1/255.255.255.255"    same as (a)
  d) sip = "! 192.168.0.1"                  packets that come from any host but 192.168.0.1

  x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x

  sport: Source port or port range specification
  dport: Destinantion port or port range specification
  the string can contain >> [!] port[:port]
  examples:
  a) sport = "5001"                    packets that come from port 5001 (any host)
  b) sport = "5001:5005"               packets that come from port 5001 to 5005 (any host)
  c) sport = "! 5001"                  packets that come from any port but 5001

  x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x

  proto: the protocol of the rule or of the packet to check.
         the specified protocol can be one of tcp, udp, icmp, or all

  the string can contain >> [!] protocol
 */
#endif