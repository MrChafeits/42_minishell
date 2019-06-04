/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_path.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: callen <callen@student.42.us.org>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/06/02 14:59:43 by callen            #+#    #+#             */
/*   Updated: 2019/06/03 18:54:15 by callen           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "ft_stdio.h"
#include <signal.h>

static void	iter_path(t_shenv *e)
{
	register int	i;
	int				ex;
	char			*s;

	ex = 0;
	i = -1;
	if (e->path)
		while (e->path[++i])
		{
			s = ft_strjoin(e->path[i], "/");
			s = ft_strjoin_free(s, *e->cmdv, 'L');
			if (!access(s, F_OK) && access(s, X_OK) < 0)
			{
				ft_dprintf(2, "minishell: %s: Permission denied\n", s);
				free(s);
				exit(126);
			}
			ex = execve(s, e->cmdv, e->envlst->list);
			free(s);
		}
	else if (e->prompt_printed == 1)
		ft_dprintf(2, "minishell: %s: Unknown command\n", *e->cmdv);
	exit(ex);
}

int			msh_exec_path(t_shenv *e)
{
	char			*path;
	pid_t			pid;

	e->path_ex = 1;
	path = get_string_value("PATH");
	e->path = ft_strsplit(path, ':');
	free(path);
	pid = fork();
	signal(SIGINT, msh_sigint);
	if (!pid)
		iter_path(e);
	else if (pid < 0)
		ft_dprintf(2, "minishell: %s: Error forking\n", *e->cmdv);
	else
		e->wid = wait(&e->ret);
	strvec_dispose(e->path);
	if (e->signal_recv)
		e->ret += 128;
	e->signal_recv = 0;
	return (e->ret);
}
