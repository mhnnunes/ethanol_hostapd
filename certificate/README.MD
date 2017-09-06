This program uses certificates that you may generate.
We provide one as an example, but we recommend that you create your own.

Ethanol controller and the server service inside ethanol agent reads the certificate from mycert.pem.
 
## generate the certificate ##
 
It is relative easy to create a certificate.
You must install openssl libraries (listed in the main README).
Then you should issue the following command and fill out the fields:

```bash
$ openssl req -x509 -nodes -days 3650 -newkey rsa:1024 -keyout mycert.pem -out mycert.pem
```
