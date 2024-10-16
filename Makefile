#******************** VARIABLES ********************#

#---------- NAMES ----------#

NAME		=		ft_ping

#---------- DIRECTORIES ----------#

SRC_DIR		=		src/
INC_DIR		=		include/
BUILD_DIR	=		.build/

#---------- SOURCES ----------#

SRC_FILES	=		main.c			\
					parser.c		\
					socket.c		\
					ping.c			\
					icmp.c			\
					print.c			\
					time.c			\
					rtt.c			\


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

#---------- DOCKER ----------#

.PHONY:				docker_build
docker_build:		$(NAME)
					docker build -t ping .

.PHONY:				docker_run
docker_run:			$(NAME)
					docker run --rm -it -v ./ft_ping:/ft_ping -it ping

#---------- EXECUTABLES ----------#

-include			$(DEPS)

$(NAME):			$(OBJ)
					$(CC) $(OBJ) -lm -o $@

#---------- OBJECTS FILES ----------#

$(BUILD_DIR)%.o:	%.c
					$(MKDIR) $(shell dirname $@)
					$(CC) $(C_FLAGS) $(D_FLAGS) $(I_FLAGS) -c $< -o $@
