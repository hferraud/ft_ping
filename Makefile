#******************** VARIABLES ********************#

#---------- NAMES ----------#

NAME		=		ft_ping

#---------- DIRECTORIES ----------#

SRC_DIR		=		src/
INC_DIR		=		include/
BUILD_DIR	=		.build/

#---------- SOURCES ----------#

SRC_FILES	=		main.c			\
					parser/parser.c	\

#---------- BUILD ----------#

SRC			=		$(addprefix $(SRC_DIR), $(SRC_FILES))
OBJ			=		$(addprefix $(BUILD_DIR), $(SRC:.c=.o))
DEPS		=		$(addprefix $(BUILD_DIR), $(SRC:.c=.d))

#---------- COMPILATION ----------#

C_FLAGS		=		-Wall -Werror -Wextra

I_FLAGS		=		-I$(INC_DIR)

D_FLAGS		=		-MMD -MP

#---------- COMMANDS ----------#

RM			=		rm -rf
MKDIR		=		mkdir -p

#******************** RULES ********************#

#---------- GENERAL ----------#

.PHONY:				all
all:				$(NAME)

.PHONY:				clean
clean:
					$(RM) $(BUILD_DIR)

.PHONY:				fclean
fclean:				clean
					$(RM) $(NAME) $(TEST) $(TEST_BONUS)

.PHONY:				re
re:					fclean
					$(MAKE)

#---------- RUN ----------#

.PHONY:				run
run:				$(TEST)
					./$(TEST)

.PHONY:				leak
leak:				$(TEST)
					valgrind --leak-check=full ./$(TEST)

#---------- EXECUTABLES ----------#

-include			$(DEPS)

$(NAME):			$(OBJ)
					$(CC) $(OBJ) -o $@

#---------- OBJECTS FILES ----------#

$(BUILD_DIR)%.o:	%.c
					$(MKDIR) $(shell dirname $@)
					$(CC) $(C_FLAGS) $(D_FLAGS) $(I_FLAGS) -c $< -o $@
