#ifndef ___ROUTE_H__
#define ___ROUTE_H__


// net = address[/mask]
int add_default_route(char * intf_name, char * net);
int add_route(char * intf_name, char * net);

int del_default_route();
int del_route(char * intf_name, char * net);

#endif