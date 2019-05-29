/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   msh_arrayfunc.h                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: callen <callen@student.42.us.org>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/05/27 23:21:05 by callen            #+#    #+#             */
/*   Updated: 2019/05/28 14:15:52 by callen           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MSH_ARRAYFUNC_H
# define MSH_ARRAYFUNC_H

# ifdef ARRAY_VARS

/*
** Must include msh_variables.h before including this file.
*/

/*
** This variable means to not expand associative array subscripts more than
** once, when performing variable expansion.
*/

extern int	g_assoc_expand_once;

/*
** The analog for indexed array subscripts
*/

extern int	g_array_expand_once;

/*
** Flags for array_value_internal and callers array_value/get_array_value
*/

#  define AV_ALLOWALL 0x001
#  define AV_QUOTED 0x002
#  define AV_USEIND 0x004

/*
** XXX - should move this
*/

#  define AV_USEVAL 0x008

/*
** no splitting, special case ${a[@]}
*/

#  define AV_ASSIGNRHS 0x010

/*
** don't run assoc subscripts thru word expansion
*/

#  define AV_NOEXPAND 0x020

/*
** Flags for valid_array_reference. Value 1 is reserved for skipsubscript()
*/

#  define VA_NOEXPAND 0x001
#  define VA_ONEWORD 0x002

extern t_var	*convert_var_to_array(t_var *v);
extern t_var	*convert_var_to_assoc(t_var *v);

/*
** char *n == n[0]
** char *m == n[1]
*/

extern char		*make_array_var_value(t_var *v, t_arind i, char *n[2], int f);

extern t_var	*bind_array_variable(char *n, t_arind i, char *d, int f);
extern t_var	*bind_array_element(t_var *v, t_arind i, char *d, int f);
extern t_var	*assign_array_element(char *n, char *v, int f);

/*
** char *n == n[0]
** char *i == n[1]
** char *d == n[2]
*/

extern t_var	*bind_assoc_variable(t_var *v, char *n[3], int f);

extern t_var	*find_or_make_array_variable(char *n, int f);

extern t_var	*assign_array_from_string(char *s, char *d, int f);
extern t_var	*assign_array_var_from_word_list(t_var *v, t_wlst *l, int f);

extern t_wlst	*expand_compound_array_assignment(t_var *v, char *n, int f);
extern void		assign_compound_array_list(t_var *v, t_wlst *l, int f);
extern t_var	*assign_array_var_from_string(t_var *v, char *s, int f);

extern int		unbind_array_element(t_var *v, char *s, int f);
extern int		skipsubcript(const char *n, int i, int f);

extern void		print_array_assignment(t_var *v, int f);
extern void		print_assoc_assignment(t_var *v, int f);

extern t_arind	array_expand_index(t_var *v, char *s, int i, int f);
extern int		valid_array_reference(const char *n, int f);
extern char		*array_value(const char *n, int i, int j, int *s, t_arind *idx);
extern char		*get_array_value(const char *n, int i, int *s, t_arind *idx);

extern char		*array_keys(char *n, int f);

extern char		*array_variable_name(const char *n, int f, char **av, int *s);
extern t_var	*array_variable_part(const char *n, int f, char **av, int *s);

# else

#  define AV_ALLOWALL 0
#  define AV_QUOTED 0
#  define AV_USEIND 0
#  define AV_ASSIGNRHS 0

#  define VA_ONEWORD 0

# endif

#endif
