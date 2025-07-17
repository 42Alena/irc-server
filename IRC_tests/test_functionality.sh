#!/bin/bash

# Start the IRC server
./ircserver 6667 &
SERVER_PID=$!
sleep 2

# test the client creation and basic commands
test_basics() {
	echo "Testing basic IRC functionality..."
	nc localhost 6667 <<EOF > response.txt &
NICK user1
USER user1 0* :User One
JOIN #testchannel
PRIVMSG #testchannel :Hello, World!
NICK user2
USER user2 0* :User Two
JOIN #testchannel
PRIVMSG #testchannel :Hi there!
PRIVMSG user1 :How are you?
QUIT
EOF

	wait
	sleep 1

	if grep -q "Hello, World!" response.txt && grep -q "Hi there!" response.txt; then
		echo "Basic functionality test passed."
	else
		echo "Basic functionality test failed."
		exit 1
	fi

	rm response.txt
}

# test the channel operations
test_channel_operations(){
	echo "Testing channel operations..."
	#1st client (operator)
	nc localhost 6667 <<EOF  &
NICK op1
USER op1 0* :Operator One
JOIN #moderatedchannel
MODE #moderated +o op1
MODE #moderated +t
TOPIC #moderated :This is a moderated channel testing topic setting
PRIVMSG #moderated :Welcome to the moderated channel!
PART #moderated :Leaving the channel
KICK #moderated user3 :Bad behaviour
QUIT
EOF

	#2nd client (regular user)
	nc localhost 6667 <<EOF &
NICK user3
USER user3 0* :User Three
JOIN #moderatedchannel
QUIT
EOF
}

# run all test
test_basics
test_channel_operations

# Cleanup
kill $SERVER_PID
wait $SERVER_PID
