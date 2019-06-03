/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   strlist_one.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: callen <callen@student.42.us.org>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/06/03 00:45:33 by callen            #+#    #+#             */
/*   Updated: 2019/06/03 01:15:27 by callen           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "msh_strvec.h"

t_strlst	*strlist_new(int n)
{
	t_strlst		*ret;
	register int	i;

	ret = malloc(sizeof(t_strlst));
	if (n)
	{
		ret->list = strvec_new(n + 1);
		ret->list_size = n;
		i = -1;
		while (++i < n)
			ret->list[i] = NULL;
		ret->list[i] = NULL;
	}
	else
	{
		ret->list = NULL;
		ret->list_size = 0;
	}
	ret->list_len = 0;
	return (ret);
}

t_strlst	*strlist_resize(t_strlst *sl, int n)
{
	register int	i;
	char			**tmp;

	if (sl == 0)
		return (sl = strlist_new(n));
	tmp = NULL;
	if (n > sl->list_size)
	{
		tmp = malloc((n + 1) * sizeof(*tmp));
		i = -1;
		while (++i < sl->list_len)
			tmp[i] = sl->list[i];
		i = sl->list_size;
		while (i < n)
		{
			tmp[i] = NULL;
			i++;
		}
		free(sl->list);
		sl->list = tmp;
		sl->list_size = n;
	}
	return (sl);
}

t_strlst	*strlist_add(t_strlst *sl, char *s)
{
	if (sl == 0)
		return (NULL);
	if (sl->list_len + 1 >= sl->list_size)
	{
		sl = strlist_resize(sl, sl->list_size * 2);
	}
	sl->list[sl->list_len] = ft_strdup(s);
	sl->list_len++;
	return (sl);
}

void		strlist_flush(t_strlst *sl)
{
	if (sl == 0 || sl->list == 0)
		return ;
	strvec_flush(sl->list);
	sl->list_len = 0;
}

void		strlist_dispose(t_strlst *sl)
{
	if (sl == 0)
		return ;
	if (sl->list)
		strvec_dispose(sl->list);
	free(sl);
}
