/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bc_cd.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: callen <callen@student.42.us.org>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/05/27 23:05:21 by callen            #+#    #+#             */
/*   Updated: 2019/05/27 23:07:34 by callen           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "msh_variables.h"
#include "ft_stdio.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <limits.h>

char		*get_string_value(const char *var)
{
	register int	i;
	int				vl;
	char			*ret;
	char			*tmp;

	msh_debug_print("get_str_val: start var(%s)", var);
	i = -1;
	ret = NULL;
	vl = ft_strlen(var);
	while (++i < g_shenv->envlst->list_len)
	{
		tmp = g_shenv->envlst->list[i];
		if ((void*)tmp == NULL)
		{
			break;
		}
		if (ft_strnequ(g_shenv->envlst->list[i], var, vl))
		{
			tmp = ft_strchr(g_shenv->envlst->list[i], '=') + 1;
			ret = ft_strdup(tmp);
			break ;
		}
	}
	msh_debug_print("get_str_val: end ret(%s)", ret);
	return (ret);
}

int			folderoni(char *path)
{
	int		r;
	char	*cdpath;
	char	*cur;

	cur = getcwd(0, 0);
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
	char	*var;

	msh_debug_print("bind_var_value: start(%s)(%s)",name,value);
	if (name && strlist_nsearch(g_shenv->envlst, name) >= 0)
		strlist_nremove(g_shenv->envlst, name, ft_strlen(name));
	var = ft_strjoin_free(name, "=", alloc == 1 || alloc == 3 ? 'L' : 0);
	var = ft_strjoin_free(var, value, alloc >= 2 ? ('L' + 'R') : 'L');
	msh_debug_print("bind_var_value: var(%s)",var);
	strlist_add(g_shenv->envlst, var);
	free(var);
	msh_debug_print("bind_var_value: end");
}

void		cd_builtin(t_shenv *e)
{
	char	*dirname;
	int		old;

	msh_debug_print("cd_builtin: start[%d](%s)", e->cmdc, *e->cmdv);
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
		bind_var_value("OLDPWD", g_shenv->sl, 2);
		bind_var_value("PWD", dirname, 2);
	}
	else
		free(dirname);
	msh_debug_print("cd_builtin: end");
}

/*
**void		pwd_builtin(t_shenv *e)
**{
**	msh_debug_print("pwd_builtin: start(%s)", *e->cmdv);
**}
*/
