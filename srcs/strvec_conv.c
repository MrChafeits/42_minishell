/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   strvec_conv.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: callen <callen@student.42.us.org>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/05/27 23:16:24 by callen            #+#    #+#             */
/*   Updated: 2019/06/03 00:50:42 by callen           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "msh_strvec.h"

char		**strvec_from_word_list(t_wlst *lst, int alloc, int start, int *ip)
{
	int		count;
	char	**arr;

	count = list_length((t_glist*)lst);
	arr = malloc((1 + count + start) * sizeof(char*));
	count = -1;
	while (++count < start)
		arr[count] = NULL;
	count = start;
	while (lst)
	{
		arr[count] = alloc ? STRSAV(lst->word->word) : lst->word->word;
		count++;
		lst = lst->next;
	}
	arr[count] = NULL;
	if (ip)
		*ip = count;
	return (arr);
}

t_wlst		*strvec_to_word_list(char **array, int alloc, int start)
{
	t_wlst	*list;
	t_wdtk	*w;
	int		i;
	int		count;

	if ((count = 0) || array == 0 || array[0] == 0)
		return (NULL);
	while (array[count])
		count++;
	i = start - 1;
	list = NULL;
	while (++i < count)
	{
		w = make_bare_word(alloc ? array[i] : "");
		if (alloc == 0)
		{
			free(w->word);
			w->word = array[i];
		}
		list = make_word_list(w, list);
	}
	return (REVLIST(list, t_wlst*));
}

t_strlst	*strlist_from_word_list(t_wlst *list, int alloc, int si, int *ip)
{
	t_strlst	*ret;
	int			slen;
	int			len;

	if (list == 0)
	{
		if (ip)
			*ip = 0;
		return (NULL);
	}
	slen = list_length((t_glist*)list);
	ret = malloc(sizeof(t_strlst));
	ret->list = strvec_from_word_list(list, alloc, si, &len);
	ret->list_size = slen + si;
	ret->list_len = len;
	if (ip)
		*ip = len;
	return (ret);
}

t_wlst		*strlist_to_word_list(t_strlst *sl, int alloc, int si)
{
	t_wlst	*list;

	if (sl == 0 || sl->list == 0)
		return (NULL);
	list = strvec_to_word_list(sl->list, alloc, si);
	return (list);
}
