/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bc_unsetenv.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: callen <callen@student.42.us.org>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/05/30 14:27:46 by callen            #+#    #+#             */
/*   Updated: 2019/06/01 13:12:39 by callen           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "msh_strvec.h"
#include "ft_string.h"

void		unsetenv_builtin(t_shenv *e)
{
	int		r;

	if (!e || e->cmdc != 2 || !e->cmdv[1] || !e->envlst)
		return ;
	r = strlist_nremove(e->envlst, e->cmdv[1], ft_strlen(e->cmdv[1]));
}

/*
** msh_debug_print("unsetenv: start");
** msh_debug_print("unsetenv: cmdv[1](%s)", e->cmdv[1]);
** msh_debug_print("unsetenv: end");
*/
