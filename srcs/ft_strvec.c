#include "msh_strvec.h"
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
		if (!(ret[i] = ft_strdup(array[i])))
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
		free(array[i]);
		i++;
	}
}

void		strvec_dispose(char **array)
{
	if (array == 0)
		return ;
	strvec_flush(array);
	free(array);
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
			free(x);
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
