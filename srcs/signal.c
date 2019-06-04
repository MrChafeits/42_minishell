/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signal.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: callen <callen@student.42.us.org>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/06/01 00:38:27 by callen            #+#    #+#             */
/*   Updated: 2019/06/03 18:53:26 by callen           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void		msh_sigint(int sig)
{
	g_shenv->signal_recv = 1;
	if (sig == SIGINT)
	{
		write(1, "\n", 1);
		g_shenv->prompt_printed = 0;
		if (!g_shenv->path_ex && !g_shenv->pwd_ex)
			msh_print_prompt();
		else
			g_shenv->path_ex = 0;
		signal(SIGINT, msh_sigint);
	}
}
