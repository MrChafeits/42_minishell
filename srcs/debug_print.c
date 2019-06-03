/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   debug_print.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: callen <callen@student.42.us.org>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/06/03 00:02:29 by callen            #+#    #+#             */
/*   Updated: 2019/06/03 00:22:04 by callen           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "libft.h"

void		msh_debug_print(char *fmt, ...)
{
	va_list	ap;

	if (g_dbg)
	{
		va_start(ap, fmt);
		write(2, "[DBG: ", 6);
		ft_vdprintf(2, fmt, ap);
		write(2, "]\n", 2);
		va_end(ap);
	}
}
