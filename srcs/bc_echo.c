/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bc_echo.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: callen <callen@student.42.us.org>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/04/22 16:48:43 by callen            #+#    #+#             */
/*   Updated: 2019/05/19 17:36:18 by callen           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "libft.h"

static int	echo_escapes(char **p, char *c)
{
	int		count;

	(*c) = *(*p)++;
	if ((*c) == 'a')
		(*c) = '\a';
	else if ((*c) == 'b')
		(*c) = '\b';
	else if ((*c) == 'c')
		return (1);
	else if ((*c) == 'e')
		(*c) = 033;
	else if ((*c) == 'f')
		(*c) = '\f';
	else if ((*c) == 'n')
		(*c) = '\n';
	else if ((*c) == 'r')
		(*c) = '\r';
	else if ((*c) == 't')
		(*c) = '\t';
	else if ((*c) == 'v')
		(*c) = '\v';
	else if ((*c) == '\\')
		(*c) = '\\';
	else if ((*c) == '0')
	{
		(*c) = 0;
		count = 3;
		while (--count >= 0 && (unsigned)(*(*p) - '0') < 8)
			(*c) = ((*c) << 3) + (*(*p)++ - '0');
	}
	else
		(*p)--;
	return (0);
}

int			echo_builtin_ash(int argc, char **argv)
{
	char	**ap;
	char	*p;
	char	c;
	int		nflag;
	int		eflag;

	nflag = 0;
	eflag = 0;
	ap = argv;
	if (argc)
		ap++;
	if ((p = *ap) != NULL)
	{
		if (ft_strequ(p, "-n"))
		{
			nflag = 1;
			ap++;
		}
		else if (ft_strequ(p, "-e"))
		{
			eflag = 1;
			ap++;
		}
	}
	while ((p = *ap++) != NULL)
	{
		while ((c = *p++) != '\0')
		{
			if (c == '\\' && eflag && echo_escapes(&p, &c))
				return (0);
			ft_putchar(c);
		}
		if (*ap)
			ft_putchar(' ');
	}
	if (!nflag)
		ft_putchar('\n');
	return (0);
}

int			echo_builtin_cmd(int argc, char **argv)
{
	char	**ap;
	char	*p;
	char	c;

	g_dbg ? ft_dprintf(2, "[DBG: echo_builtin_cmd: start]\n") : 0;
	ap = argv;
	if (argc)
		ap++;
	while ((p = *ap++) != NULL)
	{
		while ((c = *p++) != '\0')
			ft_putchar(c);
		if (*ap)
			ft_putchar(' ');
	}
	ft_putchar('\n');
	g_dbg ? ft_dprintf(2, "[DBG: echo_builtin_cmd: end]\n") : 0;
	return (0);
}

void		echo_builtin(t_shenv *e)
{
	int t;
	
	g_dbg ? ft_dprintf(2, "[DBG: echo_builtin: start]\n") : 0;
	if (e)
	{
		if (0)
			t = echo_builtin_cmd(e->cmdc, e->cmdv);
		else
			t = echo_builtin_ash(e->cmdc, e->cmdv);
	}
	e->ret = !e->ret ? t : e->ret;
	g_dbg ? ft_dprintf(2, "[DBG: echo_builtin: end]\n") : 0;
}
