#chmod +x run_tests.sh
# ./run_tests.sh

#!/bin/bash
echo "🧪 Running all ft_irc tests with Valgrind..."

# Check if ircserv exists
if [ ! -x ../ircserv ]; then
  echo "❌ Error: ../ircserv not found or not executable"
  exit 1
fi

# Loop over all test scripts
for script in test_*.sh; do
  echo "▶️  $script"
  bash $script
  echo
done

