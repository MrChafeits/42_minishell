/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: callen <callen@student.42.us.org>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/04/12 14:56:58 by callen            #+#    #+#             */
/*   Updated: 2019/04/12 15:06:44 by callen           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "libft.h"

int		main(int argc, char **argv, char **envp)
{
	int i;

	printf("(int) argc = %d\n", argc);
	i = -1;
	printf("(char **) argv = %p\n", argv);
	while (++i < argc)
		printf("(char *) argv[%d] = %s\n", i, argv[i]);
	i = -1;
	printf("(char **) envp = %p\n", envp);
	while (envp[++i])
		printf("(char *) envp[%d] = %s\n", i, envp[i]);
	return (0);
}
