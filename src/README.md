### General implementation advice

* Start by writing the in-memory version of the server. Use the test clients described in
section 7.1 during development.
* Then write the client, using your own server.
* Finally, write the disk version of the server. Your server design should be such that you
only have to change the part of the server that deals with the database.