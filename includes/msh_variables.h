/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   msh_variables.h                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: callen <callen@student.42.us.org>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/05/19 20:03:41 by callen            #+#    #+#             */
/*   Updated: 2019/05/20 22:23:37 by callen           ###   ########.fr       */
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

typedef struct			s_varctx
{
	char			*name; //empty or NULL means global context
	int				scope; // 0 means global context
	int				flags;
	struct s_varctx	*up; //previous function calls
	struct s_varctx	*down;  // down towards global scope
	t_htab			*table; //variables at this scope
}						t_varctx;

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
typedef struct s_var	*t_shvarvfunc(struct s_var *v);
typedef struct s_var	*t_shvarafunc(struct s_var *v, char *a, int i, char *b);

/* For the future */
union					u_val
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

typedef struct			s_var
{
	char			*name; /* Symbol that the user types */
	char			*value; /* Value that is returned */
	char			*exportstr; /* String for the environment */
	t_shvarafunc	*dynval; /* Function called to return value like $SECONDS */
	t_shvarvfunc	*asignf; /* Function called when this special variable is assigned a value in bind_variable */
	int				attr; /* export, readonly, array, invisible... */
	int				ctx; /* Which context this variable belongs to */
}						t_var; /* SHELL_VAR */

typedef struct			s_vlst
{
	t_var		**list;
	int			list_size; /* allocated size */
	int			list_len; /* current number of entries */
}						t_vlst;
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

# define EXPORTED_P(var) ((((var)->attr) & (ATT_EXPORTED)))
# define READONLY_P(var) ((((var)->attr) & (ATT_READONLY)))
# define ARRAY_P(var) ((((var)->attr) & (ATT_ARRAY)))
# define FUNCTION_P(var) ((((var)->attr) & (ATT_FUNCTION)))
# define INTEGER_P(var) ((((var)->attr) & (ATT_INTEGER)))
# define LOCAL_P(var) ((((var)->attr) & (ATT_LOCAL)))
# define ASSOC_P(var) ((((var)->attr) & (ATT_ASSOC)))
# define TRACE_P(var) ((((var)->attr) & (ATT_TRACE)))
# define UPPERCASE_P(var) ((((var)->attr) & (ATT_UPPERCASE)))
# define LOWERCASE_P(var) ((((var)->attr) & (ATT_LOWERCASE)))
# define CAPCASE_P(var) ((((var)->attr) & (ATT_CAPCASE)))
# define NAMEREF_P(var) ((((var)->attr) & (ATT_NAMEREF)))

# define INVISIBLE_P(var) ((((var)->attr) & (ATT_INVISIBLE)))
# define NON_UNSETTABLE_P(var) ((((var)->attr) & (ATT_NOUNSET)))
# define NOASSIGN_P(var) ((((var)->attr) & (ATT_NOASSIGN)))
# define IMPORTED_P(var) ((((var)->attr) & (ATT_IMPORTED)))
# define SPECIALVAR_P(var) ((((var)->attr) & (ATT_SPECIAL)))
# define NOFREE_P(var) ((((var)->attr) & (ATT_NOFREE)))
# define REGEN_P(var) ((((var)->attr) & (ATT_REGENERATE)))

# define TEMPVAR_P(var) ((((var)->attr) & (ATT_TEMPVAR)))
# define PROPAGATE_P(var) ((((var)->attr) & (ATT_PROPAGATE)))

/* Variable names: lvalues */
# define NAME_CELL(var) ((var)->name)

/* Accessing variable values: rvalues */
# define VALUE_CELL(var) ((var)->value)
# define FUNCTION_CELL(var) (t_cmd*)((var)->value)
# define ARRAY_CELL(var) (t_arr*)((var)->value)
# define ASSOC_CELL(var) (t_htab*)((var)->value)
# define NAMEREF_CELL(var) ((var)->value) /* so it can change later */

# define NAMEREF_MAX 8 /* only  levels of nameref indirection */

# define VAR_ISSET(var) ((var)->value != 0)
# define VAR_ISUNSET(var) ((var)->value == 0)
# define VAR_ISNULL(var) ((var)->value && *(var)->value == 0)

/* Assigning variable values: lvalues */
# define VAR_SETVALUE(var, str) ((var)->value = (str))
# define VAR_SETFUNC(var, func) ((var)->value = (char*)(func))
# define VAR_SETARRAY(var, arr) ((var)->value = (char*)(arr))
# define VAR_SETASSOC(var, arr) ((var)->value = (char*)(arr))
# define VAR_SETREF(var, str) ((var)->value = (str))

/* Make VAR be auto-exported */
# define SET_AUTO_EXPORT(var) ((var)->attr |= ATT_EXPORTED; g_anm = 1;)
# define SETVARATTR(v, a, u) ((!u) ? ((v)->attr |= (a)) : ((v)->attr &= ~(a)))
# define VSETATTR(var, attr) ((var)->attr |= (attr))
# define VUNSETATTR(var, attr) ((var)->attr &= ~(attr))
# define VGETFLAGS(var) ((var)->attr)
# define VSETFLAGS(var, flags) ((var)->attr = (flags))
# define VCLRFLAGS(var) ((var)->attr = 0)

/* Macros to perform various operations on 'exportstr' member of a t_var. */
# define CLEAR_EXPORTSTR(var) (var)->exportstr = NULL
# define COPY_EXPORTSTR(var) ((var)->exportstr?SAVESTR((var)->exportstr):NULL)
# define SET_EXPORTSTR(var, val) (var)->exportstr = (val)
# define SAVE_EXPORTSTR(var, val) (var)->exportstr = (val) ? SAVESTR(val):NULL
# define FREE_EXPORTSTR(var) (if((var)->exportstr)free((var)->exportstr);)
# define CACHE_IMPORTSTR(var, val) (var)->exportstr = SAVESTR(val)
# define NULL_EXPSTR(var) (if((var)->exportstr){(var)->exportstr=NULL;})
# define INVAL_EXPORTSTR(var) {FREE_EXPORTSTR(var);NULL_EXPSTR(var);}
# define IFSNAME(s) ((s)[0]=='I'&&(s)[1]=='F'&&(s)[2]=='S'&&(s)[3]==0)
# define MKLOC_INHERIT 0x01

/* Special value for nameref with invalid value for creation or assignment. */
extern t_var			g_nameref_inval_val;
# define INVALID_NAMEREF_VALUE (void*)&g_nameref_inval_val

/* Stuff for hacking variables. */
typedef int				t_shvarmapfunc(t_var*);

/* Where we keep the variables and functions */
extern t_varctx			*g_global_variables;
extern t_varctx			*g_shell_variables;

extern t_htab			*g_shell_functions;
extern t_htab			*g_temporary_env;

extern int				g_variable_context;
extern char				*g_dollar_vars[];
extern char				**g_export_env;

extern int				g_tempenv_assign_error;
extern int				g_array_needs_making;
extern int				g_shell_level;

/* XXX */
extern t_wlst			*g_rest_of_args;
extern pid_t			g_dollar_dollar_pid;

extern void				init_shell_vars(char **v, int n);

extern int				validate_inherited_val(t_var *v, int n);

extern t_var			*set_if_not(char *a, char *b);

extern void				sh_set_lines_and_columns(int y, int x);
extern void				set_pwd(void);
extern void				set_ppid(void);
extern void				make_funcname_visible(int n);

extern t_var			*var_lookup(const char* s, t_varctx *c);

extern t_var			*find_function(const char *s);
//TODO: Holy prototypes my guy
extern t_var			*bind_variable(const char *n, char *v, int f);
void					update_export_env_inplace(char *p, int l, char *v);

#endif
