/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bc_echo.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: callen <callen@student.42.us.org>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/04/22 16:48:43 by callen            #+#    #+#             */
/*   Updated: 2019/06/02 20:20:24 by callen           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "libft.h"

int			echo_builtin_cmd(int argc, char **argv)
{
	char	**ap;
	char	*p;
	char	c;

	ap = argv;
	if (argc)
		ap++;
	while ((p = *ap++) != NULL)
	{
		while ((c = *p++) != '\0')
			ft_putchar(c);
		if (*ap)
			ft_putchar(' ');
	}
	ft_putchar('\n');
	return (0);
}

void		echo_builtin(t_shenv *e)
{
	if (!e)
		return ;
	e->ret = echo_builtin_cmd(e->cmdc, e->cmdv);
}

/*
** msh_debug_print("echo_builtin_cmd: start");
** msh_debug_print("echo_builtin_cmd: end");
**
** msh_debug_print("echo_builtin: start");
** msh_debug_print("echo_builtin: end");
*/
