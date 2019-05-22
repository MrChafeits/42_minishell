/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: callen <callen@student.42.us.org>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/04/12 14:56:58 by callen            #+#    #+#             */
/*   Updated: 2019/05/21 17:32:26 by callen           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "msh_strvec.h"
#include "libft.h"
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>


void		env_builtin(t_shenv *e)
{
	register int i;

	g_dbg ? ft_dprintf(2, "[DBG: env_builtin: start]\n") : 0;
	i = -1;
	while (e->m->e && e->m->e[++i])
		ft_printf("%s\n", e->m->e[i]);
}

void		exit_builtin(t_shenv *e)
{
	int		st;

	g_dbg ? ft_dprintf(2, "[DBG: exit_builtin: start]\n") : 0;
	e->exit_called = 1;
	if (e->cmdc > 1)
		st = ft_atoi(e->cmdv[1]);
	else
		st = e->ret;
	g_dbg ? ft_dprintf(2, "[DBG: exit_builtin: st(%d)]\n", st) : 0;
	ft_dprintf(2, "exit\n");
	exit(st);
}

void		setenv_builtin(t_shenv *e)
{
	t_strlst	*tmp;
	t_strlst	*res;
	
	g_dbg ? ft_dprintf(2, "[DBG: setenv: start]\n") : 0;
	if (!e || e->cmdc != 2 || !e->cmdv[1])
		return ;
	tmp = strlist_new(1);
	tmp->list[0] = SAVESTR(e->cmdv[1]);
	res = strlist_append(e->sl, tmp);
	strlist_dispose(tmp);
	strlist_dispose(e->sl);
	e->sl = res;
	g_dbg ? ft_dprintf(2, "[DBG: setenv: end]\n") : 0;
}

void		unsetenv_builtin(t_shenv *e)
{
	g_dbg ? ft_dprintf(2, "[DBG: unsetenv: start]\n") : 0;
	if (!e || e->cmdc != 2 || !e->cmdv[1])
		return ;
	strlist_nremove(e->sl, e->cmdv[1]);
	g_dbg ? ft_dprintf(2, "[DBG: unsetenv: end]\n") : 0;
}

void		msh_sigint_sub(int sig)
{
	g_dbg ? ft_dprintf(2, "[DBG: msh_sigint_sub: start:sig(%d)]\n", sig) : 0;
	if (sig == SIGINT)
	{
		write(1, "\n", 1);
		signal(SIGINT, msh_sigint);
	}
}

void		msh_sigint(int sig)
{
	g_dbg ? ft_dprintf(2, "[DBG: msh_sigint: start:sig(%d)]\n", sig) : 0;
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
	int				ex;
	char			*s;
	pid_t			pid;

	g_dbg ? ft_dprintf(2, "[DBG: msh_exec_path: start:cmdv(%s)]\n", *e->cmdv) : 0;
	ex = 0;
	pid = fork();
	signal(SIGINT, msh_sigint);
	if (!pid && (i = -1))
	{
		while (e->path[++i])
		{
			s = ft_strjoin(e->path[i], "/");
			s = ft_strjoin_free(s, *e->cmdv, 'L');
			g_dbg ? ft_dprintf(2, "[DBG: msh_exec_path: s(%s)]\n", s) : 0;
			if (!access(s, F_OK) && access(s, X_OK) < 0)
			{
				ft_dprintf(2, "minishell: %s: Permission denied\n", s);
				exit(126);
			}
			ex = execve(s, e->cmdv, e->m->e);
			free(s);
		}
		exit(ex);
	}
	else if (pid < 0)
		ft_dprintf(2, "minishell: %s: Error forking\n", *e->cmdv);
	else
		i = wait(&e->ret);
	g_dbg ? ft_dprintf(2, "[DBG: msh_exec_path: end:ret(%d)i(%d)ex(%d)]\n", e->ret,i,ex) : 0;
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
	g_dbg ? ft_dprintf(2, "[DBG: msh_exec_builtin: start:cmdv(%s)]\n", *e->cmdv) : 0;
	while (g_cmds[++i].cmd != 0)
	{
		if (ft_strequ(g_cmds[i].cmd, *e->cmdv))
		{
			g_dbg ? ft_dprintf(2,
			"[DBG: msh_exec_builtin: g_cmds[%d].cmd(%s)]\n", i, g_cmds[i].cmd) : 0;
			g_cmds[i].f(e);
			status = 0;
			break ;
		}
	}
	g_dbg ? ft_dprintf(2, "[DBG: msh_exec_builtin: end:status(%d)]\n", status) : 0;
	return (status);
}

int			msh_exec_pwd(t_shenv *e)
{
	int		st;
	int		ex;
	pid_t	pid;
	pid_t	wid;

	g_dbg ? ft_dprintf(2, "[DBG: msh_exec_pwd: start:cmdv(%s)]\n", *e->cmdv) : 0;
	pid = fork();
	signal(SIGINT, msh_sigint);
	if (!pid)
	{
		if ((ex = execve(*e->cmdv, e->cmdv, e->m->e)) == -1)
			exit(127);
		g_dbg ? ft_dprintf(2, "[DBG: msh_exec_pwd: child ex(%d)]\n", ex) : 0;
		exit((e->pwd_ex = ex));
	}
	else if (pid < 0)
		exit(-1 + !(!ft_dprintf(2, "minishell: %s: Error forking\n", *e->cmdv)));
	else
	{
		wid = wait(&st);
		e->wid = wid;
		g_dbg ? ft_dprintf(2, "[DBG: msh_exec_pwd: parent st(%d)wid(%d)]\n", st,wid) : 0;
		return ((e->st = st));
	}
	g_dbg ? ft_dprintf(2, "[DBG: msh_exec_pwd: end]\n") : 0;
	return (255);
}

#define IFNT else if
#define SHR8(x) ((x) >> 8)
#define CHKEP(x) ((SHR8(x) & 0xff) != 127 && (SHR8(x) & 0xff) != 255 && (x) != 11)

int			msh_exec(t_shenv *e)
{
	int	st_b;
	int	st_d;
	int	st_p;

	g_dbg ? ft_dprintf(2, "[DBG: msh_exec: start:e->cmdv[0](%s)]\n", e->cmdv[0]) : 0;
	st_b = msh_exec_builtin(e);
	g_dbg ? ft_dprintf(2, "[DBG: msh_exec: exec_builtin:st_b(%d)]\n", st_b) : 0;
	if (WIFEXITED(st_b))
		return (st_b);
	if (ft_strchr(*e->cmdv, '/'))
	{
		g_dbg ? ft_dprintf(2, "[DBG: msh_exec: found /]\n") : 0;
		if (!access(*e->cmdv, F_OK) && access(*e->cmdv, X_OK) < 0)
		{
			ft_dprintf(2, "minishell: %s: Permission denied\n", *e->cmdv);
			return (126);
		}
		st_d = msh_exec_pwd(e);
		g_dbg ? ft_dprintf(2, "[DBG: msh_exec: exec_pwd:st_d(%d)]\n", st_d) : 0;
		if (WIFEXITED(st_d))
		{
			g_dbg ? ft_dprintf(2, "[DBG: msh_exec: exit:st_d(%d)]\n", st_d) : 0;
			return (st_d);
		}
	}
	st_p = msh_exec_path(e);
	g_dbg ? ft_dprintf(2, "[DBG: msh_exec: exec_path:st_p(%d)]\n", st_p) : 0;
	if (CHKEP(st_p) && WIFEXITED(st_p))
	{
		g_dbg ? ft_dprintf(2, "[DBG: msh_exec: exit:st_p(%d)]\n", st_p) : 0;
		return (st_p);
	}
	ft_dprintf(2, "minishell: %s: Unknown command\n", *e->cmdv);
	return (127);
}

void		msh_print_prompt(void)
{
	char	*buf;
	size_t	idx;

	g_dbg ? ft_dprintf(2, "[DBG: msh_print_prompt: start]\n") : 0;
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

	g_dbg ? ft_dprintf(2, "[DBG: msh_readline: start]\n") : 0;
	msh_print_prompt();
	if (!get_next_line(0, &ln))
		exit(g_shenv->ret);
	g_dbg ? ft_dprintf(2, "[DBG: msh_readline: end]\n") : 0;
	return (ln);
}

#define TOKSEP "\t \\;"
#define TOK_BUFSZ (64)

void		msh_panic(char *msg)
{
	ft_dprintf(2, "minishell: %s\n", msg);
	exit(1);
}

char		*msh_dollar(char *ret, char *tmp)
{
	register int	i;
	int				vl;

	g_dbg ? ft_dprintf(2, "[DBG: msh_dollar: start:ret(%s),tmp(%s)]\n", ret,tmp) : 0;
	i = -1;
	if (!tmp || !*tmp)
		return (ret);
	vl = ft_strlen(tmp + 1);
	while (++i < g_shenv->envplen)
		if (ft_strnequ(g_shenv->envp[i], tmp + 1, vl))
		{
			free(ret);
			tmp = NULL;
			ret = ft_strdup(ft_strchr(g_shenv->envp[i], '=') + 1);
			break ;
		}
	if (tmp && ft_strnequ("$?", tmp, 2))
	{
		free(ret);
		tmp = NULL;
		ret = ft_itoa(SHR8(g_shenv->ret) & 0xff);
	}
	g_dbg ? ft_dprintf(2, "[DBG: msh_dollar: subst(%s)]\n", ret) : 0;
	return (ret);
}

//subst.c subst.h
char		*msh_expand(char *token)
{
	char	*tmp;
	char	*ret;

	ret = ft_strdup(token);
	g_dbg ? ft_dprintf(2, "[DBG: msh_expand: start:token(%s)]\n", token) : 0;
	if ((tmp = ft_strchr(ret, '~')))
	{
		free(ret);
		if (!(ret = ft_strjoin(g_shenv->home, tmp + 1)))
			msh_panic("Memory allocation error in msh_expand ~");
	}
	else if ((tmp = ft_strchr(ret, '$')))
	{//echo $PATH
		if (!(ret = msh_dollar(ret, tmp)))
			msh_panic("Memory allocation error in msh_expand $");
	}
	g_dbg ? ft_dprintf(2, "[DBG: msh_expand: end:ret(%s)]\n", ret) : 0;
	return (ret);
}

//subst.c subst.h
char		**msh_tokenize(char *str)
{
	char	**tokens;
	int		i;

	g_dbg ? ft_dprintf(2, "[DBG: msh_tokenize: start]\n") : 0;
	if (!(tokens = ft_strsplit(str, ' ')))
		msh_panic("Memory allocation error in msh_tokenize");
	i = -1;
	while (tokens[++i])
	{
		if (ft_strchr(tokens[i], '~') || ft_strchr(tokens[i], '$'))
			tokens[i] = msh_expand(tokens[i]);
		g_dbg ? ft_dprintf(2, "[DBG: msh_tokenize: tokens[%d](%s)]\n",i,tokens[i]) : 0;
	}
	g_dbg ? ft_dprintf(2, "[DBG: msh_tokenize: end]\n") : 0;
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
		if (*tkns)
		{
			g_shenv->cmdv = tkns;
			g_shenv->cmdc = strvec_len(tkns);
			g_shenv->ret = msh_exec(g_shenv);
			strvec_dispose(tkns);
		}
		else
			free(tkns);
	}
	strvec_dispose(inpt);
	g_dbg ? ft_dprintf(2, "[DBG: msh_parse: end]\n") : 0;
}

int			msh_repl(void)
{
	char	*ln;
	char	**boy;

	g_dbg ? ft_dprintf(2, "[DBG: msh_repl: start]\n") : 0;
	while (1)
	{
		if (g_shenv->exit_called)
			break ;
		ln = msh_readline();
		if (ln && !(*ln))
		{
			free(ln);
			continue ;
		}
		boy = ft_strsplit(ln, ';');
		msh_parse(boy);
		if (boy && *boy && **boy)
			strvec_dispose(boy);
		if (ln && *ln)
			free(ln);
	}
	g_dbg ? ft_dprintf(2, "[DBG: msh_repl: end:ret(%d)]\n",g_shenv->ret) : 0;
	return (g_shenv->ret);
}

int			msh_prompt(void)
{
	int		b;
	char	*ln;
	t_shenv	*e;

	g_dbg ? ft_dprintf(2, "[DBG: msh_prompt: start]\n") : 0;
	ln = NULL;
	e = g_shenv;
	while (1)
	{
		msh_print_prompt();
		if ((b = get_next_line(0, &ln)) > 0)
		{
			g_dbg ? ft_dprintf(2, "[DBG: msh_prompt: ln(%s)]\n",ln) : 0;
			e->cmdv = msh_tokenize(ln);
			free(ln);
			e->cmdc = strvec_len(e->cmdv);
			e->ret = msh_exec(e);
			strvec_dispose(e->cmdv);
		}
		else
			break ;
	}
	g_dbg ? ft_dprintf(2, "[DBG: msh_prompt: end]\n") : 0;
	return (e->ret);
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

	g_dbg ? ft_dprintf(2, "[DBG: main: start]\n") : 0;
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
	e.sl = strlist_new(0);
	e.sl->list = strvec_copy(envp);
	e.sl->list_len = strvec_len(envp);
	e.sl->list_size = e.sl->list_len;
	while (envp && envp[++i])
	{
		if (ft_strnequ("HOME=", envp[i], 5))
			e.home = ft_strdup(envp[i] + 5);
		if (ft_strnequ("PATH=", envp[i], 5))
			e.path = ft_strsplit(envp[i] + 5, ':');
	}
	e.envplen = i;
	g_shenv = &e;
	/* ch = msh_prompt(); */
	ch = msh_repl();
	g_dbg ? ft_dprintf(2, "[DBG: main: end:ch(%d)]\n", ch) : 0;
	return (ch);
}
