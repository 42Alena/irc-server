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
        "irssi --config='$TEST_DIR/config$i' --home='$TEST_DIR'"
    if [ $? -ne 0 ]; then
        echo "Failed to create tmux session for irc_test_$i"
        exit 1
    fi
done

#wait for connections to establish
sleep 10

# Debug check: list all tmux sessions
tmux list-sessions

# Debug check: verify log files are created
for i in {1..3}; do
    if [ ! -f "$TEST_DIR/irssi.log.$i" ]; then
        echo "Log file for user$i not created"
        exit 1
    fi
done

# Operator sets channel modes and sends messages
tmux send-keys -t "irc_test_1" \
    "/join #test" Enter \
    "/mode #test +t" Enter \
    "/mode #test +o user1" Enter \
    "/mode #test +i" Enter \
    "/topic #test :This is a test topic set by the operator" Enter \
    "/msg #test Operator has set the channel to invite-only and added a topic." Enter
sleep 5

# User2 tries to join the channel (should fail due to +i mode)
tmux send-keys -t "irc_test_2" \
    "/join #test" Enter
sleep 5

# Operator invites User2 to the channel
tmux send-keys -t "irc_test_1" \
    "/invite user2 #test" Enter
sleep 5

# User2 tries to join again (should succeed now)
tmux send-keys -t "irc_test_2" \
    "/join #test" Enter
sleep 5

# User3 tries to join the channel (should fail due to +i mode)
tmux send-keys -t "irc_test_3" \
    "/join #test" Enter
sleep 5

# Operator sends a message to the channel
tmux send-keys -t "irc_test_1" \
    "/msg #test Welcome to the channel!" Enter
sleep 5

# Verify logs for operator actions and user messages
for i in {1..3}; do
    echo "Checking logs for user$i..."
    if ! grep -q "Operator has set the channel to invite-only and added a topic." "$TEST_DIR/irssi.log.$i"; then
        echo "Operator message not found in log of user$i"
        exit 1
    fi
    if [ "$i" -eq 2 ] && ! grep -q "user2.*has joined #test" "$TEST_DIR/irssi.log.$i"; then
        echo "User2 join message not found in log of user$i"
        exit 1
    fi
    if [ "$i" -eq 3 ] && grep -q "user3.*has joined #test" "$TEST_DIR/irssi.log.$i"; then
        echo "User3 should not have been able to join the channel"
        exit 1
    fi
done

# Clean up test directory
for i in {1..3}; do
    tmux send-keys -t "irc_test_$i" "/quit" Enter
    sleep 1
    tmux kill-session -t "irc_test_$i"
done

echo "All tests passed successfully."
exit 0