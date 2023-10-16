NAME = webserv
INCLUDE_DIR = include
SRC_DIR = srcs
OBJ_DIR = obj
CC = g++
CFLAGS = -Wall -Wextra -Werror -std=c++98 -I$(INCLUDE_DIR) -g3
RM = rm -f

SRC_WEBSERV = main Exodus Server Server_utils RequestHandler RequestEndDeterminator RequestParser ResponseHTTP ResponseSender responseGET utils

SRC_FILES = $(SRC_WEBSERV)

SRC = $(addprefix $(SRC_DIR)/, $(addsuffix .cpp, $(SRC_FILES)))
OBJ = $(addprefix $(OBJ_DIR)/, $(addsuffix .o, $(SRC_FILES)))
DEP = $(OBJ:%.o=%.d)

NUM_FILES = $(words $(SRC_FILES))
CURRENT_FILE = 1

define show_progress
	@echo -n "\033[0;34m [$(CURRENT_FILE)/$(NUM_FILES)] \033[0m"
	@$(eval CURRENT_FILE=$(shell echo $$(($(CURRENT_FILE)+1))))
endef

all: $(NAME)

$(NAME): $(OBJ) $(INCLUDE)
	$(CC) $(CFLAGS) -o $(NAME) $(OBJ)
	@echo "\033[0;32m [OK] \033[0m       \033[0;33m Compiling:\033[0m" $(NAME)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(OBJ_DIR)
	@echo "\033[0;32m [OK] \033[0m" $<
	$(show_progress)
	@($(CC) $(CFLAGS) -MMD -c $< -o $@) 2> /dev/null || (echo "\033[0;31m [ERROR] \033[0m" $< && $(CC) $(CFLAGS) -MMD -c $< -o $@ && exit 1)

-include $(DEP)

clean:
	$(RM) -rf $(OBJ_DIR)

fclean: clean
	$(RM) -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
