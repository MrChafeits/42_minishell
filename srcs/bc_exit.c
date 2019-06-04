/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bc_exit.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: callen <callen@student.42.us.org>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/05/30 14:29:30 by callen            #+#    #+#             */
/*   Updated: 2019/06/03 17:32:36 by callen           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "ft_stdio.h"

void		exit_builtin(t_shenv *e)
{
	int		st;

	if (e->cmdc > 1 && e->exit_called != -1)
		st = ft_atoi(e->cmdv[1]);
	else
		st = e->ret;
	e->exit_called = 1;
	ft_dprintf(2, "exit\n");
	exit(st);
}

/*
** msh_debug_print("exit_builtin: start");
** msh_debug_print("exit_builtin: st(%d)", st);
*/
