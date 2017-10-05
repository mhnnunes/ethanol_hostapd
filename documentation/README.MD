# COMMUNICATION WITH ETHANOL #

To ensure communication between the ethanol controller and its agents we develop a secure communication channel between the two devices. We create a server application and a client application for each of the devices. In the controller, applications are developed in Python, so they are compatible with the Pox (OpenFlow) controller. On the client side, applications must be writter in C. All C programs are compiled and linked using **make** by appropriate Makefile, all dependencies should be checked by configure file.

We will use a Socket SSL protocol as the transport mechanism. The messages are developed by our team using python and C language. You will find the messages in C language in the [messaging](https://github.com/h3dema/ethanol_hostapd/tree/master/hostapd-2.6/src/messaging) directory, and the python ones are in [another repository in ssl_message](https://github.com/h3dema/ethanol_controller/tree/master/ethanol/ssl_message) directory.

The server is multithreaded to support multiple concurrent connections.

## Wish List ##

* The client and server applications should evolve so that the connection is initially open and remains open until a specific procedure is called to close the connection on the client (or timeout occurs on the server). Today each communication (message) opens a connection, transmits data, and closes the connection.
* A procedure for error handling should be provided in case of connection failure.
* A log file (in txt format) should be generated for all requests and responses in both applications. Today some messages are added to hostapd using wprintf() command.
