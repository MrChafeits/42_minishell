# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: callen <callen@student.42.fr>              +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2018/09/23 22:04:17 by callen            #+#    #+#              #
#    Updated: 2019/05/13 15:41:47 by callen           ###   ########.fr        #
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
DBGDIR := .dbg
ASNDIR := .asn
SRCDIR := srcs

INCFLAGS := -I$(INCDIR) -I$(LIBDIR)/$(INCDIR)
LIBFLAGS := -L$(LIBDIR) -lft
DEBGLIBS := $(LIBDIR)/d_libft.a
ASANLIBS := $(LIBDIR)/a_libft.a
FRAMWRKS :=

SRC := main.c bc_echo.c

OBJ := $(addprefix $(OBJDIR)/, $(SRC:.c=.o))
DBG := $(addprefix $(DBGDIR)/, $(SRC:.c=.o))
ASN := $(addprefix $(ASNDIR)/, $(SRC:.c=.o))

NRM := $(shell which pynorme)
ifeq ($(NRM),)
	NRM = "$(shell \
	if [ -d ~/.usr_bin ] && [ -x ~/.usr_bin/norminette.py ];\
	then\
		echo ~/.usr_bin/norminette.py;\
	fi)"
endif
NORME := $(addsuffix *.h,$(INCDIR)/) $(addsuffix *.c,$(SRCDIR)/)

.PHONY: all debug j clean dclean k fclean re tags asan f aclean d norme codesize

all: $(NAME)

$(NAME): $(OBJDIR) $(OBJ)
	make -C libft all
	@$(CC) $(INCFLAGS) $(LIBFLAGS) -o $(NAME) $(OBJ)
	@echo "Compiled $(NAME)"

$(addprefix $(OBJDIR)/, %.o): $(addprefix $(SRCDIR)/, %.c)
	@$(CC) $(INCFLAGS) $(CFLAGS) -c -o $@ $<

$(OBJDIR):
	@mkdir -p $(OBJDIR)

clean:
	make -C libft clean
	rm -Rf $(OBJDIR)

fclean: clean
	make -sC libft fclean
	rm -f $(NAME)

re: fclean all

f: asan

d: aclean

asan: $(ASNDIR) $(ASN)
	@make -sC libft asan
	@$(CC) $(AFLAGS) $(INCFLAGS) $(ASANLIBS) -o $(ANAM) $(ASN)

$(addprefix $(ASNDIR)/, %.o): $(addprefix $(SRCDIR)/, %.c)
	$(CC) $(AFLAGS) $(INCFLAGS) -c -o $@ $<

$(ASNDIR):
	@mkdir -p $(ASNDIR)

aclean:
	@macke -C libft aclean
	rm -rf $(ANAM) $(ANAM).dSYM

j: debug

k: dclean

debug: $(DBGDIR) $(DBG)
	@make -sC libft debug
	@$(CC) $(DFLAGS) $(INCFLAGS) $(DEBGLIBS) -o $(DNAM) $(addprefix $(SRCDIR)/, $(SRC))

$(addprefix $(DBGDIR)/, %.o): $(addprefix $(SRCDIR)/, %.c)
	$(CC) $(DFLAGS) $(INCFLAGS) -c -o $@ $<

$(DBGDIR):
	@mkdir -p $(DBGDIR)

dclean:
	@make -C libft dclean
	rm -rf $(DNAM) $(DNAM).dSYM

tags:
	ctags $(addsuffix *.h,$(INCDIR)/) $(addsuffix *.c,$(SRCDIR)/)

norme:
	@$(NRM) $(NORME)

codesize:
	@printf "Lines of code: "
	@cat $(NORME) | grep -Ev '(^\/\*|^\*\*|^\*\/$$|^$$|\*\/)' | wc -l | cut -d' ' -f7
