#include "libft.h"
#include "msh_strvec.h"

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
#define VC_HASLOCAL 0x01
#define VC_HASTMPVAR 0x02
#define VC_FUNCENV 0x04 // also function if name != NULL
#define VC_BLTNENV 0x08 // builtin_env
#define VC_TEMPENV 0x10 // temporary_env

#define VC_TEMPFLAGS (VC_FUNCENV|VC_BLTNENV|VC_TEMPENV)

// Accessing macros
#define VC_ISFUNCENV(vc) (((vc)->flags & VC_FUNCENV) != 0)
#define VC_ISBLTNENV(vc) (((vc)->flags & VC_BLTNENV) != 0)
#define VC_ISTEMPENV(vc) (((vc)->flags & VC_TEMPENV) != 0)

#define VC_ISTEMPSCOPE(vc) (((vc)->flags & (VC_TEMPENV|VC_BLTNENV)) != 0)

#define VC_HASLOCALS(vc) (((vc)->flags & VC_HASLOCAL) != 0)
#define VC_HASTMPVARS(vc) (((vc)->flags & VC_HASTMPVAR) != 0)

// What a shell variable looks like
typedef struct s_var *t_shvarvfunc(struct s_var *v);
typedef struct s_var *t_shvarafunc(struct s_var *v, char *a, t_arid i, char *b);

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
}				t_var;

typedef struct	s_vlst
{
	t_var		**list;
	int			list_size; /* allocated size */
	int			list_len; /* current number of entries */
}				t_vlst;

//TODO
