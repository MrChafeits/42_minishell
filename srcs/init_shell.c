/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init_shell.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: callen <callen@student.42.us.org>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/06/03 13:38:59 by callen            #+#    #+#             */
/*   Updated: 2019/06/03 17:45:48 by callen           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	round_to_pow2(int n)
{
	register int	i;

	i = 1;
	while (i <= n)
		i <<= 1;
	return (i);
}

static char	*copy_env_var(const char *var)
{
	char	*ret;

	if (ft_strnequ(var, "SHLVL=", 6))
	{
		ret = ft_itoa(ft_atoi(var + 6) + 1);
		ret = ft_strjoin_free("SHLVL=", ret, 'R');
	}
	else
		ret = ft_strdup(var);
	return (ret);
}

#define DP00 "PATH="
#define DP01 DP00"/usr/local/bin:/usr/local/sbin:"
#define DP02 DP01"/usr/bin:/usr/sbin:/bin:/sbin:."
#define DEFAULT_PATH (DP02)

static char	*g_default_env[3] = {
	DEFAULT_PATH,
	"SHLVL=0",
	NULL
};

static void	init_environment_variables(t_shenv *shenv)
{
	char			**env;
	register int	i;

	if (*shenv->m->e == 0)
		env = g_default_env;
	else
		env = shenv->m->e;
	shenv->envlst = strlist_new(round_to_pow2(strvec_len(env)));
	i = -1;
	while (env[++i] && i < shenv->envlst->list_size)
	{
		shenv->envlst->list[i] = copy_env_var(env[i]);
		shenv->envlst->list_len++;
	}
}

#undef DEFAULT_PATH
#undef DP02
#undef DP01
#undef DP00

void		init_shenv(t_shenv *shenv, t_margs *mg)
{
	shenv->m = mg;
	init_environment_variables(shenv);
	shenv->tty_output = isatty(STDOUT_FILENO);
	shenv->tty_input = isatty(STDIN_FILENO);
	shenv->cmdv = NULL;
	shenv->sl = NULL;
	shenv->wid = 0;
	shenv->pwd_ex = 0;
	shenv->st = 0;
	shenv->cmdc = 0;
	shenv->ret = 0;
	shenv->home = NULL;
	shenv->path = NULL;
	shenv->exit_called = 0;
	shenv->prompt_printed = 0;
	shenv->signal_recv = 0;
}
