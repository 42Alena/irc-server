#!/bin/bash

# start server with valgrind
valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes \
		 --log-file=valgrind_output.txt ./ircserv 6667 &

SERVER_PID=$!
sleep 2

# run basic connection tests using a netcat client and redirected input
nc localhost 6667 <<EOF
NICK testuser
USER testuser 0 * :Test User
QUIT
EOF

# kill server and check valgrind output
kill $SERVER_PID
wait $SERVER_PID

# analyze the output
if grep -q "no leaks are possible" valgrind.out; then
	echo "No memory leaks detected. 😎 😎 😎 😎 😎 😎 😎 😎 🇧 🇴 🇸 🇸"
else
	echo "Memory leaks detected!! 😱 😱 😱 😱 😱 😱 😱 "
	cat valgrind.out
	exit 1
fi

