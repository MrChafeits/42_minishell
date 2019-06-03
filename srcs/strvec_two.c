/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   strvec_two.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: callen <callen@student.42.us.org>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/06/03 00:44:02 by callen            #+#    #+#             */
/*   Updated: 2019/06/03 01:16:20 by callen           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "msh_strvec.h"

void		strvec_dispose(char **array)
{
	if (array == 0)
		return ;
	strvec_flush(array);
	free(array);
}

int			strvec_remove(char **array, char *name)
{
	register int	i;
	register int	j;
	char			*x;

	if (array == 0)
		return (0);
	i = -1;
	while (array[++i])
		if (ft_strequ(name, array[i]))
		{
			x = array[i];
			j = i - 1;
			while (array[++j])
				array[j] = array[j + 1];
			free(x);
			return (1);
		}
	return (0);
}

int			strvec_nremove(char **array, char *name, int nl)
{
	register int	i;
	register int	j;
	char			*x;

	if (array == 0)
		return (0);
	i = -1;
	while (array[++i])
		if (ft_strnequ(name, array[i], nl))
		{
			x = array[i];
			j = i - 1;
			while (array[++j])
				array[j] = array[j + 1];
			free(x);
			return (1);
		}
	return (0);
}

int			strvec_strcmp(register char **s1, register char **s2)
{
	int res;

	if ((res = **s1 - **s2) == 0)
		res = ft_strcmp(*s1, *s2);
	return (res);
}
