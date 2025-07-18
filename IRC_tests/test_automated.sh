#!/bin/bash

# Configuration
SERVER_EXEC="./ircserv"  # Path to your IRC server executable
PORT=6667                # Port your server will listen on
PASSWORD="testpass"      # Password for the server
TMUX_SESSION="irc_test"  # Name of the tmux session
NUM_CLIENTS=3            # Number of irssi clients to spawn
SERVER_LOG="server.log"  # Log file for the server

set -ex

tmux kill-session -t "irc_test" 2>/dev/null || true

# Start the IRC server in a tmux session
tmux new-session -d -s $TMUX_SESSION -n server
tmux send-keys -t $TMUX_SESSION:server "$SERVER_EXEC $PORT $PASSWORD > $SERVER_LOG 2>&1" C-m
echo "IRC server started in tmux session '$TMUX_SESSION', window 'server'."

# Wait for the server to start
sleep 50

# Open tmux panes for irssi clients
for i in $(seq 1 $NUM_CLIENTS); do
    tmux split-window -t $TMUX_SESSION
    tmux select-layout -t $TMUX_SESSION tiled
    tmux send-keys -t $TMUX_SESSION "irssi" C-m
    sleep 1
    tmux send-keys -t $TMUX_SESSION "/connect -password $PASSWORD localhost $PORT" C-m
done

# Attach to the tmux session
tmux select-pane -t $TMUX_SESSION:server
tmux attach-session -t $TMUX_SESSION