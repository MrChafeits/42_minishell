/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_command.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: callen <callen@student.42.us.org>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/06/01 01:20:10 by callen            #+#    #+#             */
/*   Updated: 2019/06/01 01:20:11 by callen           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "ft_stdio.h"
#include <signal.h>

int			msh_exec_path(t_shenv *e)
{
	register int	i;
	int				ex;
	char			*s;
	pid_t			pid;

	e->path_ex = 1;
	ex = 0;
	e->path = ft_strsplit((e->home = get_string_value("PATH")), ':');
	pid = fork();
	signal(SIGINT, msh_sigint);
	if (!pid && (i = -1))
	{
		if (e->path)
			while (e->path && e->path[++i])
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
	else if (pid < 0)
		ft_dprintf(2, "minishell: %s: Error forking\n", *e->cmdv);
	else
		i = wait(&e->ret);
	free(e->home);
	strvec_dispose(e->path);
	e->signal_recv = 0;
	return (e->ret);
}

static t_bc	g_cmds[7] = {
	{ "cd", &cd_builtin },
	{ "echo", &echo_builtin },
	{ "env", &env_builtin },
	{ "exit", &exit_builtin },
	{ "setenv", &setenv_builtin },
	{ "unsetenv", &unsetenv_builtin },
};

int			msh_exec_builtin(t_shenv *e)
{
	register int	i;
	int				status;

	i = -1;
	status = 1;
	while (g_cmds[++i].cmd != 0)
	{
		if (ft_strequ(g_cmds[i].cmd, *e->cmdv))
		{
			g_cmds[i].f(e);
			status = 0;
			break ;
		}
	}
	return (status);
}

int			msh_exec_pwd(t_shenv *e)
{
	int		st;
	int		ex;
	pid_t	pid;
	pid_t	wid;

	pid = fork();
	signal(SIGINT, msh_sigint);
	if (!pid)
	{
		if ((ex = execve(*e->cmdv, e->cmdv, e->envlst->list)) == -1)
			exit(127);
		exit((e->pwd_ex = ex));
	}
	else if (pid < 0)
		return (!!ft_dprintf(2, "minishell: %s: Fatal error\n", *e->cmdv) - 2);
	else
	{
		wid = wait(&st);
		e->wid = wid;
		return ((e->st = st));
	}
	ft_dprintf(2, "minishell: %s: Unknown command\n", *e->cmdv);
	return (255);
}

#define CHKEP(x) (SHR8(x) != 127 && SHR8(x) != 255 && (x) != 11)
#define ISPWD(s) ((s)[0] == '.' && (s)[1] == '/' && (s)[2])
#define ISRELP(s) ((s)[0] == '.' && (s)[1] == '.' && (s)[2] == '/' && (s)[3])
#define ISABSP(s) ((s)[0] == '/' && (s)[1])

int			msh_exec(t_shenv *e)
{
	int	st_b;
	int	st_d;
	int	st_p;

	st_b = 0;
	st_d = 0;
	st_p = 0;
	msh_debug_print("exec: start cmdv[0](%s)", e->cmdv[0]);
	st_b = msh_exec_builtin(e);
	msh_debug_print("exec: builtin st_b(%d)", st_b);
	if (WIFEXITED(st_b))
		return (st_b);
	if (ISABSP(*e->cmdv) || ISPWD(*e->cmdv) || ISRELP(*e->cmdv))
	{
		msh_debug_print("exec: found /");
		if (!access(*e->cmdv, F_OK) && access(*e->cmdv, X_OK) < 0)
		{
			ft_dprintf(2, "minishell: %s: Permission denied\n", *e->cmdv);
			return (126);
		}
		st_d = msh_exec_pwd(e);
		msh_debug_print("exec: pwd st_d(%d)", st_d);
		if (WIFEXITED(st_d))
		{
			msh_debug_print("exec: pwd ret st_d(%d)", st_d);
			if (SHR8(st_d) == 127)
				ft_dprintf(2, "minishell: %s: Unknown command\n", *e->cmdv);
			return (st_d);
		}
	}
	st_p = msh_exec_path(e);
	msh_debug_print("exec: path st_b(%d) st_d(%d) st_p(%d)", st_b, st_d, st_p);
	if (CHKEP(st_p) && WIFEXITED(st_p))
	{
		msh_debug_print("exec: path ret st_p(%d)", st_p);
		return (st_p);
	}
	else if (st_b == 1 && !st_d && SHR8(st_p) == 255)
		ft_dprintf(2, "minishell: %s: Unknown command\n", *e->cmdv);
	else
		e->signal_recv = 0;
	return (127);
}
