/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   msh_strvec.h                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: callen <callen@student.42.us.org>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/05/27 23:20:35 by callen            #+#    #+#             */
/*   Updated: 2019/06/03 01:20:07 by callen           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MSH_STRVEC_H
# define MSH_STRVEC_H

# include "msh_command.h"

# include "ft_string.h"
# include "ft_stdlib.h"

# define REVLIST(l,t) ((l && l->next) ? (t)list_reverse((t_glist*)l) : (t)(l))
# define STRSAV(x) ft_strcpy(malloc(1 + ft_strlen(x)), (x))
# define STRDUP(x) ((x) ? ft_strcpy(malloc(1+ft_strlen(x)), (x)) : (char*)NULL)

t_wdtk		*make_bare_word(const char *s);
t_wlst		*make_word_list(t_wdtk *x, t_wlst *l);

typedef struct s_glist	t_glist;
struct		s_glist
{
	struct s_glist	*next;
};

typedef struct s_silst	t_silst;
struct		s_silst
{
	char	*word;
	int		token;
};

int			list_length(t_glist *lst);
t_glist		*list_reverse(t_glist *list);

int			strvec_len(char **t);
char		**strvec_new(int n);
char		**strvec_resize(char **array, int nsize);
char		**strvec_copy(char **array);
void		strvec_flush(char **array);
void		strvec_dispose(char **array);
int			strvec_remove(char **array, char *name);
int			strvec_nremove(char **array, char *name, int nl);
int			strvec_strcmp(register char **s1, register char **s2);
char		**strvec_from_word_list(t_wlst *lst, int al, int st, int *ip);
t_wlst		*strvec_to_word_list(char **array, int al, int st);

/*
** stringlist.c
*/

typedef struct s_strlst	t_strlst;
struct		s_strlst
{
	char	**list;
	int		list_size;
	int		list_len;
};

typedef int	t_shstrlstmapfunc(char *s);

t_strlst	*strlist_new(int size);
t_strlst	*strlist_resize(t_strlst *l, int nsize);
void		strlist_flush(t_strlst *l);
void		strlist_dispose(t_strlst *l);
int			strlist_remove(t_strlst *l, char *name);
int			strlist_nremove(t_strlst *l, char *name, int nl);
int			strlist_nsearch(t_strlst *sl, char *name);
t_strlst	*strlist_add(t_strlst *sl, char *s);
t_strlst	*strlist_copy(t_strlst *l);
t_strlst	*strlist_merge(t_strlst *m1, t_strlst *m2);
t_strlst	*strlist_append(t_strlst *m1, t_strlst *m2);
t_strlst	*strlist_prefix_suffix(t_strlst *l, char *p, char *s);
void		strlist_print(t_strlst *l, char *pr);
void		strlist_walk(t_strlst *l, t_shstrlstmapfunc *fn);

/*
** void		strlist_sort(t_strlst *l);
*/

#endif
