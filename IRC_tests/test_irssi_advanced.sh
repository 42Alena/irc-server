#!/bin/bash

# Configuration
SERVER_EXEC="./ircserv"  # Path to your IRC server executable
PORT=6667                # Port your server will listen on
PASSWORD="password123"   # Password for the server
TMUX_SESSION="irc_test"  # Name of the tmux session
NUM_CLIENTS=3            # Number of irssi clients to spawn
SERVER_LOG="server.log"  # Log file for the server

set -ex

# Create irssi configuration directory for testing
TEST_DIR="$HOME/.irssi_test"
mkdir -p "$TEST_DIR"

# Clean up previous test files
rm -f "$TEST_DIR"/irssi.log.*

# Clean up existing tmux sessions
tmux kill-session -t $TMUX_SESSION 2>/dev/null || true
tmux kill-session -t nc_test 2>/dev/null || true
for i in $(seq 1 $NUM_CLIENTS); do
    tmux kill-session -t "irc_test_$i" 2>/dev/null || true
done

# Start the IRC server in a tmux session
tmux new-session -d -s $TMUX_SESSION -n server
tmux send-keys -t $TMUX_SESSION:server "$SERVER_EXEC $PORT $PASSWORD > $SERVER_LOG 2>&1" C-m
sleep 10

# Check if the server is listening
if ! netstat -tuln | grep -q ":$PORT"; then
    echo "Error: Server is not listening on port $PORT"
    exit 1
fi
echo "Server is listening on port $PORT"

# Start netcat session to test partial commands
tmux new-session -d -s "nc_test" "nc localhost $PORT"
sleep 1
tmux send-keys -t nc_test "NICK test" Enter
tmux send-keys -t nc_test "USER testuser 0 * :Test User" Enter
tmux send-keys -t nc_test "PRIVMSG #test :Partial" C-m
sleep 1
tmux send-keys -t nc_test "Command" Enter

# Start irssi clients
for i in $(seq 1 $NUM_CLIENTS); do
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
            port = $PORT;
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
    tmux new-session -d -s "irc_test_$i" "irssi --config=$TEST_DIR/config$i --home=$TEST_DIR"
done

# Wait for connections to establish
sleep 10

# Debug checkpoint: List all tmux sessions
tmux list-sessions

# Test unexpected client disconnection
tmux send-keys -t "irc_test_1" "/msg #test Hello from user1" Enter
sleep 1
tmux kill-session -t "irc_test_1"
echo "Client 1 disconnected unexpectedly."

# Test server stability after client disconnection
tmux send-keys -t "irc_test_2" "/msg #test Hello from user2 after client1 disconnect" Enter
sleep 1

# Test unexpected nc disconnection with partial command
tmux send-keys -t nc_test "PRIVMSG #test :Partial message" C-m
sleep 1
tmux kill-session -t nc_test
echo "Netcat session disconnected unexpectedly."

# Test server stability after nc disconnection
tmux send-keys -t "irc_test_2" "/msg #test Hello from user2 after nc disconnect" Enter
sleep 1

# Test client suspension and flooding
tmux send-keys -t "irc_test_2" C-z
echo "Client 2 suspended."
sleep 1
for i in {1..10}; do
    tmux send-keys -t "irc_test_3" "/msg #test Flood message $i" Enter
    sleep 5
done
tmux send-keys -t "irc_test_2" "fg" Enter
echo "Client 2 resumed."

# Verify logs for all clients
for i in $(seq 1 $NUM_CLIENTS); do
    echo "Checking logs for user$i..."
    if ! grep -q "Flood message 10" "$TEST_DIR/irssi.log.$i"; then
        echo "Flood messages not found in log of user$i"
        exit 1
    fi
done

echo "All tests passed successfully."