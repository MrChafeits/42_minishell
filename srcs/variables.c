/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   variables.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: callen <callen@student.42.us.org>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/06/01 12:21:51 by callen            #+#    #+#             */
/*   Updated: 2019/06/01 12:30:00 by callen           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static char	*check_special_var(const char *var)
{
	char	*r;

	r = NULL;
	if (ft_strnequ(var, "$$", 2))
		r = ft_itoa(getpid());
	else if (ft_strnequ(var, "$?", 2))
		r = ft_itoa(SHR8(g_shenv->ret));
	msh_debug_print("check_special_var: r(%s)", r);
	return (r);
}

static char	*search_env_var(const char *var)
{
	register int	i;
	int				vl;
	char			*ret;
	char			*tmp;

	if (!var || *var == 0)
		return (NULL);
	i = -1;
	ret = NULL;
	vl = ft_strlen(var);
	while (++i < g_shenv->envlst->list_len)
	{
		tmp = g_shenv->envlst->list[i];
		if ((void*)tmp == NULL)
			break;
		if (ft_strnequ(g_shenv->envlst->list[i], var, vl))
		{
			tmp = ft_strchr(g_shenv->envlst->list[i], '=') + 1;
			ret = ft_strdup(tmp);
			break ;
		}
	}
	return (ret);
}

char		*get_string_value(const char *var)
{
	char			*ret;

	msh_debug_print("get_str_val: start var(%s)", var);
	ret = NULL;
	ret = check_special_var(var);
	if (!ret)
		ret = search_env_var(var);
	msh_debug_print("get_str_val: end ret(%s)", ret);
	return (ret);
}
