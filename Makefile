# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: callen <callen@student.42.fr>              +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2018/09/23 22:04:17 by callen            #+#    #+#              #
#    Updated: 2019/05/22 22:42:46 by callen           ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME := minishell
DNAM := d_$(NAME)
ANAM := a_$(NAME)

CC ?= clang
# CC := /nfs/2018/c/callen/.brew/bin/gcc-9
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

SRC := main.c \
bc_cd.c \
bc_echo.c \
ft_strvec.c

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

.PHONY: all
all: $(NAME)

$(NAME): $(OBJDIR) $(OBJ)
	make -C libft all
	@$(CC) $(INCFLAGS) $(LIBFLAGS) -o $(NAME) $(OBJ)
	@echo "Compiled $(NAME)"

$(addprefix $(OBJDIR)/, %.o): $(addprefix $(SRCDIR)/, %.c)
	@$(CC) $(INCFLAGS) $(CFLAGS) -c -o $@ $<

$(OBJDIR):
	@mkdir -p $(OBJDIR)

.PHONY: clean
clean:
	make -C libft clean
	rm -Rf $(OBJDIR)

.PHONY: fclean
fclean: clean
	make -sC libft fclean
	rm -f $(NAME)

.PHONY: re
re: fclean all

.PHONY: f
f: asan

.PHONY: d
d: aclean
	rm -rf $(ASNDIR)

.PHONY: asan
asan: $(ASNDIR) $(ASN)
	@make -sC libft asan
	@$(CC) $(AFLAGS) $(INCFLAGS) $(ASANLIBS) -o $(ANAM) $(ASN)

$(addprefix $(ASNDIR)/, %.o): $(addprefix $(SRCDIR)/, %.c)
	$(CC) $(AFLAGS) $(INCFLAGS) -c -o $@ $<

$(ASNDIR):
	@mkdir -p $(ASNDIR)

.PHONY: aclean
aclean:
	rm -rf $(ANAM) $(ANAM).dSYM

.PHONY: j
j: debug

.PHONY: k
k: dclean
	rm -rf $(DBGDIR)

.PHONY: debug
debug: $(DBGDIR) $(DBG)
	@make -sC libft debug
	@$(CC) $(DFLAGS) $(INCFLAGS) $(DEBGLIBS) -o $(DNAM) $(addprefix $(SRCDIR)/, $(SRC))

$(addprefix $(DBGDIR)/, %.o): $(addprefix $(SRCDIR)/, %.c)
	$(CC) $(DFLAGS) $(INCFLAGS) -c -o $@ $<

$(DBGDIR):
	@mkdir -p $(DBGDIR)

.PHONY: dclean
dclean:
	@make -C libft dclean
	rm -rf $(DNAM) $(DNAM).dSYM

.PHONY: tags
tags:
	ctags -R .

.PHONY: norme
norme:
	@$(NRM) $(NORME)

.PHONY: codesize
codesize:
	@printf "Lines of code: "
	@cat $(NORME) | grep -Ev '(^\/\*|^\*\*|^\*\/$$|^$$|\*\/)' | wc -l | cut -d' ' -f7
