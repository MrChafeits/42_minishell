/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bc_env.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: callen <callen@student.42.us.org>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/06/01 00:30:43 by callen            #+#    #+#             */
/*   Updated: 2019/06/03 00:19:43 by callen           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "ft_stdio.h"

static void	env_print(t_strlst *env)
{
	register int	i;

	i = 0;
	while (i < env->list_len)
	{
		ft_printf("%s\n", env->list[i]);
		i++;
	}
}

void		env_builtin(t_shenv *e)
{
	if (!e || !e->envlst)
		msh_panic("env_builtin arg *e is NULL, WHAT DID YOU DO?!?");
	env_print(e->envlst);
}

/*
** msh_debug_print("env_builtin: start");
*/
