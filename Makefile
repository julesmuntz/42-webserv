CPPC= c++

CPPFLAGS= -Wall -Werror -Wextra -std=c++98 -g

SRCS= Server.cpp main.cpp

OBJS= $(addprefix objs/, $(SRCS:.cpp=.o))

DEPS= $(OBJS:.o=.d)

NAME= webserv

objs/%.o : %.cpp
	@mkdir -p objs
	$(CPPC) $(CPPFLAGS) -MMD -MP -c $< -o $@

all : $(NAME)

$(NAME) : $(OBJS)
	$(CPPC) $(CPPFLAGS) -o $(NAME) $(OBJS)

re :
	make fclean
	make all

clean :
	rm -f $(OBJS)
	rm -rf objs

fclean : clean
	rm -f $(NAME)

-include $(DEPS)

.PHONY : all clean fclean re
