
/*
       frag[mentation_threshold]
Fragmentation allows to split an IP packet in a burst of smaller fragments transmitted on the  medium.  In  most  cases
this  adds  overhead,  but  in a very noisy environment this reduces the error penalty and allow packets to get through
interference bursts. This parameter sets the maximum fragment size which is always lower than the maximum packet size.
This parameter may also control Frame Bursting available on some  cards,  the  ability  to  send  multiple  IP  packets
together. This mechanism would be enabled if the fragment size is larger than the maximum packet size.
You may also set this parameter to auto, fixed or off.
Examples :
     iwconfig eth0 frag 512
     iwconfig eth0 frag off */


void set_fragmentation_threshold(char * intf_name, long new_frag);

int get_fragmentation_threshold(char * intf_name, long * frag);