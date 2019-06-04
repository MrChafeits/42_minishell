/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: callen <callen@student.42.us.org>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/04/12 14:56:58 by callen            #+#    #+#             */
/*   Updated: 2019/06/03 17:34:14 by callen           ###   ########.fr       */
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
** - isatty(STDOUT_FILENO) to prevent prompt output
*/

#define IFNT else if

void		msh_panic(char *msg)
{
	ft_dprintf(2, "minishell: PANIC: %s\n", msg);
	exit(1);
}

static void	msh_usage(int ex, char *v)
{
	ft_dprintf(2, "Usage: %s [option] ...\n", v);
	ft_dprintf(2, "\t-d : increase debug output\n");
	ft_dprintf(2, "\t-h : display this help output\n");
	exit(ex);
}

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
