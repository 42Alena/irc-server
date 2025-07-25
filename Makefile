NAME     =  ircserv
CXX       =  c++
CXXFLAGS   =  -Wall -Wextra -Werror -std=c++98 
#-I./include
# Default linker flags (empty by default)
LDFLAGS =

#_____OBJECTS AND SOURCES______________________
SOURCES = \
	./main.cpp\
	./src/Server.cpp\
	./src/Client.cpp\
	./src/Channel.cpp\
	./src/replies.cpp\
	./src/utils.cpp\
	./commands/pingPong.cpp\
	./commands/privmsg.cpp\
	./commands/quit.cpp\
	./commands/registration.cpp\
	./commands/join.cpp\
	./commands/part.cpp\
	./commands/topic.cpp\
	./commands/mode.cpp\
	./commands/invite.cpp\
	./commands/kick.cpp\
	./commands/key.cpp\
	
	


OBJECTS = $(SOURCES:.cpp=.o)

#_____LINKING RULE___________________________
$(NAME): $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $(NAME)  $(OBJECTS)

#____DEFAULT TARGET____________________
all: $(NAME)
	@echo "\n\n\033[1;92m******************************************************************************\033[0m"
	@echo "\033[1;92m             To run IRC 🤖server:\033[0m \033[1;97m  ./ircserv <port> <password>\033[0m"
	@echo "\033[1;92m             Example:\033[0m               \033[1;97m./ircserv 6667 abc\033[0m"
	@echo "\033[1;92m******************************************************************************\033[0m \n\n"



#_____CLEAN/FCLEAN/RE________________
clean:
	$(RM) $(OBJECTS)

# ____REBUILD everyrhing________________
fclean: clean
	$(RM) $(NAME)

re:  fclean all

#_________BUILD+RUN programm with ./ircserv <port> <password>
# run: fclean  $(NAME)
# 	clear
# 	@echo " Missing args! Use: make runpp"
# 	-./$(NAME) 

#_________BUILD+RUN programm with ./ircserv <port> <password>
runpp:
	clear
	@echo "Killing existing server by PID if known..."
	- kill `cat .server_pid` 2>/dev/null || true
	@rm -f .server_pid

	@echo "Checking if port 6667 is still in use..."
	@if lsof -i :6667 >/dev/null 2>&1; then \
		echo "Port 6667 is busy, killing process by port..."; \
		kill -9 `lsof -t -i :6667` || true; \
		sleep 2; \
	fi

	@echo "Waiting for port 6667 to become available..."
	@while lsof -i :6667 >/dev/null 2>&1; do \
		echo "Port still busy, waiting..."; \
		sleep 1; \
	done

	@make fclean
	@make $(NAME)

	# clear
	@echo "Starting server in background..."
	@sh -c './$(NAME) 6667 myPassword & echo $$! > .server_pid'
	sleep 1

	@echo "Connecting with netcat and sending test IRC commands..."
	# '-q 1' makes nc exit 1 second after sending input
	(echo "PASS myPassword"; sleep 0.5; echo "NICK testUser"; sleep 0.5; echo "USER testUser 0 * :Test User"; sleep 0.5; echo "JOIN #test"; sleep 0.5) | nc -v -q 1 localhost 6667

	@echo "Running server for a few seconds..."
	sleep 2

	@echo "Killing server..."
	- kill `cat .server_pid` 2>/dev/null || true
	@rm -f .server_pid

	@echo "runpp finished."



#______DEBUG_VALGRIND_(LINUX only)______
vdebug: CXXFLAGS += -g -v
vdebug: $(NAME)
		valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./$(NAME)

#______DEBUG_FZANITAIZE_(LINUX+MAC)_______
fdebug: CXXFLAGS += -g -fsanitize=address -fno-omit-frame-pointer
fdebug: LDFLAGS := -fsanitize=address -static-libsan
fdebug: re
	./$(NAME)


.PHONY: all clean fclean re runpp fdebug vdebug