/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   word_list_init.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: callen <callen@student.42.us.org>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/06/03 00:30:31 by callen            #+#    #+#             */
/*   Updated: 2019/06/03 00:31:28 by callen           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "msh_strvec.h"

/*
** array.c 2 functions
*/

t_wdtk		*make_bare_word(const char *s)
{
	t_wdtk *w;

	w = malloc(sizeof(*w));
	w->word = s ? STRSAV(s) : STRSAV("");
	w->flags = 0;
	return (w);
}

t_wlst		*make_word_list(t_wdtk *x, t_wlst *l)
{
	t_wlst *w;

	w = malloc(sizeof(*w));
	w->word = x;
	w->next = l;
	return (w);
}
