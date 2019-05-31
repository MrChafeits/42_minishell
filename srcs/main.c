/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: callen <callen@student.42.us.org>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/04/12 14:56:58 by callen            #+#    #+#             */
/*   Updated: 2019/05/30 20:53:06 by callen           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "msh_strvec.h"
#include "libft.h"
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <pwd.h>
#include <sys/types.h>

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

#define IFNT else if
#define CHKEP(x) (SHR8(x) != 127 && SHR8(x) != 255 && (x) != 11)
#define ISPWD(s) ((s)[0] == '.' && (s)[1] == '/' && (s)[2])
#define ISRELP(s) ((s)[0] == '.' && (s)[1] == '.' && (s)[2] == '/' && (s)[3])
#define ISABSP(s) ((s)[0] == '/' && (s)[1])

/* TODO: rename to msh_signal_handler or something */
void		msh_sigint(int sig)
{
	msh_debug_print(
		"sigint: start sig(%d) pp(%d) sr(%d) ret(%d) pwd_ex(%d) path_ex(%d) st(%d) dl(%d)",
		sig,
		g_shenv->prompt_printed,
		g_shenv->signal_recv,
		g_shenv->ret,
		g_shenv->pwd_ex,
		g_shenv->path_ex,
		g_shenv->st,
		g_shenv->dl);
	g_shenv->signal_recv = 1;
	if (sig == SIGINT)
	{
		write(1, "\n", 1);
		g_shenv->prompt_printed = 0;
		msh_debug_print("sigint: pp(%d) sr(%d)",
					g_shenv->prompt_printed, g_shenv->signal_recv);
		if (!g_shenv->path_ex && !g_shenv->pwd_ex)
			msh_print_prompt();
		else
			g_shenv->path_ex = 0;
		signal(SIGINT, msh_sigint);
	}
}

int			msh_exec_path(t_shenv *e)
{
	register int	i;
	int				ex;
	char			*s;
	pid_t			pid;

	e->path_ex = 1;
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
	msh_debug_print("exec_path: end ret(%d) i(%d) ex(%d)", e->ret, i, ex);
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
	ft_dprintf(2, "minishell: %s: Unknown command\n", *e->cmdv);
	msh_debug_print("exec_pwd: end");
	return (255);
}

int			msh_exec(t_shenv *e)
{
	int	st_b;
	int	st_d;
	int	st_p;

	st_b = 0;
	st_d = 0;
	st_p = 0; msh_debug_print("exec: start cmdv[0](%s)", e->cmdv[0]);
	st_b = msh_exec_builtin(e); msh_debug_print("exec: builtin st_b(%d)", st_b);
	if (WIFEXITED(st_b))
		return (st_b);
	if (ISABSP(*e->cmdv) || ISPWD(*e->cmdv) || ISRELP(*e->cmdv))
	{ msh_debug_print("exec: found /");
		if (!access(*e->cmdv, F_OK) && access(*e->cmdv, X_OK) < 0)
		{
			ft_dprintf(2, "minishell: %s: Permission denied\n", *e->cmdv);
			return (126);
		}
		st_d = msh_exec_pwd(e); msh_debug_print("exec: pwd st_d(%d)", st_d);
		if (WIFEXITED(st_d))
		{ msh_debug_print("exec: pwd ret st_d(%d)", st_d);
			if (SHR8(st_d) == 127)
				ft_dprintf(2, "minishell: %s: Unknown command\n", *e->cmdv);
			return (st_d);
		}
	}
	st_p = msh_exec_path(e); msh_debug_print("exec: path st_b(%d) st_d(%d) st_p(%d)", st_b, st_d, st_p);
	if (CHKEP(st_p) && WIFEXITED(st_p))
	{ msh_debug_print("exec: path ret st_p(%d)", st_p);
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
	int		is_home;

	is_home = 0; msh_debug_print("print_prompt: start p(%d) sr(%d)", g_shenv->prompt_printed, g_shenv->signal_recv);
	buf = getcwd(0, 0);
	tmp = get_string_value("HOME"); msh_debug_print("print_prompt: tmp(%s) buf(%s)", tmp, buf);
	if (!g_shenv->prompt_printed)
	{
		if (tmp && buf)
		{
			is_home = ft_strequ(buf, tmp);
			free(tmp);
		}
		if (is_home)
			ft_printf("~ msh$ ");
		else if (is_home == 0 && buf)
			ft_printf("%s msh$ ", ft_strrchr(buf, '/') + !ft_strequ(buf, "/"));
		else
			ft_printf("msh$ ");
	}
	free(buf);
	g_shenv->prompt_printed = 1;
	g_shenv->pwd_ex = 0;
	g_shenv->path_ex = 0;
}

char		*msh_readline(void)
{
	char	*ln;
	msh_debug_print("readline: start p(%d) sr(%d)", g_shenv->prompt_printed, g_shenv->signal_recv);
	if (!g_shenv->prompt_printed || !g_shenv->signal_recv)
		msh_print_prompt();
	g_shenv->signal_recv = 0;
	if (!get_next_line(0, &ln))
		exit(g_shenv->ret);
	msh_debug_print("msh_readline: end ln(%s)", ln);
	return (ln);
}

#define TOKSEP "\t "

void		msh_panic(char *msg)
{
	ft_dprintf(2, "minishell: %s\n", msg);
	exit(1);
}

static const uint8_t	g_varctbl[256] = {
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 0, 0, 0, 0, 0, 0,
	0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1,
	0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
};

int			msh_varlen(const char *s)
{
	register int	i;
	char			*p;

	p = (char*)s;
	if (!s || !p)
		return (0);
	if (g_varctbl[(int)p[0]] & 4)
		p++;
	i = 0;
	while (g_varctbl[(int)p[i]] & 3)
		i++;
	return (i);
}

#define VL (ln[0])
#define RL (ln[1])
#define CL (ln[2])
#define HL (ln[3])
//TODO: cleanup this function and fix infinite loop for 'echo $$PWD'

/*
** Variable Dereference Substitution
**
** $* - flurgh
** $@ - agh
** $# - number of positional parameters
** $? - exit status of most recently executed foreground pipeline
** $- - current option flags as specified by invocation
** $$ - process ID of the shell
** $! - process ID of job most recently placed into the background
** $0 - name of the shell or shell script
** $_ - last word of the previous line of input
*/

char		*strsub_rep(char *str, char *pat, char *rep)
{
	char	*r;
	/* int		ln[4]; */
	int patlen, replen, newlen, tmplen, sublen=0, beplen = 0;

	msh_debug_print("strsub_rep: start str(%s) pat(%s) rep(%s)", str, pat, rep);
	patlen = ft_strlen(pat); msh_debug_print("strsub_rep: patlen(%d)", patlen);
	replen = ft_strlen(rep); msh_debug_print("strsub_rep: replen(%d)", replen);
	tmplen = ft_strlen(str); msh_debug_print("strsub_rep: tmplen(%d)", tmplen);
	r = ft_strstr(str, pat); msh_debug_print("strsub_rep: strstr(%s)", r);
	sublen = ft_strlen(r); msh_debug_print("strsub_rep: sublen(%d)", sublen);
	if (ft_strnequ(r, pat, patlen))
	{
		newlen = ft_strlen(str) - patlen + replen;
		msh_debug_print("strsub_rep: newlen(%d)", newlen);

		r = ft_strnew(newlen);
		ft_strncpy(r, str, tmplen - sublen);
		msh_debug_print("strsub_rep: str1 r(%s)", r);

		ft_strncpy(r + (tmplen - sublen), rep, replen);
		msh_debug_print("strsub_rep: repl r(%s)", r);

		beplen = ft_strlen(str + tmplen - sublen + patlen);
		msh_debug_print("strsub_rep: beplen(%d)", beplen);

		msh_debug_print("strsub_rep: str2(%s)", str + tmplen - sublen + patlen);
		msh_debug_print("strsub_rep: str2 r(%s)", r);
		ft_strncpy(r + tmplen - sublen + replen, str + tmplen - sublen + patlen, beplen);
		msh_debug_print("strsub_rep: str2 r(%s)", r);

	}
	msh_debug_print("strsub_rep: end r(%s)", r);
	return (r);
}

char		*msh_dollar(char *ret, char *tmp)
{
	char			*val;
	char			*var;
	char			*r;
	int				varlen;
	msh_debug_print("msh_dollar: start ret(%s)tmp(%s)", ret, tmp);
	if (!tmp || !*tmp || (r = NULL))
		return (ret);
	msh_debug_print("msh_dollar: ret(%s)tmp(%s)", ret, tmp);
	varlen = msh_varlen(tmp);
	var = ft_strndup(tmp, varlen + (!varlen ? 2 : 1));
	msh_debug_print("msh_dollar: varlen(%d) var(%s)", varlen, var);
	val = get_string_value(var + (!varlen ? 0 : 1));
	if (val)
	{
		r = strsub_rep(ret, var, val);
		free(val);
	}
	else
		r = ft_strdup(ret);
	free(var);
	g_shenv->exp_dollar = 1;
	msh_debug_print("msh_dollar: end r(\"%s\")ret(%s)", r, ret);
	return (r);
}

char		*msh_tilde(char *ret, char *tmp)
{
	char			*r[2];
	static char		*home;;
	struct passwd	*entry;

	msh_debug_print("msh_tilde: start ret(%s)tmp(%s)", ret, tmp);
	r[0] = get_string_value("HOME");
	if (r[0] == 0)
	{
		if (!home)
		{
			entry = getpwuid(getuid());
			if (entry)
				home = ft_strdup(entry->pw_dir);
		}
		r[0] = ft_strdup(home);
	}
	msh_debug_print("msh_tilde: ret(%s)tmp(%s)", ret, tmp);
	r[1] = strsub_rep(ret, "~", r[0]);
	msh_debug_print("msh_tilde: end r[1](\"%s\")ret(%s)", r[1], ret);
	free(r[0]);
	return (r[1]);
}

#undef VL
#undef RL
#undef CL
#undef HL

/*
**subst.c subst.h
*/

char		*msh_expand(char *token)
{
	char	*tmp;
	char	*ret;
	char	*rett;

	//TODO: fix `echo $$' `echo $?' `echo $$' weird behavior
	msh_debug_print("msh_expand: start token(%s)", token);
	ret = ft_strdup(token);
	rett = ret;
	free(token);
	if ((tmp = ft_strchr(ret, '~')))
	{
		msh_debug_print("msh_expand: found tilde");
		if (!(rett = msh_tilde(ret, tmp)))
			msh_panic("Memory allocation error in msh_expand ~");
		free(g_shenv->home);
	}
	else if ((tmp = ft_strchr(ret, '$')))
	{
		msh_debug_print("msh_expand: found dollar sign");
		if (!(rett = msh_dollar(ret, tmp)))
			msh_panic("Memory allocation error in msh_dollar");
	}
	if (ft_strequ(ret, rett))
		g_shenv->expand_var = 0;
	free(ret);
	msh_debug_print("msh_expand: end rett(%s)", rett);
	return (rett);
}

int			check_token_sub(const char *tok)
{
	static char	*rep[4];

	msh_debug_print("check_token_sub: start tok(%s)", tok);
	if (!rep[0])
		rep[0] = ft_strchr(tok, '~');
	else
	{
		rep[1] = rep[0];
		rep[0] = ft_strchr(tok, '~');
	}
	if (rep[0] && rep[0] != rep[1])
		return (1);
	if (!rep[2])
		rep[2] = ft_strchr(tok, '$');
	else
	{
		rep[3] = rep[2];
		rep[2] = ft_strchr(tok, '$');
	}
	if (rep[2] && rep[2] != rep[3])
		return (1);
	return (0);
}

char		**msh_tokenize(char *str)
{
	char	**tokens;
	int		i;

	msh_debug_print("msh_tokenize: start str(%s)", str);
	if (!(tokens = quote_strsplit(str, ' ')))
		msh_panic("Memory allocation error in msh_tokenize");
	i = -1;
	while (tokens[++i])
	{
		g_shenv->expand_var = 1;
		while (g_shenv->expand_var && check_token_sub(tokens[i]))
		{
			tokens[i] = msh_expand(tokens[i]);
		}
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
			free(tkns);
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
		free(ln);
		free(dink);
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
	shenv->cmdv = NULL;
	shenv->sl = NULL;
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
	if (g_dbg == 3)
	{
		char *str, *pat, *rep, *ret;
		str = ft_strdup("----$PWD----");
		pat = ft_strdup("$PWD");
		rep = ft_strdup("/nfs/2018/c/callen");
		ret = strsub_rep(str, pat, rep);
		ft_printf("ret(%s)\n", ret);
	}
	else
		ch = msh_repl();
	msh_debug_print("main: end ch(%d)", ch);
	return (ch);
}
