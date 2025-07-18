#!/bin/bash

set -ex

# create irssi configuration directory for testing
TEST_DIR="$HOME/.irssi_test"
mkdir -p "$TEST_DIR"

# clean up previous test files
rm -f "$TEST_DIR"/irssi.log.*

# clean up existing tmux sessions
for i in {1..3}; do
	tmux kill-session -t "irc_test_$i" 2>/dev/null || true
done

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
	log = {
		"#test" = {
			target = "$TEST_DIR/irssi.log.$i";
			level = "all";
		};
	};
};
EOF

	# start irssi with logging and script execution
	tmux new-session -d -s "irc_test_$i" \
		"irssi --config="$TEST_DIR/config$i" --home=$TEST_DIR"
done

#wait for connections to establish
sleep 15

# Debug check: list all tmux sessions
tmux list-sessions

# send commands to each client
for i in {1..3}; do
	tmux send-keys -t "irc_test_$i" \
		"/msg #test Hello from user$i" Enter
	sleep 1
done

#wait for messages to be processed
sleep 10

# verify messages were received by all clients
for i in {1..3}; do
	echo "Checking logs for user$i..."
	for j in {1..3}; do
		if ! grep -q "Hello from user$j" "$TEST_DIR/irssi.log.$i"; then
			echo "Message from user$j not found in log of user$i"
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