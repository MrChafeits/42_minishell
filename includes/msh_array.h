/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   msh_array.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: callen <callen@student.42.us.org>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/05/27 23:21:13 by callen            #+#    #+#             */
/*   Updated: 2019/05/28 14:08:02 by callen           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MSH_ARRAY_H
# define MSH_ARRAY_H

# include <stdint.h>
# include "msh_command.h"
# include "msh_variables.h"

typedef intmax_t	t_arind;

enum	e_atype {array_indexed, array_assoc};

typedef struct		s_array
{
	enum e_atype	type;
	t_arind			max_index;
	int				num_elements;
	struct s_arrelm	*lastref;
	struct s_arrelm	*head;
}					t_array;

typedef struct		s_arrelm
{
	t_arind			ind;
	char			*value;
	struct s_arelem	*next;
	struct s_arelem	*prev;
}					t_arrelm;

typedef int			t_shaemapfunc(t_arrelm *a, void *d);

/*
** Basic operations on entire arrays
*/

extern t_array		*array_create(void);
extern void			array_flush(t_array *a);
extern void			array_dispose(t_array *a);
extern t_array		*array_copy(t_array *a);
extern t_array		*array_slice(t_array *a, t_arrelm *s, t_arrelm *e);
extern void			array_walk(t_array *a, t_shaemapfunc *f, void *p);

extern t_arrelm		*array_shift(t_array *a, int x, int y);
extern int			array_rshift(t_array *a, int r, char *s);
extern t_arrelm		*array_unshift_element(t_array *a);
extern int			array_sift_element(t_array *a, char *s);

extern t_array		*array_quote(t_array *a);
extern t_array		*array_quote_escapes(t_array *a);
extern t_array		*array_dequote(t_array *a);
extern t_array		*array_dequote_escapes(t_array *a);
extern t_array		*array_remove_quoted_nulls(t_array *a);

extern char			*array_subrange(t_array *a, t_arind i[2], int n[2]);
extern char			*array_patsub(t_array *a, char *s, char *r, int f);
extern char			*array_modcase(t_array *a, char *s, int i, int f);

/*
** Basic operations on array elements.
*/

extern t_arrelm		*array_create_element(t_arind i, char *d);
extern t_arrelm		*array_copy_element(t_arrelm *e);
extern void			array_dispose_element(t_arrelm *e);

extern int			array_insert(t_array *a, t_arind i, char *d);
extern t_arrelm		*array_remove(t_array *a, t_arind i);
extern char			*array_reference(t_array *a, t_arind i);

/*
** Converting to and from arrays
*/

extern t_wlst		*array_to_word_list(t_array *a);
extern t_array		*array_from_word_list(t_wlst *l);
extern t_wlst		*array_keys_to_word_list(t_array *a);

extern t_array		*array_assign_list(t_array *a, t_wlst *l);

extern char			**array_to_argv(t_array *a);

extern char			*array_to_assign(t_array *a, int f);
extern char			*array_to_string(t_array *a, char *s, int f);
extern t_array		*array_from_string(char *s1, char *s2);

/*
** Flags for array_shift
*/

# define AS_DISPOSE 0x01

# define ARRAY_NUM_ELEMENTS(a) ((a)->num_elements)
# define ARRAY_MAX_INDEX(a) ((a)->max_index)
# define ARRAY_FIRST_INDEX(a) ((a)->head->next->ind)
# define ARRAY_HEAD(a) ((a)->head)
# define ARRAY_EMPTY(a) ((a)->num_elements == 0)

# define ELEMENT_VALUE(ae) ((ae)->value)
# define ELEMENT_INDEX(ae) ((ae)->ind)
# define ELEMENT_FORW(ae) ((ae)->next)
# define ELEMENT_BACK(ae) ((ae)->prev)

# define SET_ELEMENT_VALUE(ae, val) ((ae)->value = (val))

/*
** Convenience
*/

# define ARRAY_PUSH(a,v) {array_rshift((a),1,(v));}
# define ARRAY_POP(a) {array_dispose_element(array_shift((a),1,0));}
# define VARR_GET(v) (((v) && ARRAY_P((v))) ? ARRAY_CELL(v) : (t_array*)0)
# define GET_ARRAY_FROM_VAR(n,v,a) {(v)=find_variable(n);(a)=VARR_GET(v);}
# define ALL_ELEMENT_SUB(c) ((c) == '@' || (c) == '*')

/*
** endif ARRAY_H
*/

#endif
