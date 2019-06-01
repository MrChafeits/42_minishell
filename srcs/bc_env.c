/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bc_env.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: callen <callen@student.42.us.org>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/06/01 00:30:43 by callen            #+#    #+#             */
/*   Updated: 2019/06/01 00:31:42 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "ft_stdio.h"

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