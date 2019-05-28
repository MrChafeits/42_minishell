/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   variables.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: callen <callen@student.42.us.org>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/05/27 23:19:04 by callen            #+#    #+#             */
/*   Updated: 2019/05/27 23:19:15 by callen           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"
#include "msh_variables.h"
#include "msh_array.h"

extern char	**g_environ;
extern time_t	g_shell_start_time;

t_varctx	*g_global_variables = NULL;
t_varctx	*g_shell_variables = NULL;
t_htab		*g_shell_functions = NULL;
t_htab		*g_invalid_env = NULL;

/* The current vriable context */
int			g_variable_context = 0;

/* If non-zero, local variables inherit values and attributes from a variable */
int			g_localvar_inherit = 0;

/* If non-zero, calling 'unset' */
int			g_localvar_unset = 0;

/* The set of shell assignments which are made only in the environment for a
 * single command */
t_htab		*g_temporary_env = NULL;

/**/
int			g_tempenv_assign_error;

/**/
char		*dollar_vars[10];
t_wlst		*rest_of_args = NULL;

/* The value of $$. */
pid_t		g_dollar_dollar_pid;

/* Non-zero means that we have to remake EXPORT_ENV. */
int			g_anm = 1; //array_needs_making

/* The number of times MSH has been executed. This is set by init_vars(). */
int			g_shell_level = 0;

/* An array which is passed to commands as their environment. It is manufactured
 * from the union of the initial environment and the shell variables that are
 * marked for export. */
char		**g_export_env = (char**)NULL;
static int	g_export_env_index;
static int	g_export_env_size;

t_var			g_nameref_invalid_value;
static t_var	g_nameref_maxloop_value;

static t_htab	*g_last_table_searched; /* hash_lookup uses this */
static t_varctx	*g_last_context_searched;

/* Some forward declarations */
static void		create_variable_tables(void);

static void		set_machine_vars(void);
static void		set_home_var(void);
static void		set_shell_var(void);
static char		*get_bash_name(void);
static void		init_shell_level(void);
static void		uidset(void);
static void		make_vers_array(void);
/*arrayind_t*/
static t_var	*null_assign(t_var *v, char *n, intmax_t index, char *s);

static void		init_dynamic_vars(void);

static t_var	*bind_invalid_envvar(const char *name, char *value, int flags);

static int		var_sametype(t_var *a, t_var *b);

static t_var	*hash_lookup(const char *n, t_htab *h);
static t_var	*bind_variable_internal(const char *s, char *v, t_htab *h, int i, int j);

static t_var	*find_variable_internal(const char *n, int i);

static t_var	*find_nameref_at_context(t_var *a, t_var *b);
static t_var	*find_variable_nameref_context(t_var *v, t_varctx *c, t_varctx **va);
static t_var	*find_variable_last_nameref_context(t_var *v, t_varctx *c, t_varctx **va);

static t_var	*bind_tempenv_variable(const char *n, char *v);

/* general.c */
int				assignment(const char *string, int flags)
{
	register unsigned char	c;
	register int			newi;
	register int			indx;

	indx = 0;
	c = string[indx];
	if ((legal_variable_starter(c) == 0) && ((flags & 1) == 0 || c != '['))
		return (0);
	while (c = string[indx])
	{
		if (c == '=')
			return (indx);
		if (c == '[')
		{
			newi = skipsubscript(string, indx, (flags & 2) ? 1 : 0);
			/* XXX - why not check for blank subscripts here, if we do in
			 * valid_array_reference? */
			if (string[newi++] != ']')
				return (0);
			if (string[newi] == '+' && string[newi + 1] == '=')
				return (newi + 1);
			return ((string[newi] == '=') ? newi : 0);
		}
		if (c == '+' && string[indx + 1] == '=')
			return (indx + 1);
		if (legal_variable_char(c) == 0)
			return (0);
		indx++;
	}
	return (0);
}

char			**add_or_supercede_exported_var(char *assign, int do_alloc)
{
	register int	i;
	int				equal_offset;

	equal_offset = assignment(assign, 0);
	if (equal_offset == 0)
		return (g_export_env);
	//TODO: ack
	if (assign[equal_offset + 1] == '(' &&
		ft_strnequ(assign + equal_offset + 2, ") {", 3))
		equal_offset += 4;
	i = 0;
	while (i < g_export_env_index)
	{
		if (ft_strnequ(assign, g_export_env[i], equal_offset + 1))
		{
			free(g_export_env[i]);
			g_export_env[i] = do_alloc ? SAVESTR(assign) : assign;
			return (g_export_env);
		}
	}
	add_to_export_env(assign, do_alloc);
	return (g_export_env);
}

void			update_export_env_inplace(char *env_prefix, int preflen, char *value)
{
	char	*evar;

	evar = malloc(ft_strlen(value) + preflen + 1);
	ft_strcpy(evar, env_prefix);
	if (value)
		ft_strcpy(evar + preflen, value);
	g_export_env = add_or_supercede_exported_var(evar, 0);
}

t_var		*bind_global_variable(const char *name, char *value, int flags)
{
	if (g_invalid_env == 0)
		g_invalid_env = hash_create(64);
	return (bind_variable_internal(name, value, g_invalid_env, HASH_NOSRCH, flags));
}

t_var		*bind_variable(const char *name, char *value, int flags)
{
	t_var		*v;
	t_var		*nv;
	t_varctx	*vc;
	t_varctx	*nvc;

	if (g_shell_variables == 0)
		create_variable_tables();
	/**/
	if (g_temporary_env && value) /* XXX - can value be null here? */
		bind_tempenv_variable(name, value);
	/* XXX -- handle local variables here. */
	vc = g_shell_variables;
	while (vc)
	{
		if (VC_ISFUNCENV(vc) || VC_ISBLTNENV(vc))
		{
			v = hash_lookup(name, vc->table);
			nvc = vc;
			if (v && NAMEREF_P(v))
			{
				/**/
				nv = find_variable_nameref_context(v, vc, &nvc);
				if (nv == 0)
				{
					nv = find_variable_last_nameref_context(v, vc, &nvc);
					if (nv && NAMEREF_P(nv))
					{
						/**/
						if (NAMEREF_CELL(nv) == 0)
							return (bind_variable_internal(nv->name, value, nvc->table, 0, flags));
						else if (valid_array_reference(NAMEREF_CELL(nv), 0))
							return (assign_array_element(NAMEREF_CELL(nv), value, flags));
						else
							return (bind_variable_internal(NAMEREF_CELL(nv), value, nvc->table, 0, flags));
					}
					else if (nv == &g_nameref_maxloop_value)//circular name reference
						return (bind_global_variable(v->name, value, flags));
					else
						v = nv;
				}
				else if (nv == &g_nameref_maxloop_value)//circular name reference
				{
					return (bind_global_variable(v->name, value, flags));
				}
				else
					v = nv;
			}
			if (v)
				return (bind_variable_internal(v->name, value, nvc->table, 0, flags));
		}
	}
	/* bind_variable_internal will handle nameref resolution in this case */
	return (bind_variable_internal(name, value, g_global_variables->table, 0, flags));
}
