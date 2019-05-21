#ifndef MSH_STRVEC_H
# define MSH_STRVEC_H

# include "msh_command.h"

# include "ft_string.h"
# include "ft_stdlib.h"

# define STRSAV(x) ft_strcpy(malloc(1 + ft_strlen(x)), (x))
# define REVLIST(l,t) ((l&&l->next) ? (t)list_reverse((t_glist*)l):(t)(l))

t_wdtk		*make_bare_word(const char *s);
t_wlst		*make_word_list(t_wdtk *x, t_wlst *l);

typedef struct	s_glist
{
	struct s_glist	*next;
}				t_glist;
typedef struct	s_silst
{
	char	*word;
	int		token;
}				t_silst;

int			list_length(t_glist *lst);
t_glist		*list_reverse(t_glist *list);

int			strvec_len(char **t);
char		**strvec_new(int n);
char		**strvec_resize(char **array, int nsize);
char		**strvec_copy(char **array);
void		strvec_flush(char **array);
void		strvec_dispose(char **array);
int			strvec_remove(char **array, char *name);
int			strvec_strcmp(register char **s1, register char **s2);
char		**strvec_from_word_list(t_wlst *lst, int alloc, int start, int *ip);
t_wlst		*strvec_to_word_list(char **array, int alloc, int start);

#endif
