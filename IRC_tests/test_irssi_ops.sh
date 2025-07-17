#!/bin/bash

TEST_DIR="$HOME/.irssi_test"
mkdir -p "$TEST_DIR"

# clean up previous test files
rm -f "$TEST_DIR"/ops.log.*

# create operator client configuration
cat > "$TEST_DIR/op_config" <<EOF
settings = {
  core = {
    real_name = "Operator";
    user_name = "operator";
    nick = "op1";
  };
  servers = (
    {
      address = "localhost";
      chatnet = "testnet";
      port = "6667";
      autoconnect = "yes";
      password = "operatorpass";
    }
  );
  chats = {
    "#moderated" = {
      autojoin = "yes";
    };
  };
};
EOF

# create regular user client configuration
cat > "$TEST_DIR/user_config" <<EOF
settings = {
  core = {
    real_name = "Regular User";
    user_name = "reguser";
    nick = "reg1";
  };
  servers = (
    {
      address = "localhost";
      chatnet = "testnet";
      port = "6667";
      autoconnect = "yes";
    }
  );
};
EOF

# start clients 
tmux new-session -d -s "irc_op" \
  irssi --config="$TEST_DIR/op_config" --home="$TEST_DIR" \
  --log-file="$TEST_DIR/ops.log.op" --connect=localhost

tmux new-session -d -s "irc_user" \
  irssi --config="$TEST_DIR/user_config" --home="$TEST_DIR" \
  --log-file="$TEST_DIR/ops.log.user" --connect=localhost

sleep 5

# operatos sets channel modes
tmux send-keys -t "irc_op" \
  	"/join #moderated" Enter \
  	"/mode #moderated +t" Enter \
	"/mode #moderated +o op1" Enter \
	"/mode #moderated +i" Enter \
  	"/topic #moderated :This is a moderated channel testing topic setting" Enter \
  	"/msg #moderated Welcome to the moderated channel!" Enter
sleep 2

# user tries to join (should fail due to +i)
tmux send-keys -t "irc_user" \
	"join #moderated" Enter
sleep 2

# operator invites user to the channel
tmux send-keys -t "irc_op" \
	"/invite reg1 #moderated" Enter
sleep 2

# User tries again (should work now)
tmux send-keys -t "irc_user" \
    "/join #moderated" Enter
sleep 1

# Operator sets topic
tmux send-keys -t "irc_op" \
    "/topic #moderated This is a test topic" Enter
sleep 1

# Verify results
if ! grep -q "Cannot join #moderated (invite only)" "$TEST_DIR/ops.log.user"; then
    echo "ERROR: Invite-only mode not working"
    exit 1
fi

if ! grep -q "reg1.*has joined #moderated" "$TEST_DIR/ops.log.op"; then
    echo "ERROR: Invite system not working"
    exit 1
fi

if ! grep -q "Topic for #moderated" "$TEST_DIR/ops.log.user"; then
    echo "ERROR: Topic not propagated"
    exit 1
fi

# Clean up
tmux send-keys -t "irc_op" "/quit" Enter
tmux send-keys -t "irc_user" "/quit" Enter
sleep 1
tmux kill-session -t "irc_op"
tmux kill-session -t "irc_user"

echo "Operator tests passed successfully!"
exit 0