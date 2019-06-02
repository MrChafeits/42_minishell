/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bc_cd.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: callen <callen@student.42.us.org>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/05/27 23:05:21 by callen            #+#    #+#             */
/*   Updated: 2019/06/01 22:18:31 by callen           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "msh_variables.h"
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
	char	*cdpath;
	char	*cur;

	cur = getcwd(0, 0);
	r = 0;
	if (!path)
		path = g_shenv->home;
	if ((r = access(path, F_OK)) == -1)
		ft_dprintf(2, "minishell: cd: %s: No such file or directory\n", path);
	else if ((r = access(path, R_OK)) == -1)
		ft_dprintf(2, "minishell: cd: %s: Permission denied\n", path);
	else
		chdir(path);
	if (!r && ft_strstr(path, ".."))
	{
		cdpath = getcwd(0, 0);
		g_shenv->temporoni = ft_strdup(cdpath);
		free(cdpath);
	}
	g_shenv->sl = ft_strdup(cur);
	free(cur);
	return (r);
}

void		bind_var_value(char *name, char *value, int alloc)
{
	char	*var;

	msh_debug_print("bind_var_value: start(%s)(%s)", name, value);
	if (name && strlist_nsearch(g_shenv->envlst, name) >= 0)
		strlist_nremove(g_shenv->envlst, name, ft_strlen(name));
	var = ft_strjoin_free(name, "=", alloc == 1 || alloc == 3 ? 'L' : 0);
	var = ft_strjoin_free(var, value, alloc >= 2 ? ('L' + 'R') : 'L');
	msh_debug_print("bind_var_value: var(%s) value(%s)", var, value);
	strlist_add(g_shenv->envlst, var);
	free(var);
	msh_debug_print("bind_var_value: end");
}

void		cd_builtin(t_shenv *e)
{
	char	*dirname;
	int		old;

	msh_debug_print("cd_builtin: start ac(%d) *av(%s)", e->cmdc, *e->cmdv);
	if (e->cmdc == 1)
		if (!(dirname = get_string_value("HOME")))
			ft_dprintf(2, "minishell: cd: HOME not set\n");
	if (e->cmdc == 2 && ft_strequ("-", e->cmdv[1]))
	{
		dirname = get_string_value("OLDPWD");
		ft_printf("%s\n", dirname);
	}
	else if (e->cmdc >= 2)
		dirname = ft_strdup(e->cmdv[1]);
	msh_debug_print("cd_builtin: dirname(%s)", dirname);
	dirname ? old = folderoni(dirname) : 0;
	if (dirname && !old)
	{
		msh_debug_print("cd_builtin: oldpwd dirname(%s) tmp(%s)", dirname, e->temporoni);
		bind_var_value("OLDPWD", e->sl, 2);
		if (!g_shenv->temporoni)
		{
			msh_debug_print("cd_builtin: pwdd dirname(%s) tmp(%s)", dirname, e->temporoni);
			bind_var_value("PWD", dirname, 2);
		}
		else
		{
			msh_debug_print("cd_builtin: pwdt dirname(%s) tmp(%s)", dirname, e->temporoni);
			bind_var_value("PWD", e->temporoni, 4);
			free(dirname);
		}
	}
	else
		free(dirname);
	msh_debug_print("cd_builtin: end");
}

/*
**
**void		pwd_builtin(t_shenv *e)
**{
**	msh_debug_print("pwd_builtin: start(%s)", *e->cmdv);
**}
*/
