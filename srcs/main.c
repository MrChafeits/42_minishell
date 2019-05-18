/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: callen <callen@student.42.us.org>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/04/12 14:56:58 by callen            #+#    #+#             */
/*   Updated: 2019/05/17 20:07:40 by callen           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "msh_strvec.h"
#include "libft.h"
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>

int		g_dbg;
t_shenv	*g_shenv;

void		envdump(t_shenv *e)
{
	int	i;

	ft_printf("m->c = %d\n", e->m->c);
	i = -1;
	ft_printf("m->v = %p\n", e->m->v);
	while (++i < e->m->c)
		ft_printf("m->v[%d] = %s\n", i, e->m->v[i]);
	i = -1;
	ft_printf("m->e = %p\n", e->m->e);
	while (e->m->e && e->m->e[++i])
		ft_printf("m->e[%d] = %s\n", i, e->m->e[i]);
	i = -1;
	ft_printf("m->a = %p\n", e->m->a);
	while (e->m->a && e->m->a[++i])
		ft_printf("m->a[%d] = %s\n", i, e->m->a[i]);
};

void		env_builtin(t_shenv *e)
{
	register int i;

	i = -1;
	while (e->m->e && e->m->e[++i])
		ft_printf("%s\n", e->m->e[i]);
}

void		exit_builtin(t_shenv *e)
{
	int		st;

	e->exit_called = 1;
	if (e->cmdc > 1)
		st = ft_atoi(e->cmdv[1]);
	else
		st = e->ret;
	ft_dprintf(2, "exit\n");
	exit(st);
}

void		cd_builtin(t_shenv *e)
{
	ft_dprintf(2, "minishell: %s: TODO\n", *e->cmdv);
}

void		setenv_builtin(t_shenv *e)
{
	ft_dprintf(2, "minishell: %s: TODO\n", *e->cmdv);
}

void		unsetenv_builtin(t_shenv *e)
{
	ft_dprintf(2, "minishell: %s: TODO\n", *e->cmdv);
}

static t_bc	g_cmds[7] = {
	{ "cd", &cd_builtin },
	{ "echo", &echo_builtin },
	{ "env", &env_builtin },
	{ "exit", &exit_builtin },
	{ "setenv", &setenv_builtin },
	{ "unsetenv", &unsetenv_builtin },
};

void		msh_sigint_sub(int sig)
{
	g_dbg ? ft_dprintf(2, "DEBUG: msh_sigint_sub() sig = %d\n", sig) : 0;
	if (sig == SIGINT)
	{
		write(1, "\n", 1);
		signal(SIGINT, msh_sigint);
	}
}

void		msh_sigint(int sig)
{
	g_dbg ? ft_dprintf(2, "DEBUG: msh_sigint() sig = %d\n", sig) : 0;
	if (sig == SIGINT)
	{
		write(1, "\n", 1);
		msh_print_prompt();
		signal(SIGINT, msh_sigint);
	}
}

int			msh_exec_path(t_shenv *e)
{
	register int	i;
	char			*s;
	pid_t			pid;

	g_dbg ? ft_dprintf(2, "[DBG: msh_exec_path: cmdv = %s]\n", *e->cmdv) : 0;
	pid = fork();
	signal(SIGINT, msh_sigint);
	if (!pid && (i = -1))
	{
		while (e->path[++i])
		{
			s = ft_strjoin(e->path[i], "/");
			s = ft_strjoin_free(s, *e->cmdv, 'L');
			g_dbg ? ft_dprintf(2, "[DBG: msh_exec_path: s = \"%s\"]\n", s) : 0;
			if (!access(s, F_OK) && access(s, X_OK) < 0)
			{
				ft_dprintf(2, "minishell: %s: Permission denied\n", s);
				exit(126);
			}
			execve(s, e->cmdv, e->m->e);
			ft_memdel((void**)&s);
		}
		exit(1);
	}
	else
		wait(&e->ret);
	return (e->ret);
}

int			msh_exec_builtin(t_shenv *e)
{
	register int	i;
	int				status;

	i = -1;
	status = 0;
	g_dbg ? ft_dprintf(2, "[DBG: builtin: cmdv = %s]\n", *e->cmdv) : 0;
	while (g_cmds[++i].cmd != 0)
	{
		if (ft_strequ(g_cmds[i].cmd, *e->cmdv))
		{
			g_dbg ? ft_dprintf(2,
			"[DBG: builtin: g_cmds[%d].cmd = %s]\n", i, g_cmds[i].cmd) : 0;
			g_cmds[i].f(e);
			status = 1;
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

	g_dbg ? ft_dprintf(2, "[DBG: msh_exec_pwd: cmdv = %s]\n", *e->cmdv) : 0;
	pid = fork();
	signal(SIGINT, msh_sigint);
	if (!pid)
	{
		if ((ex = execve(*e->cmdv, e->cmdv, e->m->e)) == -1)
			exit(127);
		// ft_dprintf(2, "minishell: %s: Child process error\n", *e->cmdv);
		g_dbg ? ft_dprintf(2, "[DBG: msh_exec_pwd: ex = %d]\n", ex) : 0;
		exit((e->pwd_ex = ex));
	}
	else if (pid < 0)
		ft_dprintf(2, "minishell: %s: Error forking\n", *e->cmdv);
	else
	{
		wid = wait(&st);
		e->wid = wid;
		g_dbg ? ft_dprintf(2, "[DBG: msh_exec_pwd: st = %d]\n", st) : 0;
		g_dbg ? ft_dprintf(2, "[DBG: msh_exec_pwd: wid = %d]\n", wid) : 0;
		return ((e->st = st));
	}
	return (255);
}

#define IFNT else if
#define SHR8(x) ((x) >> 8)
#define CHKEP(x) (SHR8(x) != 127 || (x) != 11)

int			msh_exec(t_shenv *e)
{
	int	st_b;
	int	st_d;
	int	st_p;

	g_dbg ? ft_dprintf(2, "[DBG: msh_exec: e->cmdv[0] = %s]\n", e->cmdv[0]) : 0;
	if ((st_b = msh_exec_builtin(e)) != 0)
		return (st_b);
	g_dbg ? ft_dprintf(2, "[DBG: msh_exec: st_b = %d]\n", st_b) : 0;
	if (*e->cmdv && **e->cmdv && ft_strchr(*e->cmdv, '/'))
	{
		if (!access(*e->cmdv, F_OK) && access(*e->cmdv, X_OK) < 0)
		{
			ft_dprintf(2, "minishell: %s: Permission denied\n", *e->cmdv);
			return (0);
		}
		else if (((st_d = msh_exec_pwd(e)) >> 8) != 127)
		{
			g_dbg ? ft_dprintf(2, "[DBG: msh_exec: st_d = %d]\n", st_d) : 0;
			return (st_d);
		}
	}
	if (((st_p = msh_exec_path(e)) || CHKEP(st_p)))
	{
		g_dbg ? ft_dprintf(2, "[DBG: msh_exec: st_p = %d]\n", st_p) : 0;
		return (st_p);
	}
	ft_dprintf(2, "minishell: %s: Unknown command\n", *e->cmdv);
	return (0);
}

void		msh_print_prompt(void)
{
	char	*buf;
	size_t	idx;

	g_dbg ? ft_dprintf(2, "[DBG: msh_print_prompt]\n") : 0;
	buf = ft_strnew(4097); // NULL check maybe?
	getcwd(buf, 4096);
	idx = ft_strlen(g_shenv->home);
	if (ft_strnequ(buf, g_shenv->home, idx))
		buf[idx - 1] = '~';
	if (buf[idx - 1] == '~')
		ft_printf("%s msh$ ", buf + idx - 1);
	else
		ft_printf("%s msh$ ", buf);
	free(buf);
}

char		*msh_readline(void)
{
	char	*ln;

	msh_print_prompt();
	if (!get_next_line(0, &ln))
		exit(g_shenv->ret);
	return (ln);
}

#define TOKSEP "\t\r\n\a \\;"

static const char	g_toksep[] = {
	0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09,
	0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10, 0x11, 0x12,
	0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B,
	0x1C, 0x1D, 0x1E, 0x1F, 0x20, 0x00
};

#define TOK_BUFSZ (64)

void		msh_panic(char *msg)
{
	ft_dprintf(2, "minishell: %s\n", msg);
	exit(1);
}

//subst.c subst.h
char		*msh_expand(char *token)
{
	char	*tmp;
	char	*ret;

	ret = ft_strdup(token);
	g_dbg ? ft_dprintf(2, "[DBG: msh_expand: token = %s]\n", token) : 0;
	if ((tmp = ft_strchr(ret, '~')))
	{
		free(ret);
		if (!(ret = ft_strjoin(g_shenv->home, tmp + 1)))
			msh_panic("Memory allocation error in msh_tilde");
	}
	else if ((tmp = ft_strchr(ret, '$')))
	{
		ft_dprintf(2, "minishell: $VAR expansion: TODO\n");
	}
	g_dbg ? ft_dprintf(2, "[DBG: msh_expand: ret = %s]\n", ret) : 0;
	return (ret);
}

//subst.c subst.h
char		**msh_tokenize(char *str)
{
	char	**tokens;
	char	*token;
	int		i;
	int		bsz;

	bsz = TOK_BUFSZ;
	if (!(tokens = malloc(bsz * sizeof(char*))))
		msh_panic("Memory allocation error in msh_tokenize");
	token = ft_strtok(str, TOKSEP);
	i = -1;
	while (token)
	{
		if (ft_strchr(token, '~'))
			tokens[++i] = msh_expand(token);
		g_dbg ? ft_dprintf(2, "[DBG: msh_tokenize: token = %s]\n", token) : 0;
		if (i >= bsz)
		{
			if (!(tokens = ft_realloc(tokens, bsz * sizeof(char*),
									  (bsz + TOK_BUFSZ) * sizeof(char*))))
				msh_panic("Memory reallocation error");
			bsz += TOK_BUFSZ;
		}
		token = ft_strtok(NULL, (char*)&g_toksep);
	}
	tokens[i] = NULL;
	return (tokens);
}

void		msh_parse(char **inpt)
{
	char			**tkns;
	register int	i;

	g_dbg ? ft_dprintf(2, "[DBG: msh_parse: start]\n") : 0;
	i = -1;
	while (inpt[++i])
	{
		tkns = msh_tokenize(inpt[i]);
		// msh_expand_dong(tkns);
		if (*tkns)
		{
			g_shenv->cmdv = tkns;
			g_shenv->cmdc = strvec_len(tkns);
			msh_exec(g_shenv);
			strvec_dispose(tkns);
		}
		else
			free(tkns);
	}
	strvec_dispose(inpt);
}

void		msh_repl(void)
{
	char	*ln;
	char	**boy;

	g_dbg ? ft_dprintf(2, "[DBG: msh_repl: start]\n") : 0;
	while (1)
	{
		if (g_shenv->exit_called || g_shenv->ret)
			break ;
		ln = msh_readline();
		if (!(*ln))
		{
			ft_strdel(&ln);
			continue ;
		}
		boy = ft_strsplit(ln, ';');
		ft_strdel(&ln);
		msh_parse(boy);
		strvec_dispose(boy);
	}
	g_dbg ? ft_dprintf(2, "[DBG: msh_repl: end]\n") : 0;
}

void		msh_prompt(void)
{
	int		b;
	char	*ln;
	t_shenv	*e;

	ln = NULL;
	e = g_shenv;
	while (1)
	{
		msh_print_prompt();
		if ((b = get_next_line(0, &ln)) > 0)
		{
			e->cmdv = ft_strsplit(ln, ' ');
			ft_memdel((void**)&ln);
			e->cmdc = strvec_len(e->cmdv);
			e->ret = !e->ret ? msh_exec(e) : e->ret;
			strvec_dispose(e->cmdv);
		}
		else
			break ;
	}
}
void		msh_usage(int ex, char *v)
{
	ft_dprintf(2, "Usage: %s [option] ...\n", v);
	ft_dprintf(2, "\t-d : increase debug output\n");
	ft_dprintf(2, "\t-h : display this help output\n");
	exit(ex);
}

int			main(int argc, char **argv, char **envp, char **aplv)
{
	static t_shenv	e;
	static t_margs	m;
	int				ch;
	register int	i;

	g_dbg = 0;
	while ((ch = ft_getopt(argc, argv, "dh")) != -1)
	{
		if (ch == 'd')
			g_dbg++;
		else
			msh_usage(ch != 'h', *argv);
	}
	i = -1;
	m.c = argc;
	m.v = argv;
	m.e = envp;
	m.a = aplv;
	e.m = &m;
	e.envp = envp;
	while (envp && envp[++i])
	{
		if (ft_strnequ("HOME=", envp[i], 5))
			e.home = ft_strdup(envp[i] + 5);
		if (ft_strnequ("PATH=", envp[i], 5))
			e.path = ft_strsplit(envp[i] + 5, ':');
	}
	g_shenv = &e;
	msh_prompt();
	return (0);
}
