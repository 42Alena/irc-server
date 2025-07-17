STEP 1: Understand the Goal
[]    Listen on a given port
[] 	 Handl multiple clients with one poll()
[]    Parses and responds to basic IRC commands
[]    Manages channels and permissions

# IRC 
The goal of this project is to build our own IRC (Internet Relay Chat) Server. User should be able to exchange private messages and join group channels. 


## Project requirements

- The server must handle multiple clients simultaneously without hanging. 
- Forking is forbidden, so all the I/O ops must be non-blocking
- Only 1 `()` can be used for handling all these operations (read, write, listen and so forth)
- Communication between client and server has to be done via TCP/IP (v4 or v6)
- You must be able to authenticate, set a nickname, a username, join a channel, send and receive private messages using your reference client
- All the messages sent from one client to a channel have to be forwarded to every other client that joined the channel
- You must have operators and regular users
- implement the commands that are specific to channel operators:
	- KICK: remove a client from the channel
	- INVITE: invite a client to a channel
	- TOPIC: change or view the channel topic
	- MODE: Change the channel's mode:
		- i: set/remove Invite-only channel
		- t: set/remove the restrictions of the TOPIC command to channel operators
		- k: set/remove the channel key (password)
		- o: give/take channel operator privilege
		- l: set/remove the user limit to channel


