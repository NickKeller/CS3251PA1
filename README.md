S 3251 - Computer Networks I
 
Sockets Programming Assignment 1 
 
 
TCP and UDP Applications Programming
 
For this assignment you will design and write your own application program using network sockets. You will implement two different versions of the application. One version will be based on TCP. The other will use UDP. You will implement both the client and server portions of this application. You may choose Java, Python or C/C++ for your implementation.
 
The application you are designing is a basic online authentication service. The service is based on a shared secret "password" and a challenge response protocol. The client interface will allow users to provide a username and password that will be used to determine whether or not they are granted access. The client command should be called "letmein" and it should read the username and password from the command line. The command should print either "Welcome to our service." or "User authorization failed." as output.
 
You must allow the host name (or IP address) and port number of the server to be specified on the command line of the client in addition to the username and password. 
(Of course, you would never implement a real security system that specifies username and password on the command line. In such cases another user running the "ps" command would quickly learn your password!) 
 
For instance, the command:
letmein 127.0.0.1:8591 “username” “password”
might produce the output:
Welcome to our service.
 
This access query would have been sent to the server application running on the local host (same system), listening on port 8591. The only output is the resulting message or an error message if it fails. Note: The command line must work exactly this way to make sure the TA can easily test your program.
 
Your server should support at least three different users. You can keep this simple and use a static list of usernames and passwords. When you start the server, you should specify the port number on which it will be listening.
 
letmein-server 8591
 
This would start the server on port 8591. Normally, the server should run forever, processing multiple clients. 
 
Challenge Response Algorithm
 
The access algorithm you are implementing is an extremely simplistic form of the Digest Authentication scheme widely used by web servers and many other services including VoIP. You should start by reading about these in Chapter 8 of your text and also in the Wikipedia pages on Digest Authentication and MD5 Hash Function.
Your access application will require the exchange of four messages.
Client sends "authentication request" message.
Server responds with a one time use, challenge value in the form of a random 64 character string. (You get to decide how this random string is generated.)
Client computes a MD5 hash of the string formed by concatenating the username, password and  the random string sent by the server. Hash = MD5("username","password","challenge")
Client sends the clear text "username" and the resulting "Hash" to the server.
The server takes the username, finds the corresponding password on file, and performs the same MD5 calculation. It then compares the calculated Hash to the one sent by the client. 
If they match, a message indicating success is sent back to the client. If they don't match, a message indicating failure is sent.
 
You will need to develop your own "protocol" for the communication between the client and the server. While you should use TCP or UDP to transfer messages back and forth, you must determine exactly what messages will be sent, what they mean and when they will be sent (syntax, semantics and timing). Be sure to document your protocol completely in the program writeup.
 
Your server application should listen for requests from access clients, process the client and return the result. After handling a client the server should then listen for more requests. The server application should be designed so that it will never exit as a result of a client user action. You should also be prepared for multiple requests arriving from different clients at the same time.
 
Your server will "store" the password along with the username. I'm asking you to keep it simple. Use a simple, in memory record format.
 
I'm asking you to implement the MD5 hash. However, you don't have to write the code. There are numerous versions already implemented that you should use. For instance, the RFC has one. You can also find example code here. However you choose to implement it, you must include a citation in your source code indicating where your MD5 came from. Note that you don't have to fully understand the MD5 algorithms to be able to use this in your program! You just need a function that will calculate the hash from your string.
 
Focus on limited functionality that is fully implemented. Practice defensive programming as you parse the data arriving from the other end. Again, don't focus on a powerful database or a fancy GUI, focus on the protocol and data exchange and make sure that you deal gracefully with whatever you or the TAs might throw at it.
