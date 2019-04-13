# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: callen <callen@student.42.fr>              +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2018/09/23 22:04:17 by callen            #+#    #+#              #
#    Updated: 2019/04/12 17:55:44 by callen           ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME := minishell
DNAM := d_$(NAME)
ANAM := a_$(NAME)

CC ?= clang
CFLAGS := -Wall -Wextra -Werror
DFLAGS := -Wall -Wextra -g
AFLAGS := $(DFLAGS) -fsanitize=address

LIBDIR := libft
INCDIR := includes
OBJDIR := .obj
SRCDIR := srcs

INCFLAGS := -I$(INCDIR) -I$(LIBDIR)/$(INCDIR)
LIBFLAGS := -L$(LIBDIR) -lft
DEBGLIBS := $(LIBDIR)/d_libft.a
ASANLIBS := $(LIBDIR)/a_libft.a
FRAMWRKS :=

SRC := main.c

OBJ := $(addprefix $(OBJDIR)/, $(SRC:.c=.o))

NRM := $(shell which pynorme)
ifeq ($(NRM),)
	NRM = "$(shell \
	if [ -d ~/.usr_bin ] && [ -x ~/.usr_bin/norminette.py ];\
	then\
		echo ~/.usr_bin/norminette.py;\
	fi)"
endif
NORME := $(addsuffix *.h,$(INCDIR)/) $(addsuffix *.c,$(SRCDIR)/)

.PHONY: all debug clean dclean fclean re tags j k asan norme codesize

all: $(NAME)

$(NAME): $(OBJDIR) $(OBJ)
	make -C libft all
	@$(CC) $(INCFLAGS) $(LIBFLAGS) -o $(NAME) $(OBJ)
	@echo "Compiled $(NAME)"

j: debug

k: fclean dclean

asan:
	@make -sC libft asan
	@$(CC) $(AFLAGS) $(INCFLAGS) $(ASANLIBS) -o $(ANAM) $(addprefix $(SRCDIR)/, $(SRC))

debug:
	@make -sC libft debug
	@$(CC) $(DFLAGS) $(INCFLAGS) $(DEBGLIBS) -o $(DNAM) $(addprefix $(SRCDIR)/, $(SRC))

dclean:
	@make -C libft dclean
	rm -rf $(DNAM) $(DNAM).dSYM $(ANAM) $(ANAM).dSYM

clean:
	make -C libft clean
	rm -Rf $(OBJDIR)

fclean: clean
	make -sC libft fclean
	rm -f $(NAME)

re: fclean all

tags:
	ctags $(addsuffix *.h,$(INCDIR)/) $(addsuffix *.c,$(SRCDIR)/)

$(addprefix $(OBJDIR)/, %.o): $(addprefix $(SRCDIR)/, %.c)
	@$(CC) $(INCFLAGS) $(CFLAGS) -o $@ -c $<

$(OBJDIR):
	@mkdir -p $(OBJDIR)

norme:
	@$(NRM) $(NORME)

codesize:
	@printf "Lines of code: "
	@cat $(NORME) | grep -Ev '(^\/\*|^\*\*|^\*\/$$|^$$|\*\/)' | wc -l | cut -d' ' -f7
