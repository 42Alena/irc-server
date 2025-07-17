#!/bin/bash

test_channel_message() {
	echo "Testing channel message functionality..."
	#user 1 joins and send message
	nc localhost 6667 > user1.log <<EOF &
NICK user1
USER user1 0* :User One
JOIN #test
PRIVMSG #test :Hello everybody!!
PRIVMSG #test :How are you doing?
QUIT
EOF

#user 2 joins and should receive the message
	nc localhost 6667 > user2.log <<EOF &
NICK user2
USER user2 0* :User Two
JOIN #test
QUIT
EOF

	sleep 1
	# check if user2 received messages
	if ! grep -q "Hello everybody!!" user2.log || ! grep -q "How are you doing?" user2.log; then
		echo "Channel message test failed."
		return 1
	echo "Channel message test passed."
	return 0
}