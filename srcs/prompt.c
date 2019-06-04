/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   prompt.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: callen <callen@student.42.us.org>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/06/01 01:19:51 by callen            #+#    #+#             */
/*   Updated: 2019/06/03 17:24:27 by callen           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "ft_stdio.h"

void		msh_print_prompt(void)
{
	char	*buf;
	char	*tmp;
	int		is_home;

	is_home = 0;
	buf = getcwd(0, 0);
	tmp = get_string_value("HOME");
	if (!g_shenv->prompt_printed)
	{
		if (tmp && buf)
		{
			is_home = ft_strequ(buf, tmp);
			free(tmp);
		}
		if (is_home)
			ft_printf("~ msh$ ");
		else if (is_home == 0 && buf)
			ft_printf("%s msh$ ", ft_strrchr(buf, '/') + !ft_strequ(buf, "/"));
		else
			ft_printf("msh$ ");
	}
	free(buf);
	g_shenv->prompt_printed = 1;
	g_shenv->pwd_ex = 0;
	g_shenv->path_ex = 0;
}

char		*msh_readline(void)
{
	char	*ln;

	msh_debug_print("msh_readline: start");
	if (!g_shenv->prompt_printed || !g_shenv->signal_recv)
		if (g_shenv->tty_input && g_shenv->tty_output)
			msh_print_prompt();
	g_shenv->signal_recv = 0;
	if (get_next_line(STDIN_FILENO, &ln) != 1)
	{
		g_shenv->exit_called = -1;
		exit_builtin(g_shenv);
	}
	if (ln && *ln == 0)
		g_shenv->prompt_printed = 0;
	return (ln);
}
