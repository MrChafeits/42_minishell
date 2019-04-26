/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   msh_command.h                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: callen <callen@student.42.us.org>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/04/16 19:07:27 by callen            #+#    #+#             */
/*   Updated: 2019/04/16 19:07:29 by callen           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MSH_COMMAND_H
# define MSH_COMMAND_H

/*
** TODO: decide whether or not this is a bad idea
*/

typedef struct s_bkt	t_bkt;
struct	s_bkt
{
	t_bkt		*next;
	char		*key;
	void		*data;
	unsigned	khash;
	int			nfnd;
};
typedef struct s_htab	t_htab;
struct	s_htab
{
	t_bkt	**bkt_arr;
	int		nbkts;
	int		nentr;
};
typedef int	t_hfun(t_bkt*);

extern t_htab	*hash_create(int size);
extern t_htab	*hash_copy(t_htab *tbl, void (*f)());
extern void		hash_flush(t_htab *tbl, void (*f)());
extern void		hash_delet(t_htab *tbl);
extern void		hash_walk(t_htab *tbl, t_hfun (*f)());
extern int		hash_bkt(const char *s, t_htab *tbl);
extern int		hash_size(t_htab *tbl);
extern t_bkt	*hash_search(const char *s, t_htab *tbl, int l);
extern t_bkt	*hash_insert(char *s, t_htab *tbl, int l);
extern t_bkt	*hash_remove(const char *s, t_htab *tbl, int l);
extern unsigned	hash_string(const char *s);

#define hash_items(b, t) ((t && (b < t->nbkts)) ? t->bkt_arr[b] : (t_bkt*)NULL)
#define HTAB_DEF 128
#define HTAB_ENT(ht) ((ht) ? (ht)->nentr : 0)
#define HTAB_NOSRCH 0x01
#define HTAB_CREATE 0x02

enum	e_cmdtype {cm_for, cm_case, cm_while, cm_if, cm_simple, cm_select,
	cm_connection, cm_function_def, cm_until, cm_group, cm_arith, cm_cond,
	cm_arith_for, cm_subshell, cm_coproc};

typedef struct s_wdtk	t_wdtk;
struct	s_wdtk
{
	char	*word;
	int		flags;
};
typedef struct s_wlst	t_wlst;
struct	s_wlst
{
	t_wlst	*next;
	t_wdtk	*word;
};

#endif
