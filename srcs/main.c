/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: callen <callen@student.42.us.org>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/04/12 14:56:58 by callen            #+#    #+#             */
/*   Updated: 2019/06/03 00:27:49 by callen           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "msh_strvec.h"
#include "libft.h"
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>

/*
** TODO: Things to fix
** - `cd' incorrectly updating $PWD if passed a relative path
** -
*/

#define IFNT else if

void		msh_panic(char *msg)
{
	ft_dprintf(2, "minishell: %s\n", msg);
	exit(1);
}

void		msh_usage(int ex, char *v)
{
	ft_dprintf(2, "Usage: %s [option] ...\n", v);
	ft_dprintf(2, "\t-d : increase debug output\n");
	ft_dprintf(2, "\t-h : display this help output\n");
	exit(ex);
}

int			round_to_pow2(int n)
{
	register int	i;

	i = 1;
	while (i <= n)
		i <<= 1;
	return (i);
}

#define INCSHLVL(v) (ft_strjoin_free("SHLVL=", ft_itoa(ft_atoi((v)+6)+1), 'R'))
#define COPYVAR(v) (ft_strnequ(v,"SHLVL=", 6) ? INCSHLVL(v) : ft_strdup(v))

static void	init_shenv(t_shenv *shenv, t_margs *mg)
{
	register int	i;
	int				l;

	shenv->m = mg;
	l = round_to_pow2(strvec_len(mg->e));
	shenv->envlst = strlist_new(l);
	i = -1;
	while (mg->e[++i] && i < shenv->envlst->list_size)
	{
		shenv->envlst->list[i] = COPYVAR(mg->e[i]);
		shenv->envlst->list_len++;
	}
	shenv->cmdv = NULL;
	shenv->sl = NULL;
	shenv->wid = 0;
	shenv->pwd_ex = 0;
	shenv->st = 0;
	shenv->dl = 0;
	shenv->cmdc = 0;
	shenv->ret = 0;
	shenv->home = NULL;
	shenv->path = NULL;
	shenv->exit_called = 0;
	shenv->prompt_printed = 0;
	shenv->signal_recv = 0;
}

#undef COPYVAR
#undef INCSHLVL

/*
** TODO for 21sh
** - parse command line input int a word list (t_wlst)
*/

int			main(int argc, char **argv, char **envp, char **aplv)
{
	static t_shenv	e;
	static t_margs	m;
	int				ch;

	signal(SIGINT, msh_sigint);
	msh_debug_print("main: start");
	g_dbg = 0;
	while ((ch = ft_getopt(argc, argv, "dh")) != -1)
	{
		if (ch == 'd')
			g_dbg++;
		else
			msh_usage(ch != 'h', *argv);
	}
	m.c = argc;
	m.v = argv;
	m.e = envp;
	m.a = aplv;
	init_shenv(&e, &m);
	g_shenv = &e;
	ch = msh_repl();
	msh_debug_print("main: end ch(%d)", ch);
	return (ch);
}
