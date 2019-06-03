/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   strlist_three.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: callen <callen@student.42.us.org>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/06/03 00:47:47 by callen            #+#    #+#             */
/*   Updated: 2019/06/03 01:16:51 by callen           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_stdio.h"
#include "msh_strvec.h"

t_strlst	*strlist_append(t_strlst *m1, t_strlst *m2)
{
	register int	i;
	register int	n;
	register int	len1;
	register int	len2;

	if (m1 == 0)
		return (m2 ? strlist_copy(m2) : NULL);
	len1 = m1->list_len;
	len2 = m2 ? m2->list_len : 0;
	if (len2)
	{
		m1 = strlist_resize(m1, len1 + len2 + 1);
		i = -1;
		n = len1;
		while (++i < len2)
			m1->list[n++] = STRDUP(m2->list[i]);
		m1->list[n] = NULL;
		m1->list_len = n;
	}
	return (m1);
}

#define PLEN (l[0])
#define SLEN (l[1])
#define TLEN (l[2])
#define LLEN (l[3])

t_strlst	*strlist_prefix_suffix(t_strlst *sl, char *prefix, char *suffix)
{
	int		l[4];
	int		i;
	char	*t;

	if (sl == 0 || sl->list == 0 || sl->list_len == 0)
		return (sl);
	PLEN = ft_strlen(prefix);
	SLEN = ft_strlen(suffix);
	if (PLEN == 0 && SLEN == 0)
		return (sl);
	i = -1;
	while (++i < sl->list_len)
	{
		LLEN = ft_strlen(sl->list[i]);
		TLEN = PLEN + LLEN + SLEN + 1;
		t = malloc(TLEN + 1);
		if (PLEN)
			ft_strcpy(t, prefix);
		ft_strcpy(t + PLEN, sl->list[i]);
		if (SLEN)
			ft_strcpy(t + PLEN + LLEN, suffix);
		free(sl->list[i]);
		sl->list[i] = t;
	}
	return (sl);
}

#undef PLEN
#undef SLEN
#undef TLEN
#undef LLEN

void		strlist_print(t_strlst *sl, char *prefix)
{
	register int i;

	if (sl == 0)
		return ;
	i = 0;
	while (i < sl->list_len)
	{
		ft_printf("%s%s\n", prefix ? prefix : "", sl->list[i]);
		i++;
	}
}

void		strlist_walk(t_strlst *sl, t_shstrlstmapfunc *func)
{
	register int i;

	if (sl == 0)
		return ;
	i = 0;
	while (i < sl->list_len)
	{
		if ((*func)(sl->list[i]) < 0)
			break ;
		i++;
	}
}

/*
** TODO: strvec_sort(sl->list);
*/

void		strlist_sort(t_strlst *sl)
{
	if (sl == 0 || sl->list_len == 0 || sl->list == 0)
		return ;
}
