# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: callen <callen@student.42.fr>              +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2018/09/23 22:04:17 by callen            #+#    #+#              #
#    Updated: 2019/06/03 15:59:31 by callen           ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME := minishell
DNAM := d_$(NAME)
ANAM := a_$(NAME)

CC := clang
# CC := /nfs/2018/c/callen/.brew/bin/gcc-9
CFLAGS := -Wall -Wextra -Werror
DFLAGS := -Wall -Wextra -g
AFLAGS := $(DFLAGS) -fsanitize=address

LIBDIR := libft/
INCDIR := includes/
OBJDIR := .obj/
DBGDIR := .dbg/
ASNDIR := .asn/
SRCDIR := srcs/

INCFLAGS := -I$(INCDIR) -I$(LIBDIR)$(INCDIR)
LIBFLAGS := -L$(LIBDIR) -lft
DEBGLIBS := $(LIBDIR)d_libft.a
ASANLIBS := $(LIBDIR)a_libft.a
FRAMWRKS :=

SRC := \
bc_cd.c \
bc_echo.c \
bc_env.c \
bc_exit.c \
bc_setenv.c \
bc_unsetenv.c \
debug_print.c \
exec_command.c \
exec_path.c \
expansion.c \
generic_list.c \
init_shell.c \
main.c \
prompt.c \
quote_str.c \
signal.c \
strlist_one.c \
strlist_three.c \
strlist_two.c \
strvec_conv.c \
strvec_one.c \
strvec_two.c \
tokenize.c \
variables.c \
varlen.c \
word_list_init.c

OBJ := $(addprefix $(OBJDIR), $(SRC:.c=.o))
DBG := $(addprefix $(DBGDIR), $(SRC:.c=.o))
ASN := $(addprefix $(ASNDIR), $(SRC:.c=.o))

NRM := $(shell which pynorme)

ifeq ($(NRM),)
	NRM = "$(shell \
	if [ -d ~/.usr_bin ] && [ -x ~/.usr_bin/norminette.py ];\
	then\
		echo ~/.usr_bin/norminette.py;\
	fi)"
endif

NORME := $(addsuffix *.h,$(INCDIR)) $(addsuffix *.c,$(SRCDIR))

.PHONY: all
all: $(NAME)

$(NAME): libs $(OBJDIR) $(OBJ)
	@$(CC) $(INCFLAGS) $(LIBFLAGS) -o $(NAME) $(OBJ)
	@echo "Compiled $(NAME)"

.PHONY: libs
libs:
	make -j4 -C libft all

$(addprefix $(OBJDIR), %.o): $(addprefix $(SRCDIR), %.c)
	@$(CC) $(INCFLAGS) $(CFLAGS) -c -o $@ $<

$(OBJDIR):
	@mkdir -p $(OBJDIR)

.PHONY: clean
clean:
	make -sC libft clean
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

.PHONY: asan
asan: asanlibs $(ASNDIR) $(ASN)
	@$(CC) $(AFLAGS) $(INCFLAGS) $(ASANLIBS) -o $(ANAM) $(ASN)

.PHONY: asanlibs
asanlibs:
	@make -j4 -C libft asan

$(addprefix $(ASNDIR), %.o): $(addprefix $(SRCDIR), %.c)
	$(CC) $(AFLAGS) $(INCFLAGS) -c -o $@ $<

$(ASNDIR):
	@mkdir -p $(ASNDIR)

.PHONY: aclean
aclean:
	make -sC libft aclean
	rm -rf $(ANAM) $(ANAM).dSYM
	rm -rf $(ASNDIR)

.PHONY: j
j: debug

.PHONY: k
k: dclean

.PHONY: debug
debug: debuglibs $(DBGDIR) $(DBG)
	@$(CC) $(DFLAGS) $(INCFLAGS) $(DEBGLIBS) -o $(DNAM) $(DBG)

.PHONY: debuglibs
debuglibs:
	@make -j4 -C libft debug

$(addprefix $(DBGDIR), %.o): $(addprefix $(SRCDIR), %.c)
	$(CC) $(DFLAGS) $(INCFLAGS) -c -o $@ $<

$(DBGDIR):
	@mkdir -p $(DBGDIR)

.PHONY: dclean
dclean:
	make -sC libft dclean
	rm -rf $(DBGDIR)
	rm -rf $(DNAM) $(DNAM).dSYM

tags:
	ctags -R .

TAGS:
	ctags -f TAGS -e -R .

.PHONY: norme
norme:
	@$(NRM) $(NORME)

.PHONY: codesize
codesize:
	@printf "Lines of code: "
	@cat $(NORME) | grep -Ev '(^\/\*|^\*\*|^\*\/$$|^$$|\*\/)' | wc -l
