/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: callen <callen@student.42.us.org>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/04/12 14:56:58 by callen            #+#    #+#             */
/*   Updated: 2019/04/20 19:56:38 by callen           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "libft.h"
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>

static int	envdump(t_margs *m)
{
	int	i;

	ft_printf("(int) m->c = %d\n", m->c);
	i = -1;
	ft_printf("(char **) m->v = %p\n", m->v);
	while (++i < m->c)
		ft_printf("(char *) m->v[%d] = %s\n", i, m->v[i]);
	i = -1;
	ft_printf("(char **) m->e = %p\n", m->e);
	while (m->e && m->e[++i])
		ft_printf("(char *) m->e[%d] = %s\n", i, m->e[i]);
	i = -1;
	ft_printf("(char **) m->a = %p\n", m->a);
	while (m->a && m->a[++i])
		ft_printf("(char *) m->a[%d] = %s\n", i, m->a[i]);
	return (0);
};

int			echo_builtin(int argc, char **argv)
{
	char	**ap;
	char	*p;
	char	c;
	int		count;
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
			if (c == '\\' && eflag)
			{
				switch (*p++) {
				case 'a': c = '\a'; break;
				case 'b': c = '\b'; break;
				case 'c': return (0);
				case 'e': c =  033; break;
				case 'f': c = '\f'; break;
				case 'n': c = '\n'; break;
				case 'r': c = '\r'; break;
				case 't': c = '\t'; break;
				case 'v': c = '\v'; break;
				case '\\': break;
				case '0':
					c = 0;
					count = 3;
					while (--count >= 0 && (unsigned)(*p - '0') < 8)
						c = (c << 3) + (*p++ - '0');
					break;
				default:
					p--;
					break;
				}
			}
			ft_putchar(c);
		}
		if (*ap)
			ft_putchar(' ');
	}
	if (!nflag)
		ft_putchar('\n');
	return (0);
}

int			len_strtab(char **t)
{
	register int i;

	i = 0;
	while (t && t[i])
		++i;
	return (i);
}

void		free_strtab(char ***t)
{
	register int i;

	if (t && *t && **t)
	{
		i = -1;
		while ((*t)[++i])
			ft_memdel((void**)&(*t)[i]);
		ft_memdel((void**)&(*t));
	}
}
int			ft_prompt(t_margs *m)
{
	int		b;
	int		status;
	char	*ln;
	int		c;
	char	**v;

	status = 0;
	ln = NULL;
	while (1)
	{
		ft_printf("msh$ ");
		if ((b = get_next_line(0, &ln)) > 0)
		{
			v = ft_strsplit(ln, ' ');
			c = len_strtab(v);
			if (ft_strequ("envdump", *v))
				status = envdump(m);
			else if (ft_strequ("echo", *v))
				status = echo_builtin(c, v);
			else if (ft_strequ("exit", *v))
				return (status);
			else
				ft_dprintf(2, "minishell: %s: Unknown command\n", *v);
			free_strtab(&v);
		}
		else if (!b)
			break ;
		else
			break ;
	}
	return (status);
}

int			main(int argc, char **argv, char **envp, char **aplv)
{
	int		ret;
	t_margs	m;

	m.c = argc;
	m.v = argv;
	m.e = envp;
	m.a = aplv;
	if ((ret = ft_prompt(&m)))
	{
		return (ret);
	}
	return (0);
}
