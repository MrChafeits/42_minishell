/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bc_cd.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: callen <callen@student.42.us.org>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/05/27 23:05:21 by callen            #+#    #+#             */
/*   Updated: 2019/06/03 02:09:24 by callen           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "ft_stdio.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <limits.h>

#define ISRELPU(s) (ft_strstr(path, ".."))
#define ISRELPD(s) ((s)[0] != '/' && (ft_strrchr((s), '/')) != 0)
#define ISRELP(s) (ISRELPU(s) || ISRELPD(s))

int			folderoni(char *path)
{
	int		r;
	char	*cur;

	msh_debug_print("folderoni: start path(%s)", path);
	cur = getcwd(0, 0);
	r = 0;
	if ((r = access(path, F_OK)) == -1)
		ft_dprintf(2, "minishell: cd: %s: No such file or directory\n", path);
	else if ((r = access(path, R_OK)) == -1)
		ft_dprintf(2, "minishell: cd: %s: Permission denied\n", path);
	else
	{
		g_shenv->sl = ft_strdup(cur);
		chdir(path);
	}
	g_shenv->ret = (r == -1 ? 256 : 0);
	free(cur);
	msh_debug_print("folderoni: end r(%d)", r);
	return (r);
}

void		bind_var_value(char *name, char *value, int alloc)
{
	char	*var;

	msh_debug_print("bind_var_value: start name(%s) value(%s)", name, value);
	if (name && strlist_nsearch(g_shenv->envlst, name) >= 0)
		strlist_nremove(g_shenv->envlst, name, ft_strlen(name));
	var = ft_strjoin_free(name, "=", alloc == 1 || alloc == 3 ? 'L' : 0);
	var = ft_strjoin_free(var, value, alloc >= 2 ? ('L' + 'R') : 'L');
	strlist_add(g_shenv->envlst, var);
	free(var);
	msh_debug_print("bind_var_value: end");
}

static char	*special_args(t_shenv *e)
{
	char	*dir;

	msh_debug_print("special_args: start e->ret(%d)", e->ret);
	dir = NULL;
	if (e->cmdc == 1)
	{
		if (!(dir = get_string_value("HOME")))
			ft_dprintf(2, "minishell: cd: HOME not set\n");
	}
	if (e->cmdc == 2 && ft_strequ("-", e->cmdv[1]))
	{
		if (!(dir = get_string_value("OLDPWD")))
			ft_dprintf(2, "minishell: cd: OLDPWD not set\n");
	}
	else if (e->cmdc >= 2)
		dir = ft_strdup(e->cmdv[1]);
	e->ret = (dir == 0 ? 256 : 0);
	msh_debug_print("special_args: end e->ret(%d)", e->ret);
	return (dir);
}

void		cd_builtin(t_shenv *e)
{
	char	*dirname;
	int		old;

	msh_debug_print("cd_builtin: start e->ret(%d)", e->ret);
	old = 1;
	dirname = special_args(e);
	dirname ? old = folderoni(dirname) : 0;
	if (dirname && !old)
	{
		msh_debug_print("cd_builtin: oldpwd dirname(%s)", dirname);
		bind_var_value("OLDPWD", e->sl, 2);
		if (!ft_strstr(dirname, "..") && *dirname == '/')
			bind_var_value("PWD", dirname, 2);
		else
		{
			msh_debug_print("cd_builtin: pwdt dirname(%s)", dirname);
			bind_var_value("PWD", getcwd(0, 0), 2);
			free(dirname);
		}
		e->ret = 0;
	}
	else
		free(dirname);
	msh_debug_print("cd_builtin: end e->ret(%d)", e->ret);
}

/*
**void		pwd_builtin(t_shenv *e)
**{
**	msh_debug_print("pwd_builtin: start(%s)", *e->cmdv);
**}
*/
