#!/bin/bash

# List the variables that will be used in the script
SERVER_EXC="./ircserv"
PORT=6667
PASWWORD="MyPassw0rd"
REFERENCE_CLIENT="irssi"
LOG_FILE="test_log.txt"

# Function to check code compliance
check_code_compliance() {
	echo "Checking code compliance..." | tee -a $LOG_FILE

	#check poll() ocurrences
	POLL_COUNT=$(grep -r "poll(" src/ | wc -l)
	echo "Number of poll() occurrences: $POLL_COUNT" | tee -a $LOG_FILE
	if [ "$POLL_COUNT" -ne 1 ]; then
		echo "Error: poll() should be used exactly once." | tee -a $LOG_FILE
	fi

	# verify poll() usage
	grep -r "poll(" src/ | grep -E "accept|read|recv|write|send" > /dev/null
    if [ $? -ne 0 ]; then
        echo "Error: poll() must be called before accept, read/recv, write/send." | tee -a $LOG_FILE
    fi

	# check fcntl() usage
	FCNTL_COUNT=$(grep -r "fcntl(" src/ | wc -l)
	echo "Number of fcntl() occurrences: $FCNTL_COUNT" | tee -a $LOG_FILE
	FCNTL_VALID=$(grep -r "fcntl(" src/ | grep "F_SETFL, O_NONBLOCK" | wc - l)
	if [ "$FCNTL_COUNT" -ne "$FCNTL_VALID" ]; then
		echo "Error: fcntl() should be used with F_SETFL and O_NONBLOCK." | tee -a $LOG_FILE
	fi
}

# Function to start the IRC Server
start_server() {
	echo "Starting IRC server..." | tee -a $LOG_FILE
	$SERVER_EXC $PORT $PASSWORD &
	SERVER_PID=$!
	sleep 2
}

# Function to test the connections
test_connections() {
	echo "Testing connections..." | tee -a $LOG_FILE
	
	#test with nc
	echo "Testing connections with netcat..." | tee -a $LOG_FILE
	(echo "PASS $PASSWORD"; echo "NICK testUser"; echo "USER testUser 0 * :Test User"; echo "JOIN #test"; echo "PRIVMSG #test :Hello, world!") | nc -v -q 1 localhost $PORT
	#test with reference client
	echo "Testing connections with reference client ($REFERENCE_CLIENT)..." | tee -a $LOG_FILE
	echo "Please connect to the server using $REFERENCE_CLIENT and test manually."
}

# Function to test edge cases
test_edge_cases() {
	echo "Testing edge cases..." | tee -a $LOG_FILE
	#test partial commands
	echo "Testing partial commands..." | tee -a $LOG_FILE
    (echo "PASS $PASSWORD"; echo "NICK testUser"; echo "USER testUser 0 * :Test User"; echo "JOIN #test"; echo -n "PRIVMSG #test :Partial message") | nc -v -q 1 localhost $PORT

	# Kill a client unexpectedly
    echo "Testing unexpected client termination..." | tee -a $LOG_FILE
    CLIENT_PID=$(pgrep nc)
    kill -9 $CLIENT_PID

    # Stop a client (^Z) and flood the channel
    echo "Testing client suspension and flooding..." | tee -a $LOG_FILE
    CLIENT_PID=$(pgrep nc)
    kill -STOP $CLIENT_PID
    (echo "PRIVMSG #test :Flood message") | nc -v -q 1 localhost $PORT
    kill -CONT $CLIENT_PID
}

# Function to stop the server
stop_server() {
	echo "Stopping IRC server..." | tee -a $LOG_FILE
	kill -9 $SERVER_PID
}

#main execution
check_code_compliance
start_server
test_connections
test_edge_cases
stop_server

echo "Tests completed. Check $LOG_FILE for details."
