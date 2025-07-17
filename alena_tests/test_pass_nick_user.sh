#!/bin/bash
PORT=6667
PASSWORD=pass123
SERVER=../ircserv

# Start the server under valgrind in the background
valgrind --leak-check=full --log-file=valgrind_pass_nick_user.txt $SERVER $PORT $PASSWORD &
PID=$!
sleep 1

{
  echo "PASS $PASSWORD"
  echo "NICK user1"
  echo "USER u1 0 * :User One"
  sleep 1
} | nc -C 127.0.0.1 $PORT > output_pass_nick_user.txt

cat output_pass_nick_user.txt
grep -q ":user1!u1@.* 001" output_pass_nick_user.txt && echo "✅ PASS/NICK/USER test passed" || echo "❌ PASS/NICK/USER test failed"

# Wait for valgrind to flush
sleep 1
kill $PID

# Show valgrind summary
echo "🧠 Valgrind summary:"
grep -A10 "HEAP SUMMARY" valgrind_pass_nick_user.txt
