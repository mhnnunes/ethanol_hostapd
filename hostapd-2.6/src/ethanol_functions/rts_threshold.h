
/*
rts_threshold
RTS/CTS  adds  a  handshake before each packet transmission to make sure that the channel is clear. This adds overhead,
but increases performance in case of hidden nodes or a large number of active nodes. This parameter sets  the  size  of
the smallest packet for which the node sends RTS ; a value equal to the maximum packet size disables the mechanism. You
may also set this parameter to auto, fixed or off.
Examples :
     iwconfig eth0 rts 250
     iwconfig eth0 rts off
*/

//TODO: TUDO!!

void set_rts_threshold(char * intf_name, long new_rts);