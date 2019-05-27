/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: callen <callen@student.42.us.org>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/04/12 14:56:58 by callen            #+#    #+#             */
/*   Updated: 2019/05/27 14:10:41 by callen           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "msh_strvec.h"
#include "libft.h"
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>

void		msh_debug_print(char *fmt, ...)
{
	va_list	ap;

	if (g_dbg)
	{
		va_start(ap, fmt);
		write(2, "[DBG: ", 6);
		ft_vdprintf(2, fmt, ap);
		write(2, "]\n",  2);
		va_end(ap);
	}
}

void		env_builtin(t_shenv *e)
{
	register int	i;

	if (!e || !e->envlst)
		msh_panic("env_builtin arg *e is NULL, WHAT DID YOU DO?!?");
	msh_debug_print("env_builtin: start");
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

	msh_debug_print("exit_builtin: start");
	e->exit_called = 1;
	if (e->cmdc > 1)
		st = ft_atoi(e->cmdv[1]);
	else
		st = e->ret;
	msh_debug_print("exit_builtin: st(%d)", st);
	ft_dprintf(2, "exit\n");
	exit(st);
}

void		setenv_builtin(t_shenv *e)
{
	char	*t;
	int		r;
	
	msh_debug_print("setenv: start l(%d)s(%d)", e->envlst->list_len, e->envlst->list_size);
	if (!e || e->cmdc != 2 || !e->cmdv[1] || !e->envlst)
		return ;
	msh_debug_print("setenv: cmdv[1](%s)", e->cmdv[1]);
	if (ft_strlen(e->cmdv[1]) < 3 || !(t = ft_strchr(e->cmdv[1], '=')))
		return ;
	r = strlist_nremove(e->envlst, e->cmdv[1], t - e->cmdv[1]);
	msh_debug_print("setenv: r(%d)", r);
	e->envlst = strlist_add(e->envlst, e->cmdv[1]);
	msh_debug_print("setenv: l(%d)s(%d)",
				e->envlst->list_len, e->envlst->list_size);
	msh_debug_print("setenv: end");
}

void		unsetenv_builtin(t_shenv *e)
{
	int		r;

	msh_debug_print("unsetenv: start");
	if (!e || e->cmdc != 2 || !e->cmdv[1] || !e->envlst)
		return ;
	msh_debug_print("unsetenv: cmdv[1](%s)", e->cmdv[1]);
	r = strlist_nremove(e->envlst, e->cmdv[1], ft_strlen(e->cmdv[1]));
	msh_debug_print("unsetenv: end");
}

/* TODO: rename to msh_signal_handler or something */
void		msh_sigint(int sig)
{
	msh_debug_print("sigint: start sig(%d)sr(%d)", sig, g_shenv->signal_recv);
	g_shenv->signal_recv = sig ? 1 : 0;
	if (sig == SIGINT)
	{
		write(1, "\n", 1);
		msh_debug_print("sigint: pp(%d)sr(%d)",
					g_shenv->prompt_printed, g_shenv->signal_recv);
		if (!g_shenv->prompt_printed && !g_shenv->signal_recv)
		{
			msh_print_prompt();
		}
		else
		{
			g_shenv->prompt_printed = 0;
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

	msh_debug_print("exec_path: start cmdv(%s)", *e->cmdv);
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
				msh_debug_print("exec_path: s(%s)", s);
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
	e->signal_recv = 1;
	msh_debug_print("exec_path: end ret(%d)i(%d)ex(%d)", e->ret, i, ex);
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
	msh_debug_print("exec_builtin: start cmdv(%s)", *e->cmdv);
	while (g_cmds[++i].cmd != 0)
	{
		if (ft_strequ(g_cmds[i].cmd, *e->cmdv))
		{
			msh_debug_print("exec_builtin: c[%d].cmd(%s)", i, g_cmds[i].cmd);
			g_cmds[i].f(e);
			status = 0;
			break ;
		}
	}
	msh_debug_print("exec_builtin: end status(%d)", status);
	return (status);
}

int			msh_exec_pwd(t_shenv *e)
{
	int		st;
	int		ex;
	pid_t	pid;
	pid_t	wid;

	msh_debug_print("exec_pwd: start cmdv(%s)", *e->cmdv);
	pid = fork();
	signal(SIGINT, msh_sigint);
	if (!pid)
	{
		if ((ex = execve(*e->cmdv, e->cmdv, e->envlst->list)) == -1)
			exit(127);
		msh_debug_print("exec_pwd: child ex(%d)", ex);
		exit((e->pwd_ex = ex));
	}
	else if (pid < 0)
		return (!!ft_dprintf(2, "minishell: %s: Error forking\n", *e->cmdv) - 2);
	else
	{
		wid = wait(&st);
		e->wid = wid;
		msh_debug_print("exec_pwd: parent st(%d)wid(%d)", st, wid);
		return ((e->st = st));
	}
	msh_debug_print("exec_pwd: end");
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
			return (st_d);
		}
	}
	st_p = msh_exec_path(e);
	msh_debug_print("exec: path st_b(%d)st_d(%d)st_p(%d)", st_b, st_d, st_p);
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

void		msh_print_prompt(void)
{
	char	*buf;
	char	*tmp;
	int		idx;

	msh_debug_print("print_prompt: start p(%d) sr(%d)", g_shenv->prompt_printed, g_shenv->signal_recv);
	idx = 0;
	buf = getcwd(0, 0);
	tmp = get_string_value("HOME");
	msh_debug_print("print_prompt: tmp(%s) buf(%s)", tmp, buf);
	if (!g_shenv->prompt_printed)
	{
		if (tmp != 0 && buf != 0)
		{
			if (ft_strequ(buf, tmp))
				idx = 1;
			free(tmp);
		}
		if (idx)
			ft_printf("~ 0msh$ ");
		else if (idx == 0 && buf)
		{
			tmp = ft_strrchr(buf, '/') + 1;
			ft_printf("%s 1msh$ ", tmp);
		}
		else
		{
			ft_printf("msh$ ");
		}
	}
	FREE(buf);
	g_shenv->prompt_printed = 1;
}

char		*msh_readline(void)
{
	char	*ln;

	msh_debug_print("readline: start p(%d) sr(%d)", g_shenv->prompt_printed, g_shenv->signal_recv);
	if (!g_shenv->prompt_printed || !g_shenv->signal_recv)
	{
		msh_print_prompt();
	}
	g_shenv->signal_recv = 0;
	if (!get_next_line(0, &ln))
		exit(g_shenv->ret);
	msh_debug_print("msh_readline: end");
	return (ln);
}

#define TOKSEP "\t \\"

void		msh_panic(char *msg)
{
	ft_dprintf(2, "minishell: %s\n", msg);
	exit(1);
}

char		*msh_dollar(char *ret, char *tmp)
{
	t_strlst		*l;
	register int	i;
	char			*r;
	int				vl;

	msh_debug_print("msh_dollar: start ret(%s)tmp(%s)",	ret, tmp);
	if (!tmp || !*tmp || (r = NULL))
		return (ret);
	if (ft_strnequ("$?", tmp, 2))
		return (ft_itoa(SHR8(g_shenv->ret) & 0xff));
	vl = ft_strlen(tmp + 1);
	l = g_shenv->envlst;
	i = -1;
	while (vl && l && ++i < l->list_len)
	{
		if (ft_strnequ(l->list[i], tmp + 1, vl))
		{
			r = ft_strdup(ft_strchr(l->list[i], '=') + 1);
			msh_debug_print("msh_dollar: found r(%s)", r);
			free(ret);
			return (r);
		}
	}
	msh_debug_print("msh_dollar: end r(\"%s\")ret(%s)", r, ret);
	return (ret);
}

//subst.c subst.h
char		*msh_expand(char *token)
{
	char	*tmp;
	char	*ret;
	char	*rett;

	msh_debug_print("msh_expand: start token(%s)", token);
	ret = ft_strdup(token); //XXX: leaks for unknown command, and others
	rett = ret;
	free(token);
	if ((tmp = ft_strchr(ret, '~')))
	{
		if (!(g_shenv->home = get_string_value("HOME")) ||
			!(rett = ft_strjoin(g_shenv->home, tmp + 1)))
			msh_panic("Memory allocation error in msh_expand ~");
		free(g_shenv->home);
	}
	else if ((tmp = ft_strchr(ret, '$')))
	{
		if (!(rett = msh_dollar(ret, tmp)))
			msh_panic("Memory allocation error in msh_dollar");
	}
	msh_debug_print("msh_expand: end rett(%s)", rett);
	return (rett);
}

//subst.c subst.h
char		**msh_tokenize(char *str)
{
	char	*tmp;
	char	**tokens;
	int		i;

	msh_debug_print("msh_tokenize: start str(%s)", str);
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
		msh_debug_print("msh_tokenize: tokens[%d](%s)", i, tokens[i]);
	}
	msh_debug_print("msh_tokenize: end");
	return (tokens);
}

void		msh_parse(char **inpt)
{
	char			**tkns;
	register int	i;

	msh_debug_print("msh_parse: start");
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
	msh_debug_print("msh_parse: end");
}

int			msh_repl(void)
{
	char	*ln;
	char	*dink;
	char	**boy;

	msh_debug_print("msh_repl: start");
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
	msh_debug_print("msh_repl: end ret(%d)",g_shenv->ret);
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
	shenv->home = NULL;
	shenv->path = NULL;
	shenv->exit_called = 0;
	shenv->prompt_printed = 0;
	shenv->signal_recv = 0;
}

int			ft_strccmp(const char *s1, const char *s2, int c)
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
/*TODO: separate environment variable structure so VARNAME and VARVALUE are
 *      stored separately */
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
