/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   generic_list.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: callen <callen@student.42.us.org>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/06/03 00:31:54 by callen            #+#    #+#             */
/*   Updated: 2019/06/03 00:34:35 by callen           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "msh_strvec.h"

/*
** Generic List list.c 3 functions
*/

int			list_length(t_glist *lst)
{
	register int i;

	i = 0;
	while (lst)
	{
		lst = lst->next;
		i++;
	}
	return (i);
}

t_glist		*list_reverse(t_glist *list)
{
	register t_glist *n;
	register t_glist *p;

	p = 0;
	while (list)
	{
		n = list->next;
		list->next = p;
		p = list;
		list = n;
	}
	return (p);
}

t_glist		*list_append(t_glist *head, t_glist *tail)
{
	register t_glist	*t_head;

	if (head == 0)
		return (tail);
	t_head = head;
	while (t_head->next)
		t_head = t_head->next;
	t_head->next = tail;
	return (head);
}
