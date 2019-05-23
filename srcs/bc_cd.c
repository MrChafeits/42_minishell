#include "minishell.h"
#include "msh_variables.h"
#include "ft_stdio.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <limits.h>

#ifndef MP_DOTILDE /* lib/sh/makepath.c */
# define MP_DOTILDE 0x01
# define MP_DOCWD 0x02
# define MP_RMDOT 0x04
# define MP_IGNDOT 0x08
#endif
/* lib/sh/pathcanon.c */
#define PATH_CHECKDOTDOT 0x0001
#define PATH_CHECKEXISTS 0x0002
#define PATH_HARDPATH 0x0004
#define PATH_NOALLOC 0x0008
/* lib/sh/pathphys.c */
#define DOUBLE_SLASH(p) ((p[0] == '/') && (p[1] == '/') && (p[2] != '/'))
/* lib/sh/stringlist.c */
//TODO

extern const char *const g_msh_getcwd_errstr;

/* global variables */
char		*g_the_current_working_directory = NULL;
int			g_nolinks = 0;
int			g_posixly_correct = 0;
int			g_o_xattr = 1;
static int	g_xattrfd = -1;

/* Clean up the O_XATTR baggage. Currently only closes xattrfd */
static void	resetxattr(void)
{
	if (g_o_xattr)
		if (g_xattrfd >= 0)
			close(g_xattrfd);
	g_xattrfd = -1;
}

/* Just set $PWD, don't change OLDPWD. Used by 'pwd -P' in posix mode. * /
static int	g_xattrflag;
static int	setpwd(char *dirname)
{
	int			old_anm;
	t_var		*tvar;

	old_anm = g_array_needs_making;
	tvar = bind_variable("PWD", dirname ? dirname : "", 0);
	if (tvar && READONLY_P(tvar))
		return (1);
	if (tvar && old_anm == 0 && g_array_needs_making && EXPORTED_P(tvar))
	{
		update_export_env_inplace("PWD=", 4, dirname ? dirname : "");
		g_array_needs_making = 0;
	}
	return (0);
}*/

char		*get_working_directory(const char *for_whom)
{
	if (g_nolinks)
	{
		free(g_the_current_working_directory);
		g_the_current_working_directory = NULL;
	}
	if (g_the_current_working_directory == 0)
	{
		g_the_current_working_directory = getcwd(0, 0);
		if (g_the_current_working_directory == 0)
		{
			ft_dprintf(2, "%s: error retrieving current directory: %s: %s\n",
				(for_whom && *for_whom) ? for_whom : "get_name_for_error()",
				"bash_getcwd_errstr", "did the bad");
			return (NULL);
		}
	}
	return (SAVESTR(g_the_current_working_directory));
}

void		set_working_directory(char *name)
{
	FREE(g_the_current_working_directory);
	g_the_current_working_directory = SAVESTR(name);
}

int			absolute_pathname(const char *str)
{
	if (!str || *str == '\0')
		return (0);
	if (ABSPATH(str))
		return (1);
	if (str[0] == '.' && PATHSEP(str[1]))
		return (1);
	if (str[0] == '.' && str[1] == '.' && PATHSEP(str[2]))
		return (1);
	return (0);
}
#define MAKEDOT() xpath = malloc(2);*xpath = '.';xpath[1] = 0;pathlen = 1;
#define MAXSYMLINKS 32
static char *g_nullpath = "";

static int	path_readlink(char *path, char *buf, int bufsiz)
{
	return (readlink(path, buf, bufsiz));
}

/* Return 1 if PATH corresponds to a directory. A function for debugging. */
static int	path_isdir(char *path)
{
	int			l;
	struct stat	sb;

	/* This should leave g_errno set to the correct value. */
	g_errno = 0;
	l = stat(path, &sb) == 0 && S_ISDIR(sb.st_mode);
	return (l);
}

char		*sh_physpath(char *path, int flags)
{
	char	tbuf[PATH_MAX + 1];
	char	linkbuf[PATH_MAX + 1];
	char	*result;
	char	*p;
	char	*q;
	char	*qsave;
	char	*qbase;
	char	*workpath;
	int		double_slash_path;
	int		linklen;
	int		nlink;

	(void)flags;
	linklen = ft_strlen(path);
	nlink = 0;
	q = malloc(PATH_MAX + 1);
	result = q;
	if (linklen >= PATH_MAX)
		workpath = SAVESTR(path);
	else
	{
		workpath = malloc(PATH_MAX + 1);
		ft_strcpy(workpath, path);
	}
	qbase = workpath + 1;
	double_slash_path = DOUBLE_SLASH(workpath);
	qbase += double_slash_path;
	p = workpath;
	while (p < qbase)
		*q++ = *p++;
	qbase = q;
	while (*p)
	{
		if (ISDIRSEP(p[0])) /* null element */
			p++;
		else if (p[0] == '.' && PATHSEP(p[1])) /* . and ./ */
			p += 1; /* don't count the separator in case it is null */
		else if (p[0] == '.' && p[1] == '.' && PATHSEP(p[2])) /* .. and ../ */
		{
			p += 2; /* skip '..' */
			if (q > qbase)
			{
				while (--q > qbase && ISDIRSEP(*q) == 0)
					;
			}
		}
		else
		{
			qsave = q;
			if (q != qbase)
				*q++ = DIRSEP;
			while (*p && (ISDIRSEP(*p) == 0))
			{
				if (q - result >= PATH_MAX)
				{
					g_errno = 63; /* ENAMETOOLONG */
					goto error; //TODO: delet
				}
				*q++ = *p++;
			}
			*q = '\0';
			linklen = path_readlink(result, linkbuf, PATH_MAX);
			if (linklen < 0) /* if g_errno == EINVAL, it's not a symlink */
			{
				if (g_errno != 22)
					goto error; //TODO: delet
				continue ;
			}
			/* it's a symlink, and the value is in LINKBUF */
			nlink++;
			if (nlink > MAXSYMLINKS)
			{
				g_errno = 62; /* ELOOP */
			error:
				free(result);
				free(workpath);
				return (NULL);
			}
			linkbuf[linklen] = '\0';
			if ((ft_strlen(p) + linklen + 2) >= PATH_MAX)
			{
				g_errno = 63; /* ENAMETOOLONG */
				goto error; //TODO: delet
			}
			ft_strcpy(tbuf, linkbuf);
			tbuf[linklen] = '/';
			ft_strcpy(tbuf + linklen, p);
			ft_strcpy(workpath, tbuf);
			if (ABSPATH(linkbuf))
			{
				q = result;
				qbase = workpath + 1;
				double_slash_path = DOUBLE_SLASH(workpath);
				qbase += double_slash_path;
				p = workpath;
				while (p < qbase)
					*q++ = *p++;
				qbase = q;
			}
			else
			{
				p = workpath;
				q = qsave;
			}
		}
	}
	*q = '\0';
	free(workpath);
	if (DOUBLE_SLASH(result) && double_slash_path == 0)
	{
		if (result[2] == '\0')
			result[1] = '\0';
		else
			ft_memmove(result, result + 1, ft_strlen(result + 1) + 1);
	}
	return (result);
}

char		*sh_canonpath(char *path, int flags)
{
	char	stub_char;
	char	*result;
	char	*p;
	char	*q;
	char	*base;
	char	*dotdot;
	int		rooted;
	int		double_slash_path;

	/* The result cannot be larger than the input PATH. */
	result = (flags & PATH_NOALLOC) ? path : SAVESTR(path);
	/* POSIX.2 says to leave a leading '//' alone. */
	if ((rooted = ROOTEDPATH(path)))
	{
		stub_char = DIRSEP;
		base = result + 1;
		double_slash_path = DOUBLE_SLASH(path);
		base += double_slash_path;
	}
	else
	{
		stub_char = '.';
		base = result;
		double_slash_path = 0;
	}
	/* invariants: */
	dotdot = base;
	q = dotdot;
	p = q;
	while (*p)
	{
		if (ISDIRSEP(p[0])) /* null element */
			p++;
		else if (p[0] == '.' && PATHSEP(p[1])) /* . and ./ */
			p += 1; /* don't count the separator in case it is null */
		else if (p[0] == '.' && p[1] == '.' && PATHSEP(p[2])) /* .. and ../ */
		{
			p += 2; /* skip '..' */
			if (q > dotdot) /* can backtrack */
			{
				if (flags * PATH_CHECKDOTDOT)
				{
					char c;//y u do dis

					c = *q;
					*q = '\0';
					if (path_isdir(result) == 0)
					{
						if ((flags & PATH_NOALLOC) == 0)
							free(result);
						return (NULL);
					}
					*q = c;
				}
				while (--q > dotdot && ISDIRSEP(*q) == 0)
					;
			}
			else if (rooted == 0)
			{
				/* /.. is / but ./../ is .. */
				if (q != base)
					*q++ = DIRSEP;
				*q++ = '.';
				*q++ = '.';
				dotdot = q;
			}
		}
		else /* real path element */
		{
			/* add separator if not at start of work portion of result */
			if (q != base)
				*q++ = DIRSEP;
			while (*p && (ISDIRSEP(*p) == 0))
				*q++ = *p++;
			/* Check here for a valid directory with path_isdir */
			if (flags & PATH_CHECKEXISTS)
			{
				char c; //y tho bb

				/* Make sure what we have so far corresponds to a valid path
				 * before we chop some of it off. */
				c = *q;
				*q = '\0';
				if (path_isdir(result) == 0)
				{
					if ((flags & PATH_NOALLOC) == 0)
						free(result);
					return (NULL);
				}
				*q = c;
			}
		}
	}
	/* Empty string is really '.' or '/' depending on what we started with. */
	if (q == result)
		*q++ = stub_char;
	*q = '\0';
	/* If the result starts with '//', but the original path doesn't we can
	 * turn the // into /. Because of how we set 'base', this should never be
	 * true, but it's a sanit check. */
	if (DOUBLE_SLASH(result) && double_slash_path == 0)
	{
		if (result[2] == '\0') /* short-circuit for bare '//' */
			result[1] = '\0';
		else
			ft_memmove(result, result + 1, ft_strlen(result + 1) + 1);
	}
	return (result);
}

char		*sh_makepath(const char *path, const char *dir, int flags)
{
	int		dirlen;
	int		pathlen;
	char	*ret;
	char	*xpath;
	char	*xdir;
	char	*r;
	char	*s;

	if (path == 0 || *path == '\0')
	{
		if (flags & MP_DOCWD)
		{
			if (!(xpath = get_working_directory("sh_makepath")))
				if ((ret = get_string_value("PWD")))
					xpath = SAVESTR(ret);
			if (!xpath)
			{
				MAKEDOT();
			}
			else
				pathlen = ft_strlen(xpath);
		}
		else
		{
			MAKEDOT();
		}
	}
	else if ((flags & MP_IGNDOT) && path[0] == '.' && (path[1] == 0 ||
		(path[1] == '/' && path[2] == 0)))
	{
		xpath = g_nullpath;
		pathlen = 0;
	}
	else
	{
		xpath = (char*)path;
		pathlen = ft_strlen(xpath);
	}
	xdir = (char*)dir;
	dirlen = ft_strlen(xdir);
	if ((flags & MP_RMDOT) && dir[0] == '.' && dir[1] == '/')
	{
		xdir += 2;
		dirlen -= 2;
	}
	r = malloc(2 + dirlen + pathlen);
	ret = r;
	s = xpath;
	while (*s)
		*r++ = *s++;
	if (s > xpath && s[-1] != '/')
		*r++ = '/';
	s = xdir;
	while ((*r++ = *s++))
		;
	if (xpath != path && xpath != g_nullpath)
		free(xpath);
	return (ret);
}

char		*make_absolute(const char *str, const char *dot_path)
{
	char *res;

	if (!dot_path || ABSPATH(str))
		res = SAVESTR(str);
	else
		res = sh_makepath(dot_path, str, 0);
	return (res);
}

char		*get_string_value(const char *var)
{
	register int	i;
	int				vl;
	char			*ret;

	i = -1;
	ret = NULL;
	vl = ft_strlen(var);
	while (!ret && ++i < g_shenv->envplen)
		if (ft_strnequ(g_shenv->envp[i], var, vl))
			ret = ft_strdup(ft_strchr(g_shenv->envp[i], '=') + 1);
	return (ret);
}

static char	*resetpwd(char *caller)
{
	char *tdir;

	FREE(g_the_current_working_directory);
	g_the_current_working_directory = NULL;
	tdir = get_working_directory(caller);
	return (tdir);
}

static int	change_to_directory(char *newdir, int nolinks, int xattr)
{
	char	*t;
	char	*tdir;
	int		err;
	int		canon_failed;
	int		r;
	int		ndlen;

	(void)xattr;
	tdir = NULL;
	if (g_the_current_working_directory == 0)
	{
		t = get_working_directory("chdir");
		FREE(t);
	}
	t = make_absolute(newdir, g_the_current_working_directory);
	tdir = nolinks ? sh_physpath(t, 0)
		: sh_canonpath(t, PATH_CHECKDOTDOT|PATH_CHECKEXISTS);
	ndlen = ft_strlen(newdir);
	canon_failed = 0;
	if (tdir && *tdir)
		free(t);
	else
	{
		FREE(tdir);
		tdir = t;
		canon_failed = 1;
	}
	if (g_posixly_correct && nolinks == 0 && canon_failed && ndlen > PATH_MAX)
	{
		g_errno = 20; /* ENOTDIR */
		free(tdir);
		return (0);
	}
	r = chdir(nolinks ? newdir : tdir);
	if (r >= 0)
		resetxattr();
	if (r == 0)
	{
		if (canon_failed)
		{
			t = resetpwd("cd");
			if (t == 0)
				set_working_directory(tdir);
			else
				free(t);
		}
		else
			set_working_directory(tdir);
		free(tdir);
		return (1);
	}
	if (nolinks)
	{
		free(tdir);
		return (0);
	}
	err = g_errno;
	if (chdir(newdir) == 0)
	{
		t = resetpwd("cd");
		if (t == 0)
			set_working_directory(tdir);
		else
			free(t);
		r = 1;
	}
	else
	{
		g_errno = err;
		r = 0;
	}
	free(tdir);
	return (r);
}

/* static int	bindpwd(int links) */
/* { */
/* 	(void)links; */
/* 	return (0); */
/* } */

/* #define LCD_DOVARS 0x001 */
/* #define LCD_DOSPELL 0x002 */
/* #define LCD_PRINTPATH 0x004 */
/* #define LCD_FREEDIRNAME 0x008 */

/* int			cd_blabbo_builtin(t_wlst *list) */
/* { */
/* 	char	*dirname; */
/* 	char	*cdpath; */
/* 	char	*path; */
/* 	char	*temp; */
/* 	int		path_index; */
/* 	int		no_symlinks; */
/* 	int		opt; */
/* 	int		lflag; */
/* 	int		e; */

/* 	g_dbg?ft_dprintf(2,"[DBG:cd_builtin:start[%d](%s)]\n",g_shenv->cmdc,*g_shenv->cmdv):0; */
/* 	no_symlinks = g_nolinks; */
/* 	/\* list = loptend; *\/ */
/* 	/\* lflag = (cdable_vars ? LCD_DOVARS : 0) | *\/ */
/* 	/\* 	((interactive && cdspelling) ? LCD_DOSPELL : 0); *\/ */
/* 	/\* if (g_eflag && no_symlinks == 0) *\/ */
/* 	/\* 	g_eflag = 0; *\/ */
/* 	if (list == 0) */
/* 	g_xattrflag = 0; */
/* 	if (g_shenv->cmdc == 1) */
/* 		dirname = get_string_value("HOME"); */
/* 	else if (g_shenv->cmdc == 2 && ft_strequ("-", g_shenv->cmdv[1])) */
/* 		dirname = get_string_value("OLDPWD"); */
/* 	else */
/* 		dirname = g_shenv->cmdv[1]; */
/* 	g_dbg ? ft_dprintf(2, "[DBG: cd_builtin: dirname(%s)]\n",dirname) : 0; */
/* 	if (change_to_directory(dirname, no_symlinks, g_xattrflag)) */
/* 	{ */
/* 		return (bindpwd(no_symlinks)); */
/* 	} */
/* 	if (lflag & LCD_DOVARS) */
/* 	{ */
/* 		temp = get_string_value(dirname); */
/* 		if (temp && change_to_directory(temp, no_symlinks, g_xattrflag)) */
/* 		{ */
/* 			ft_printf("%s\n", temp); */
/* 			return (bindpwd(no_symlinks)); */
/* 		} */
/* 	} */
/* 	g_dbg ? ft_dprintf(2, "[DBG: cd_builtin: end]\n") : 0; */
/* } */
//TODO: update $PWD and $OLDPWD when cd_builtin is called
int			folderoni(char *path)
{
	int		r;
	char	*cdpath;
	char	*cur;

	cur = ft_strnew(4096);
	getcwd(cur, 4096);
	if (!(r = 0) && path && path[0] == '~')
	{
		cdpath = ft_strjoin_free(g_shenv->home, path + 1, 'R');
		path = cdpath;
		path = ft_strcat(cur, path);
	}
	else if (!path)
		path = g_shenv->home;
	if ((r = access(path, F_OK)) == -1)
		ft_dprintf(2, "minishell: cd: %s: No such file or directory\n", path);
	else if ((r = access(path, R_OK)) == -1)
		ft_dprintf(2, "minishell: cd: %s: Permission denied\n", path);
	else
		chdir(path);
	g_shenv->sl = ft_strdup(cur);
	free(cur);
	return (r);
}

void		word_list_head_flush(t_wlst *list)
{
	if (list == 0 || list->word == 0)
		return ;
	FREE(list->word->word);
	FREE(list->word);
	FREE(list);
}

#define DELN (w[0])
#define TEMP (w[1])
#define PREV (w[2])

void		unbind_var_value(char *name, int len)
{
	t_wlst	*w[4];

	if (!name)
		return ;
	PREV = NULL;
	DELN = NULL;
	len = ft_strlen(name);
	TEMP = g_shenv->list;
	while (TEMP && TEMP->word)
	{
		if (ft_strnequ(name, TEMP->word->word, len))
		{
			if (PREV)
				PREV->next = TEMP->next;
			else
				g_shenv->list = TEMP->next;
			DELN = TEMP;
			break ;
		}
		PREV = TEMP;
		TEMP = TEMP->next;
	}
	word_list_head_flush(DELN);
	strvec_dispose(g_shenv->envp);
	g_shenv->envp = strvec_from_word_list(g_shenv->list, 1, 0, 0);
}

#undef DELN
#undef TEMP
#undef PREV

int			strvec_search(char **array, char *name)
{
	register int	i;
	int				nl;

	if (!array || !name)
		return (-1);
	nl = ft_strlen(name);
	i = -1;
	while (array[++i])
		if (ft_strnequ(name, array[i], nl))
			return (i);
	return (-1);
}

int			word_list_search(t_wlst *list, char *name)
{
	register t_wlst	*t;
	register int	i;
	int				nl;

	if (list == 0 || name == 0)
		return (-1);
	i = 0;
	t = list;
	nl = ft_strlen(name);
	while (t)
	{
		if (ft_strnequ(t->word->word, name, nl))
			return (i);
		i++;
		t = t->next;
	}
	return (-1);
}

void		bind_var_value(char *name, char *value, int alloc)
{
	t_wdtk	*new;
	t_wlst	*tmp;
	char	*var;
	/* char	**h; */

	g_dbg ? ft_dprintf(2, "[DBG: bind_var_value: start(%s)(%s)]\n",name,value) : 0;
	if (value != NULL && word_list_search(g_shenv->list, name) >= 0)
		unbind_var_value(name, 0);
	var = ft_strjoin_free(name, "=", alloc == 1 || alloc == 3 ? 'L' : 0); //XXX
	var = ft_strjoin_free(var, value, alloc >= 2 ? ('L' + 'R') : 'L'); //XXX - potential issue
	g_dbg ? ft_dprintf(2, "[DBG: bind_var_value: var(%s)]\n",var) : 0;
	new = make_bare_word(var);
	FREE(var);
	tmp = REVLIST(g_shenv->list, t_wlst*);
	tmp = make_word_list(new, tmp);
	g_shenv->list = REVLIST(tmp, t_wlst*);
	g_dbg ? ft_dprintf(2, "[DBG: bind_var_value: dispose envp]\n") : 0;
	strvec_dispose(g_shenv->envp);
	g_dbg ? ft_dprintf(2, "[DBG: bind_var_value: new envp]\n") : 0;
	g_shenv->envp = strvec_from_word_list(g_shenv->list, 1, 0, 0);
	g_dbg ? ft_dprintf(2, "[DBG: bind_var_value: end]\n") : 0;
	/* strvec_dispose(h); */
}

void		cd_builtin(t_shenv *e)
{
	char	*dirname;
	int		old;

	g_dbg ? ft_dprintf(2, "[DBG: cd_builtin: start[%d](%s)]\n", e->cmdc, *e->cmdv) : 0;
	if (e->cmdc == 1)
		dirname = get_string_value("HOME");
	else if (e->cmdc == 2 && ft_strequ("-", e->cmdv[1]))
	{
		dirname = get_string_value("OLDPWD");
		ft_printf("%s\n", dirname);
	}
	else
		dirname = ft_strdup(e->cmdv[1]);
	g_dbg ? ft_dprintf(2, "[DBG: cd_builtin: dirname(%s)]\n", dirname) : 0;
	old = folderoni(dirname);
	if (old < 0)
	{
		bind_var_value("OLDPWD", g_shenv->sl, 2);
		bind_var_value("PWD", dirname, 2);
	}
	free(dirname);
	free(g_shenv->sl);
	g_dbg ? ft_dprintf(2, "[DBG: cd_builtin: end]\n") : 0;
}

void		pwd_builtin(t_shenv *e)
{
	g_dbg ? ft_dprintf(2, "[DBG: pwd_builtin: start(%s)]\n", *e->cmdv) : 0;
	change_to_directory(0, 0, 0);
}
