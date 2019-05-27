/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: callen <callen@student.42.us.org>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/04/12 14:56:58 by callen            #+#    #+#             */
/*   Updated: 2019/05/26 20:38:42 by callen           ###   ########.fr       */
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
	register int	i;

	if (!e || !e->envlst)
		msh_panic("env_builtin arg *e is NULL, WHAT DID YOU DO?!?");
	g_dbg ? ft_dprintf(2, "[DBG: env_builtin: start]\n") : 0;
	i = 0;
	while (i < e->envlst->list_len)
	{
		ft_printf("%s\n", e->envlst->list[i]);
		i++;
	}
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

int			ft_strcncmp(const char *s1, const char *s2, int c)
{
	register const unsigned char	*p1;
	register const unsigned char	*p2;
	unsigned char					c1;
	unsigned char					c2;

	p1 = (const unsigned char*)s1;
	p2 = (const unsigned char*)s2;
	c1 = 0;
	c2 = 0;
	while (c1 == c2)
	{
		c1 = (unsigned char)*s1++;
		c2 = (unsigned char)*s2++;
		if (c1 == '\0' || c1 == (unsigned)c || c2 == (unsigned)c)
			return (c1 - c2);
	}
	return (c1 - c2);
}

void		setenv_builtin(t_shenv *e)
{
	char	*t;
	int		r;
	
	g_dbg ? ft_dprintf(2, "[DBG: setenv: start]\n") : 0;
	g_dbg ? ft_dprintf(2, "[DBG: setenv: l(%d)s(%d)]\n", e->envlst->list_len, e->envlst->list_size) : 0;
	if (!e || e->cmdc != 2 || !e->cmdv[1] || !e->envlst)
		return ;
	g_dbg ? ft_dprintf(2, "[DBG: setenv: e->cmdv[1](%s)]\n", e->cmdv[1]) : 0;
	if (ft_strlen(e->cmdv[1]) < 3 || !(t = ft_strchr(e->cmdv[1], '=')))
		return ;
	r = strlist_nremove(e->envlst, e->cmdv[1], t - e->cmdv[1]);
	g_dbg ? ft_dprintf(2, "[DBG: setenv: r(%d)]\n", r) : 0;
	e->envlst = strlist_add(e->envlst, e->cmdv[1]);
	g_dbg ? ft_dprintf(2, "[DBG: setenv: l(%d)s(%d)]\n", e->envlst->list_len, e->envlst->list_size) : 0;
	g_dbg ? ft_dprintf(2, "[DBG: setenv: end]\n") : 0;
}

void		unsetenv_builtin(t_shenv *e)
{
	int		r;

	g_dbg ? ft_dprintf(2, "[DBG: unsetenv: start]\n") : 0;
	if (!e || e->cmdc != 2 || !e->cmdv[1] || !e->envlst)
		return ;
	g_dbg ? ft_dprintf(2, "[DBG: unsetenv: e->cmdv[1](%s)]\n", e->cmdv[1]) : 0;
	r = strlist_nremove(e->envlst, e->cmdv[1], ft_strlen(e->cmdv[1]));
	g_dbg ? ft_dprintf(2, "[DBG: unsetenv: end]\n") : 0;
}

void		msh_sigint_sub(int sig)
{
	g_dbg ? ft_dprintf(2, "[DBG: msh_sigint_sub: start:sig(%d)sr(%d)]\n", sig,g_shenv->signal_recv) : 0;
	g_shenv->signal_recv = !g_shenv->signal_recv ? sig : 0;
	if (sig == SIGINT)
	{
		write(1, "\n", 1);
		signal(SIGINT, msh_sigint);
	}
}

void		msh_sigint(int sig)
{
	g_dbg ? ft_dprintf(2, "[DBG: msh_sigint: start:sig(%d)sr(%d)]\n", sig,g_shenv->signal_recv) : 0;
	if (sig == SIGINT)
	{
		write(1, "\n", 1);
		if (!g_shenv->prompt_printed && !g_shenv->signal_recv)
		{
			msh_print_prompt();
			g_shenv->prompt_printed = 1;
		}
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
				g_dbg ? ft_dprintf(2, "[DBG: msh_exec_path: s(%s)]\n", s) : 0;
				if (!access(s, F_OK) && access(s, X_OK) < 0)
				{
					ft_dprintf(2, "minishell: %s: Permission denied\n", s);
					exit(126);
				}
				ex = execve(s, e->cmdv, e->envlst->list);
				free(s);
			}
		else
			if (e->prompt_printed == 1)
				ft_dprintf(2, "minishell: %s: Unknown command\n", *e->cmdv);
		exit(ex);
	}
	else if (pid < 0)
		ft_dprintf(2, "minishell: %s: Error forking\n", *e->cmdv);
	else
		i = wait(&e->ret);
	FREE(e->home);
	strvec_dispose(e->path);
	e->path = NULL;
	e->signal_recv = 1;
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
		if ((ex = execve(*e->cmdv, e->cmdv, e->envlst->list)) == -1)
			exit(127);
		g_dbg ? ft_dprintf(2, "[DBG: msh_exec_pwd: child ex(%d)]\n", ex) : 0;
		exit((e->pwd_ex = ex));
	}
	else if (pid < 0)
		exit(-2 + !(!ft_dprintf(2, "-minishell: %s: Error forking\n", *e->cmdv)));
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
	g_dbg ? ft_dprintf(2, "[DBG: msh_exec: start:e->cmdv[0](%s)]\n", e->cmdv[0]) : 0;
	st_b = msh_exec_builtin(e);
	g_dbg ? ft_dprintf(2, "[DBG: msh_exec: exec_builtin:st_b(%d)]\n", st_b) : 0;
	if (WIFEXITED(st_b))
		return (st_b);
	if (ISABSP(*e->cmdv) || ISPWD(*e->cmdv) || ISRELP(*e->cmdv))
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
	g_dbg ? ft_dprintf(2, "[DBG: msh_exec:exec_path st_b(%d)st_d(%d)st_p(%d)]\n", st_b,st_d,st_p) : 0;
	if (CHKEP(st_p) && WIFEXITED(st_p))
	{
		g_dbg ? ft_dprintf(2, "[DBG: msh_exec: exit:st_p(%d)]\n", st_p) : 0;
		return (st_p);
	}
	else if ((st_b == 1 && !st_d && SHR8(st_p) == 255) || !g_shenv->signal_recv)
		ft_dprintf(2, "minishell: %s: Unknown command\n", *e->cmdv);
	else
		e->signal_recv = 0;
	return (127);
}

void		msh_print_prompt(void)
{
	char	*buf = 0;
	char	*home = 0;
	size_t	idx = 0;

	g_dbg ? ft_dprintf(2, "[DBG: msh_print_prompt: start p(%d) sr(%d)]\n",g_shenv->prompt_printed,g_shenv->signal_recv) : 0;
	buf = getcwd(0, 0);
	home = get_string_value("HOME");
	g_dbg ? ft_dprintf(2, "[DBG: msh_print_prompt: home(%s) buf(%s)]\n",home,buf) : 0;
	if (home && buf)
	{
		idx = ft_strlen(home);
		if (ft_strnequ(buf, home, idx))
			buf[idx - 1] = '~';
		if (buf[idx - 1] == '~')
			ft_printf("%s msh$ ", buf + idx - 1);
		else
			ft_printf("%s msh$ ", buf);
		FREE(home);
	}
	else if (buf)
		ft_printf("%s msh$ ", buf);
	else
		ft_printf("msh$ ");
	FREE(buf);
	g_shenv->prompt_printed = 1;
}

char		*msh_readline(void)
{
	char	*ln;

	g_dbg ? ft_dprintf(2, "[DBG: msh_readline: start p(%d) sr(%d)]\n",g_shenv->prompt_printed,g_shenv->signal_recv) : 0;
	if (!g_shenv->prompt_printed || !g_shenv->signal_recv)
	{
		msh_print_prompt();
	}
	else
	{
		g_shenv->prompt_printed = 0;
	}
	g_shenv->signal_recv = 0;
	if (!get_next_line(0, &ln))
		exit(g_shenv->ret);
	g_dbg ? ft_dprintf(2, "[DBG: msh_readline: end]\n") : 0;
	return (ln);
}

#define TOKSEP "\t \\"

void		msh_panic(char *msg)
{
	ft_dprintf(2, "-minishell: %s\n", msg);
	exit(1);
}

char		*msh_dollar(char *ret, char *tmp)
{
	t_strlst		*l;
	register int	i;
	char			*r;
	int				vl;

	g_dbg ? ft_dprintf(2, "[DBG: msh_dollar: start:ret(%s),tmp(%s)]\n", ret,tmp) : 0;
	if (!tmp || !*tmp || (r = NULL))
		return (ret);
	vl = ft_strlen(tmp + 1);
	if (ft_strnequ("$?", tmp, 2))
	{
		free(ret);
		r = ft_itoa(SHR8(g_shenv->ret) & 0xff);
		vl = 0;
	}
	l = g_shenv->envlst;
	i = -1;
	while (vl && l && ++i < l->list_len)
	{
		if (ft_strnequ(l->list[i], tmp + 1, vl))
		{
			free(ret);
			r = ft_strdup(ft_strchr(l->list[i], '=') + 1);
			g_dbg ? ft_dprintf(2, "[DBG: msh_dollar: foundr(%s)]\n", r) : 0;
			break ;
		}
	}
	g_dbg ? ft_dprintf(2, "[DBG: msh_dollar: subst(%s)]\n", r) : 0;
	return (r ? r : ret);
}

//subst.c subst.h
char		*msh_expand(char *token)
{
	char	*tmp;
	char	*ret;
	char	*rett;

	g_dbg ? ft_dprintf(2, "[DBG: msh_expand: start:token(%s)]\n", token) : 0;
	ret = ft_strdup(token);
	rett = ret;
	free(token);
	if ((tmp = ft_strchr(ret, '~')))
	{
		ft_strdel(&ret);
		g_shenv->home = get_string_value("HOME");
		if (!(rett = ft_strjoin(g_shenv->home, tmp + 1)))
			msh_panic("Memory allocation error in msh_expand ~");
		free(g_shenv->home);
	}
	else if ((tmp = ft_strchr(ret, '$')))
	{
		if (!(rett = msh_dollar(ret, tmp)))
			msh_panic("Memory allocation error in msh_expand $");
	}
	g_dbg ? ft_dprintf(2, "[DBG: msh_expand: end:ret(%s)]\n", rett) : 0;
	return (rett);
}

//subst.c subst.h
char		**msh_tokenize(char *str)
{
	char	*tmp;
	char	**tokens;
	int		i;

	g_dbg ? ft_dprintf(2, "[DBG: msh_tokenize: start(%s)]\n", str) : 0;
	if (!(tokens = ft_strsplit(str, ' ')))
		msh_panic("Memory allocation error in msh_tokenize");
	i = -1;
	while (tokens[++i])
	{
		tmp = ft_strtrim(tokens[i]);
		free(tokens[i]);
		tokens[i] = tmp;
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
	while (inpt && inpt[++i])
	{
		tkns = msh_tokenize(inpt[i]);
		if (tkns && *tkns)
		{
			g_shenv->cmdv = tkns;
			g_shenv->cmdc = strvec_len(tkns);
			g_shenv->ret = msh_exec(g_shenv);
			strvec_dispose(tkns);
		}
		else
			FREE(tkns);
	}
	strvec_dispose(inpt);
	g_dbg ? ft_dprintf(2, "[DBG: msh_parse: end]\n") : 0;
}

int			msh_repl(void)
{
	char	*ln;
	char	*dink;
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
		dink = ft_strtrim(ln);
		boy = ft_strsplit(dink, ';');
		msh_parse(boy);
		FREE(ln);
		FREE(dink);
		g_shenv->prompt_printed = 0;
	}
	g_dbg ? ft_dprintf(2, "[DBG: msh_repl: end:ret(%d)]\n",g_shenv->ret) : 0;
	return (g_shenv->ret);
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
		/* if (ft_strnequ("SHLVL=", mg->e[i]) */
		shenv->envlst->list[i] = ft_strdup(mg->e[i]);
		shenv->envlst->list_len++;
	}
	shenv->envp = strvec_copy(mg->e);
	shenv->cmdv = NULL;
	shenv->sl = NULL;
	shenv->list = strvec_to_word_list(mg->e, 1, 0);
	shenv->envplen = strvec_len(mg->e);
	shenv->wid = 0;
	shenv->pwd_ex = 0;
	shenv->st = 0;
	shenv->dl = 0;
	shenv->cmdc = 0;
	shenv->ret = 0;
	shenv->exit_called = 0;
	shenv->home = NULL;
	shenv->path = NULL;
	shenv->prompt_printed = 0;
	shenv->signal_recv = 0;
}
/*TODO: separate environment variable structure so VARNAME and VARVALUE are
 *      stored separately */
int			main(int argc, char **argv, char **envp, char **aplv)
{
	static t_shenv	e;
	static t_margs	m;
	int				ch;
	register int	i;

	signal(SIGINT, msh_sigint);
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
	init_shenv(&e, &m);
	g_shenv = &e;
	if (g_dbg == 3)
	{
		char *tmp = "\t\t \t     \tls -la   \t    \t\t ;echo toast boast\t\t \t";
		char *t2 = ft_strtrim(tmp);
		ft_printf("%s\n", t2);
	}
	else
		ch = msh_repl();
	g_dbg ? ft_dprintf(2, "[DBG: main: end:ch(%d)]\n", ch) : 0;
	return (ch);
}
