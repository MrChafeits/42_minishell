/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bc_setenv.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: callen <callen@student.42.us.org>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/05/30 14:27:14 by callen            #+#    #+#             */
/*   Updated: 2019/06/01 00:35:27 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "msh_strvec.h"
#include "ft_string.h"

/*
** msh_debug_print("setenv: start l(%d)s(%d)",
** e->envlst->list_len, e->envlst->list_size);
** msh_debug_print("setenv: cmdv[1](%s)", e->cmdv[1]);
** msh_debug_print("setenv: r(%d)", r);
** msh_debug_print("setenv: l(%d)s(%d)",
** e->envlst->list_len, e->envlst->list_size);
** msh_debug_print("setenv: end");
*/

void		setenv_builtin(t_shenv *e)
{
	char	*t;
	int		r;

	
	if (!e || e->cmdc != 2 || !e->cmdv[1] || !e->envlst)
		return ;
	if (ft_strlen(e->cmdv[1]) < 3 || !(t = ft_strchr(e->cmdv[1], '=')))
		return ;
	r = strlist_nremove(e->envlst, e->cmdv[1], t - e->cmdv[1]);
	e->envlst = strlist_add(e->envlst, e->cmdv[1]);
}
