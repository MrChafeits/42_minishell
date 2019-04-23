/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: callen <callen@student.42.us.org>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/04/12 14:56:58 by callen            #+#    #+#             */
/*   Updated: 2019/04/22 20:12:25 by callen           ###   ########.fr       */
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

int			env_builtin(t_margs *m)
{
	register int i;

	i = -1;
	while (m->e && m->e[++i])
		ft_printf("%s\n", m->e[i]);
	return (0);
}

void		exit_builtin(int c, char **v, int last)
{
	int		st;

	if (c > 1)
		st = ft_atoi(v[1]);
	else
		st = last;
	exit(st);
}

int			len_strtab(char **t)
{
	register int i;

	i = 0;
	while (t && t[i])
		++i;
	return (i);
}

char		**dup_strtab(char **t)
{
	register int	i;
	int				len;
	char			**ret;

	len = len_strtab(t);
	if (!(ret = malloc(sizeof(char*) * (len + 1))))
		return (NULL);
	i = -1;
	while (++i < len)
		if (!(ret[i] = ft_strdup(t[i])))
			return (NULL);
	return (ret);
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

int			msh_exec(char *ln, t_margs *m)
{
	static int	ret;
	int			c;
	char		**v;

	v = ft_strsplit(ln, ' ');
	c = len_strtab(v);
	if (ft_strequ("envdump", *v))
		ret = envdump(m);
	else if (ft_strequ("env", *v))
		ret = env_builtin(m);
	else if (ft_strequ("echo", *v))
		ret = echo_builtin_cmd(c, v);
	else if (ft_strequ("exit", *v))
		exit_builtin(c, v, ret);
	else //TODO: make this check path and exec appropriate binary
		ft_dprintf(2, "minishell: %s: Unknown command\n", *v);
	free_strtab(&v);
	return (ret);
}

int			msh_prompt(t_margs *m)
{
	int		b;
	int		status;
	char	*ln;

	status = 0;
	ln = NULL;
	while (1)
	{
		ft_printf("msh$ ");
		if ((b = get_next_line(0, &ln)) > 0)
		{
			status = msh_exec(ln, m);
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
	if ((ret = msh_prompt(&m)))
	{
		return (ret);
	}
	return (0);
}
