/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   msh_variables.h                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: callen <callen@student.42.us.org>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/05/19 20:03:41 by callen            #+#    #+#             */
/*   Updated: 2019/05/20 13:04:02 by callen           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef VARIABLES_H
# define VARIABLES_H
# include "libft.h"
# include "msh_strvec.h"

//#include "array.h"
//#include "assoc.h"
//#include "hashlib.h"
//#include "conftypes.h"

typedef struct s_varctx
{
	char			*name; //empty or NULL means global context
	int				scope; // 0 means global context
	int				flags;
	struct s_varctx	*up; //previous function calls
	struct s_varctx	*down;  // down towards global scope
	t_htab			*table; //variables at this scope
}				t_varctx;

// flags for t_varctx->flags
# define VC_HASLOCAL 0x01
# define VC_HASTMPVAR 0x02
# define VC_FUNCENV 0x04 // also function if name != NULL
# define VC_BLTNENV 0x08 // builtin_env
# define VC_TEMPENV 0x10 // temporary_env

# define VC_TEMPFLAGS (VC_FUNCENV|VC_BLTNENV|VC_TEMPENV)

// Accessing macros
# define VC_ISFUNCENV(vc) (((vc)->flags & VC_FUNCENV) != 0)
# define VC_ISBLTNENV(vc) (((vc)->flags & VC_BLTNENV) != 0)
# define VC_ISTEMPENV(vc) (((vc)->flags & VC_TEMPENV) != 0)

# define VC_ISTEMPSCOPE(vc) (((vc)->flags & (VC_TEMPENV|VC_BLTNENV)) != 0)

# define VC_HASLOCALS(vc) (((vc)->flags & VC_HASLOCAL) != 0)
# define VC_HASTMPVARS(vc) (((vc)->flags & VC_HASTMPVAR) != 0)

// What a shell variable looks like
typedef struct s_var *t_shvarvfunc(struct s_var *v);
typedef struct s_var *t_shvarafunc(struct s_var *v, char *a, int i, char *b);

/* For the future */
union u_val
{
	char		*s; /* string value */
	intmax_t	i; /* int value */
	//COMMAND *f; /* function */
	//ARRAY *a; /* array */
	t_htab		*h; /* associatve array */
	double		d; /* floating point number */
	long double	ld; /* long double */
	struct var	*v; /* possible indirect variable use */
	void		*o; /* opaque data for future use */
};

typedef struct	s_var
{
	char			*name; /* Symbol that the user types */
	char			*value; /* Value that is returned */
	char			*exportstr; /* String for the environment */
	t_shvarafunc	*dynval; /* Function called to return value like $SECONDS */
	t_shvarvfunc	*asignf; /* Function called when this special variable is assigned a value in bind_variable */
	int				attr; /* export, readonly, array, invisible... */
	int				ctx; /* Which context this variable belongs to */
}				t_var; /* SHELL_VAR */

typedef struct	s_vlst
{
	t_var		**list;
	int			list_size; /* allocated size */
	int			list_len; /* current number of entries */
}				t_vlst;
/* The various attributes that a given variable can have */
/* First, the user-visible attributes */
# define ATT_EXPORTED   0x0000001 /* export to environment */
# define ATT_READONLY   0x0000002 /* cannot change */
# define ATT_ARRAY      0x0000004 /* value is an array */
# define ATT_FUNCTION   0x0000008 /* value is a function */
# define ATT_INTEGER    0x0000010 /* internal representation is int */
# define ATT_LOCAL      0x0000020 /* variable is local to a function */
# define ATT_ASSOC      0x0000040 /* variable is an associative array */
# define ATT_TRACE      0x0000080 /* function is traced with DEBUG trap */
# define ATT_UPCASE     0x0000100 /* word converted to uppercase on assignment */
# define ATT_LOWCASE    0x0000200 /* word converted to lowercase on assignment */
# define ATT_CAPCASE    0x0000400 /* word is capitalized on assignment */
# define ATT_NAMEREF    0x0000800 /* word is a name reference */

# define ATTMASK_USER   0x0000fff

/* Internal attributes used for bookkeeping */
# define ATT_INVISIBLE  0x0001000 /* cannot see */
# define ATT_NOUNSET    0x0002000 /* cannot unset */
# define ATT_NOASSIGN   0x0004000 /* assignment not allowed */
# define ATT_IMPORTED   0x0008000 /* came from environment */
# define ATT_SPECIAL    0x0010000 /* requires special handling */
# define ATT_NOFREE     0x0020000 /* do not free value on unset */
# define ATT_REGENERATE 0x0040000 /* regenerate when exported */

# define ATTMASK_INT    0x00ff000

/* Internal attributes used for variable scoping */
# define ATT_TEMPVAR    0x0100000 /* variable came from the temp environment */
# define ATT_PROPAGATE  0x0200000 /* propagate to previous scope */

# define ATTMASK_SCOPE  0x0f00000
//TODO
#endif
