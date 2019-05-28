/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   quote_str.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: callen <callen@student.42.us.org>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/05/27 22:00:44 by callen            #+#    #+#             */
/*   Updated: 2019/05/27 22:38:10 by callen           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "ft_stdio.h"

int			quote_wordcount(char *s, char c)
{
	register int	total;
	register int	count;
	register int	i;

	if (!s || !*s)
		return (0);
	total = 0;
	count = 0;
	i = 0;
	while (s[i])
	{
		while (s[i] == c)
			i++;
		if (s[i] && s[i] != c && count % 2 == 0)
			total++;
		while (s[i] != c && s[i])
		{
			if (s[i] == '\"')
				count++;
			i++;
		}
	}
	return (total);
}

static int	check_quotes(char *gonk)
{
	register int	i;
	register int	count;

	if (!gonk)
		return (0);
	i = -1;
	count = 0;
	while (gonk[++i])
	{
		if (gonk[i] == '\"')
			count++;
	}
	return (count % 2 == 0);
}

static int	quote_wordlen(char *s, int c)
{
	register int	i;
	register int	l;

	i = 0;
	l = 0;
	while (s[i] == c)
		i++;
	if (s[i] == '\"')
		while (s[i++] && s[i] != '\"')
		{
			l = !l ? l + 2 : l + 1;
		}
	else
		while (s[i] && s[i] != c)
		{
			i++;
			l++;
		}
	return (l + (s[i] == '\"' ? 1 : 0));
}

#ifdef ISQT
# undef ISQT
#endif
#define ISQT(c) ((c) == '\"')
#ifdef QT
# undef QT
#endif
#define QT(c) (ISQT(c) ? 2 : 0)

char		**quote_strsplit(char *s, int c)
{
	register int	i;
	register int	j;
	register int	n;
	register int	k;
	char			**ret;

	n = quote_wordcount(s, c);
	if (!s || !n || !check_quotes(s) || !(ret = malloc((n + 1) * sizeof(*ret))))
		return (0);
	i = -1;
	j = 0;
	k = 0;
	while (++i < n)
	{
		k = quote_wordlen(&s[j], c);
		if (!(ret[i] = ft_strndup(&s[j + ISQT(s[j])], k - QT(s[j]))))
			break ;
		j += quote_wordlen(&s[j], c);
		while (s[j] == c)
			j++;
	}
	ret[i] = 0;
	return (ret);
}

#undef ISQT
#undef QT
