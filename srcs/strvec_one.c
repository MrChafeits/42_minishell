/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   strvec_one.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: callen <callen@student.42.us.org>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/06/03 00:42:09 by callen            #+#    #+#             */
/*   Updated: 2019/06/03 01:22:10 by callen           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "msh_strvec.h"

/*
** Strvec lib/sh/stringvec.c 11 functions
*/

int			strvec_len(char **t)
{
	register int i;

	i = 0;
	while (t && t[i])
		++i;
	return (i);
}

char		**strvec_new(int n)
{
	return (malloc((n) * sizeof(char*)));
}

char		**strvec_resize(char **array, int nsize)
{
	register int	i;
	int				len;
	char			**new;

	len = strvec_len(array);
	if (len < nsize)
		return (array);
	new = strvec_new(nsize + 1);
	i = -1;
	while (++i < len)
		new[i] = array[i];
	i = len - 1;
	while (++i < nsize)
		new[i] = NULL;
	free(array);
	return (new);
}

char		**strvec_copy(char **array)
{
	register int	i;
	int				len;
	char			**ret;

	len = strvec_len(array);
	if ((ret = malloc(sizeof(char*) * (len + 1))) == 0)
		return (NULL);
	i = -1;
	while (++i < len)
		if ((ret[i] = STRDUP(array[i])) == 0)
			return (NULL);
	return (ret);
}

/*
** TODO: remove incredibly hacky bandaid for buffer overflow
*/

#define ABSUB(a,b) ((a) > (b) ? ((a) - (b)) : ((b) - (a)))

void		strvec_flush(char **array)
{
	register int	i;
	uintptr_t		p;

	if (!array)
		return ;
	i = 0;
	p = 0;
	while (array[i] && *array[i])
	{
		if (p == 0 || ABSUB((uintptr_t)(array[i]), p) < (uintptr_t)(array[0]))
		{
			free(array[i]);
		}
		else
			break ;
		if (i > 0)
			p = (uintptr_t)(array[i]);
		i++;
	}
}

#undef ABSUB
