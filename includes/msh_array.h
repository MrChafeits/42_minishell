#ifndef MSH_ARRAY_H
# define MSH_ARRAY_H

# include "msh_variables.h"

/* */
extern int	g_assoc_expand_once;

/* */
extern int	g_array_expand_once;

/* Flags for array_value_internal and callers array_value/get_array_value */
# define AV_ALLOWALL 0x001
# define AV_QUOTED 0x002
# define AV_USEIND 0x004
# define AV_USEVAL 0x008 /* XXX - should move this */
# define AV_ASSIGNRHS 0x010 /* no splitting, special case ${a[@]} */
# define AV_NOEXPAND 0x020 /* don't run assoc subscripts thru word expansion */

/* Flags for valid_array_reference. Value 1 is reserved for skipsubscript() */
# define VA_NOEXPAND 0x001
# define VA_ONEWORD 0x002

extern t_var	*assign_array_element(char *n, char *v, int f);

extern int		valid_array_reference(const char *n, int f);

#endif
