# Ethanol's configuration#

This directory contains the source code used to read ethanol's configuration file.
This file must be named **ethanol.ini** and placed in the /etc directory.


# ethanol.ini - File layout #

";" are inline comments. All contents after this symbol til the newline are considered as comments.

## section [protocol] ##

Set **enabled** equal to 0 to disabled all ethanol features in hostapd.
If parameter **enabled** is equal to 1, when you start the modified version of hostapd, the ethanol modifications will run.
The first step in the ethanol startup is to send a HELLO message to the controller.
You need to set properly the parameters **address** and **port_remote** to define the controller's socket address (IP + tcp port).

Ethanol's hostapd also can receive messages from any controller that has the same SSL certificate.
You can set the local TCP port using **port_local**. The default value is 22223.

## section [log] ##

This section defined where ethanol will write its logs.

logfile=/tmp/ethanol.log
