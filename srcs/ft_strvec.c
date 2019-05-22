#include "msh_strvec.h"
#include "ft_stdio.h"
//Array.c
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

//Generic List

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

// Strvec
#ifdef FREE
# undef FREE
#endif
#define FREE(x) {if (x) free(x);}
#ifdef STRDUP
# undef STRDUP
#endif
#define STRDUP(x) ((x) ? SAVESTR(x) : (char*)NULL)
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
	return (ft_realloc(array, strvec_len(array), nsize * sizeof(char*)));
}

char		**strvec_copy(char **array)
{
	register int	i;
	int				len;
	char			**ret;

	len = strvec_len(array);
	if (!(ret = malloc(sizeof(char*) * (len + 1))))
		return (NULL);
	i = -1;
	while (++i < len)
		if (!(ret[i] = STRDUP(array[i])))
			return (NULL);
	return (ret);
}

void		strvec_flush(char **array)
{
	register int i;

	if (!array)
		return ;
	i = 0;
	while (array[i])
	{
		FREE(array[i]);
		i++;
	}
}

void		strvec_dispose(char **array)
{
	if (array == 0)
		return ;
	strvec_flush(array);
	FREE(array);
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
			FREE(x);
			return (1);
		}
	return (0);
}

int			strvec_nremove(char **array, char *name)
{
	register int	i;
	register int	j;
	int				nl;
	char			*x;

	if (array == 0)
		return (0);
	nl = ft_strlen(name);
	i = -1;
	while (array[++i])
		if (ft_strnequ(name, array[i], nl))
		{
			x = array[i];
			j = i - 1;
			while (array[++j])
				array[j] = array[j + 1];
			FREE(x);
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
		arr[count] = alloc ? STRSAV(lst->word->word) : lst->word->word;
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
	i = start;
	list = NULL;
	while (i < count)
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

/*   stringlist.c   */
#ifdef FREE
# undef FREE
#endif
#define FREE(x) {if (x) free(x);}
#ifdef STRDUP
# undef STRDUP
#endif
#define STRDUP(x) ((x) ? SAVESTR(x) : (char*)NULL)

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
	register int i;

	if (sl == 0)
		return (sl = strlist_new(n));
	if (n > sl->list_size)
	{
		sl->list = strvec_resize(sl->list, n + 1);
		i = sl->list_size - 1;
		while (++i <= n)
			sl->list[i] = NULL;
		sl->list_size = n;
	}
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
	FREE(sl);
}

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

int			strlist_nremove(t_strlst *sl, char *s)
{
	int r;

	if (sl == 0 || sl->list == 0 || sl->list_len == 0)
		return (0);
	r = strvec_nremove(sl->list, s);
	if (r)
		sl->list_len--;
	return (r);
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
		FREE(sl->list[i]);
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

void		strlist_sort(t_strlst *sl)
{
	if (sl == 0 || sl->list_len == 0 || sl->list == 0)
		return ;
	//TODO: strvec_sort(sl->list);
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
