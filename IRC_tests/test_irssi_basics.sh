#!/bin/bash

# create irssi configuration directory for testing
TEST_DIR="$HOME/.irssi_test"
mkdir -p "$TEST_DIR"

# clean up previous test files
rm -f "$TEST_DIR"/irssi.log.*

# start irssi instances in separate terminals
for i in {1..3}; do
	#create custom configuration for each client
	cat > "$TEST_DIR/config$i" <<EOF
settings = {
	core = {
		real_name = "Test user $i";
		user_name = "user$i";
		nick = "user$i";
	};
	servers = (
		{
			address = "localhost";
			chatnet = "testnet";
			port = 6667;
			autoconnect = "yes"; 
		}
	);
	chats = {
		"#test" = {
			autojoin = "yes";
		};
	};
};
EOF

	# start irssi with logging and script execution
	tmux new-session -d -s "irc_test_$i" \
		irssi --config="$TEST_DIR/config$i" --home="$TEST_DIR" \
		--log-file="$TEST_DIR/irssi.log.$i" \
		--connect=localhost
done

#wait for connections to establish
sleep 5

# send commands to each client
for i in {1..3}; do
	tmux send-keys -t "irc_test_$i" \
		"/msg #test Hello from user$i" Enter
	sleep 1
done

#wait for messages to be processed
sleep 2

# verify messages were received by all clients
for i in {1..3}; do
	echo "Checking logs for user$i..."
	for j in {1..3}; do
		if ! grep -q "Hello from user$j" "$TEST_DIR/irssi.log.$i"; then
			echo "Message from user$i not found in log of user$j"
			exit 1
		fi
	done
done

# Clean up test directory
for i in {1..3}; do
	tmux send-keys -t "irc_test_$i" "/quit" Enter
	sleep 1
	tmux kill-session -t "irc_test_$i"
done

echo "All tests passed successfully."
exit 0