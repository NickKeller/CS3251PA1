Nick Keller
nkeller3@gatech.edu
902906955
Section B
Today's Date: Feb 11, 2015

Programming Assignment 1

Platforms: 
I wrote this code on both my tower, and my laptop, which run Ubuntu 14.04, and Ubuntu 13.04, respectively. I have not tested it on any other platform. It should work on other linux platforms, but I know for a fact that it will run if it is run on those 2 Ubuntu instances

TO COMPILE:

-naming convention goes like: protocol-role.c
-protocol is either tcp or udp
-role is either client or server
*********
-Server code for a specific protocol is located under protocol/server/
-Client code for a specific protocol is located under protocol/
-There are 4 Makefiles, all located with the C code that they compile
	-ex:-udp server code is under udp/server/
		-udp client code is under udp/
		-udp client makefile is under udp/
		-udp server makefile is under udp/server/

All you have to do run "make" under both protocol/ and protocol/server/ to generate the executeables. There will be some warnings, but that is okay. They don't cause any problems

To run the program:

SERVER: ./server-protocol [-d] port
CLIENT: ./client-protocol [-d] ipAddress[:port] username password
-If you get it wrong, it will tell you that you got it wrong
-ipAddress can be an actual ip address, or it can be a host name

Available usernames and passwords
user1 - pass1
user2 - pass2
user123 - pass3

DESCRIPTION OF PROTOCOL EXCHANGE:

*****YOU WON'T NEED MORE THAN 100 BYTES PER PACKET*****
-The server treats the \n character as the end of the message. It will ignore anything after it
-If you fail to include a \n character in the first 100 bytes, your message will be ignored

An example exchange looks like this:

C: REQ: Please Connect\n
S: CHA: challengeString\n
C: RES: username hash\n
S: ACK: Welcome to our Service\n

Stages of Protocol

Connection Request
-This is nice and simple. Just send the exact text "REQ: Please Connect\n", and you're good to go. 
-You have to have the "Please Connect" part. The server looks for it
-You also have to have the : and the space. The server splits the message on the space character, and it indexes as if the colon was there. If it is not, your message will be dropped.

Challenge and Response
-If a valid Connection request comes through, the server will generate a random 64 character string, and return it in this format:
CHA: 64CharacterChallengeString\n

-Your job: take the challenge string and contatenate it in this format:
username64CharacterChallengeStringPassword
-Once you concat it, run it through your md5 hash function. Specifically, the server uses the code found here: http://rosettacode.org/wiki/MD5#C
-Once your md5 function returns, send the response back in this format:
RES: username md5Result
-You must have a space between username and md5Result
-Like in the connection request, you must have "RES: " be exactly the first thing in your message

ACK: or NAK:
Depending on the username and md5Result that you send back, the server will respond with either a ACK or a NAK, and will look as follows

ACK: Welcome to our service\n
NAK: Wrong Credentials\n


Known Weirdness - Not really a bug
-For some reason, the MD5 function on the server side, when given the same string as the client used, would generate the same thing that the client generated, plus 3 extra bytes tacked on at the end. This happened even when given a static challenge string. So, on the server side, I just ignore those last 3 bytes. 
****THE CLIENT AND THE SERVER RAN THE EXACT SAME MD5 CODE, AND THAT FUNCTION IS CALLED THE EXACT SAME WAY ON BOTH THE CLIENT AND THE SERVER****
	-like I literally copied and pasted the code between both the client and the server
	-if you run a diff on the two md5.c files, the only thing that is different is the local header file name that is included
