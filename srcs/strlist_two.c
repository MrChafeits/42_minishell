/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   strlist_two.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: callen <callen@student.42.us.org>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/06/03 00:46:32 by callen            #+#    #+#             */
/*   Updated: 2019/06/03 01:15:49 by callen           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "msh_strvec.h"

int			strlist_remove(t_strlst *sl, char *s)
{
	int r;

	if (sl == 0 || sl->list == 0 || sl->list_len == 0)
		return (0);
	r = strvec_remove(sl->list, s);
	if (r)
		sl->list_len--;
	return (r);
}

int			strlist_nremove(t_strlst *sl, char *s, int nl)
{
	int r;

	if (sl == 0 || sl->list == 0 || sl->list_len == 0)
		return (0);
	r = strvec_nremove(sl->list, s, nl);
	if (r)
		sl->list_len--;
	return (r);
}

int			strlist_nsearch(t_strlst *sl, char *name)
{
	register int	i;
	int				nl;

	if (!sl || !sl->list_len || !name)
		return (-1);
	nl = ft_strlen(name);
	i = -1;
	while (++i < sl->list_len)
		if (ft_strnequ(sl->list[i], name, nl))
			return (i);
	return (-1);
}

t_strlst	*strlist_copy(t_strlst *sl)
{
	t_strlst		*new;
	register int	i;

	if (sl == 0)
		return (NULL);
	new = strlist_new(sl->list_size);
	if (sl->list)
	{
		i = -1;
		while (++i < sl->list_size)
			new->list[i] = STRDUP(sl->list[i]);
	}
	new->list_size = sl->list_size;
	new->list_len = sl->list_len;
	if (new->list)
		new->list[new->list_len] = NULL;
	return (new);
}

t_strlst	*strlist_merge(t_strlst *m1, t_strlst *m2)
{
	t_strlst	*sl;
	int			i;
	int			n;
	int			l1;
	int			l2;

	l1 = m1 ? m1->list_len : 0;
	l2 = m2 ? m2->list_len : 0;
	sl = strlist_new(l1 + l2 + 1);
	i = -1;
	n = 0;
	while (++i < l1)
		sl->list[n++] = STRDUP(m1->list[i]);
	i = -1;
	while (++i < l2)
		sl->list[n++] = STRDUP(m2->list[i]);
	sl->list_len = n;
	sl->list[n] = NULL;
	return (sl);
}
