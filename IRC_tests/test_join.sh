#!/bin/bash
PORT=6667
PASSWORD=pass123
SERVER=./ircserv

$SERVER $PORT $PASSWORD &
PID=$!
sleep 1

{
  echo "PASS $PASSWORD"
  echo "NICK user1"
  echo "USER u1 0 * :User One"
  echo "JOIN #chan42"
  sleep 1
} | nc -C 127.0.0.1 $PORT > output_join.txt

cat output_join.txt
grep -q "JOIN :#chan42" output_join.txt && echo "✅ JOIN test passed" || echo "❌ JOIN test failed"
kill $PID
