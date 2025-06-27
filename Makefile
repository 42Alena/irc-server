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


OBJECTS = $(SOURCES:.cpp=.o)

#_____LINKING RULE___________________________
$(NAME): $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $(NAME)  $(OBJECTS)

#____DEFAULT TARGET____________________
all: $(NAME)

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
runpp: fclean $(NAME)
	clear
	@echo "Starting server in background..."
	./$(NAME) 6667 myPassword & echo $$ > .server_pid
	sleep 5
	@echo "Connecting with netcat..."
	- nc -v localhost 6667; echo "nc exit code: $$?"
	
	@echo "Sending test IRC commands..."
	(echo "NICK testUser"; echo "USER testUser 0 * :Test User"; echo "JOIN #test"; sleep 2) | nc localhost 6667

	@echo "Running server for 5sec  ..."
	sleep 5
	@echo "Killing server..."
	- kill `cat .server_pid` 2>/dev/null || true
	@rm -f .server_pid

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