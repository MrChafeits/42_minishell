#include "minishell.h"
#include "ft_stdio.h"
#include <sys/types.h>
#include <sys/stat.h>

#ifndef MP_DOTILDE
# define MP_DOTILDE 0x01
# define MP_DOCWD 0x02
# define MP_RMDOT 0x04
# define MP_IGNDOT 0x08
#endif

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

static char *g_nullpath = "";

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
	while (!ret && i < g_shenv->envplen)
		if (ft_strnequ(g_shenv->envp[i], var, vl))
			ret = ft_strdup(ft_strchr(g_shenv->envp[i], '=') + 1);
	return (ret);
}
int g_nolinks = 0;
static int	change_to_directory(const char *newdir)
{
	(void)newdir;
	return (1);
}

void		cd_builtin(t_shenv *e)
{
	char	*dirname;
	/* char	*cdpath; */
	/* char	*path; */
	/* char	*tmp; */
	/* int		path_index; */
	g_dbg ? ft_dprintf(2, "[DBG: cd_builtin: start[%d](%s)]\n",e->cmdc,*e->cmdv) : 0;
	if (e->cmdc == 1)
		dirname = get_string_value("HOME");
	else if (e->cmdc == 2 && ft_strequ("-", e->cmdv[1]))
		dirname = get_string_value("OLDPWD");
	else
		dirname = e->cmdv[1];
	g_dbg ? ft_dprintf(2, "[DBG: cd_builtin: dirname(%s)]\n",dirname) : 0;
	if (change_to_directory(dirname))
	{
	}
	g_dbg ? ft_dprintf(2, "[DBG: cd_builtin: end]\n") : 0;
}

void		pwd_builtin(t_shenv *e)
{
	g_dbg ? ft_dprintf(2, "[DBG: pwd_builtin: start(%s)]\n", *e->cmdv) : 0;
}
