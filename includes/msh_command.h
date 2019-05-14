/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   msh_command.h                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: callen <callen@student.42.us.org>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/04/16 19:07:27 by callen            #+#    #+#             */
/*   Updated: 2019/05/13 20:46:16 by callen           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MSH_COMMAND_H
# define MSH_COMMAND_H

/*
** TODO: decide whether or not this is a bad idea
*/

/*
** command types
*/

enum	e_cmdtype {cm_for, cm_case, cm_while, cm_if, cm_simple, cm_select,
	cm_connection, cm_function_def, cm_until, cm_group, cm_arith, cm_cond,
	cm_arith_for, cm_subshell, cm_coproc};

/*
** Possible values for the flags field of s_wdtk
*/

#define W_HASDOLLAR		0x00000001
#define W_QUOTED		0x00000002
#define W_ASSIGNMENT	0x00000004
#define W_SPLITSPACE	0x00000008
#define W_NOSPLIT		0x00000010
#define W_NOGLOB		0x00000020
#define W_NOSPLIT2		0x00000040
#define W_TILDEEXP		0x00000080
#define W_DOLLARAT		0x00000100
#define W_DOLLARSTAR	0x00000200
#define W_NOCOMSUB		0x00000400
#define W_ASSIGNRHS		0x00000800
#define W_NOTILDE		0x00001000
#define W_ITILDE		0x00002000
#define W_NOEXPAND		0x00004000
#define W_COMPASSIGN	0x00008000
#define W_ASSNBLTIN		0x00010000
#define W_ASSIGNARG		0x00020000
#define W_HASQUOTEDNULL	0x00040000
#define W_DQUOTE		0x00080000
#define W_NOPROCSUB		0x00100000
#define W_SAWQUOTEDNULL	0x00200000
#define W_ASSIGNASSOC	0x00400000
#define W_ASSIGNARRAY	0x00800000
#define W_ARRAYIND		0x01000000
#define W_ASSNGLOBAL	0x02000000
#define W_NOBRACE		0x04000000
#define W_COMPLETE		0x08000000
#define W_CHKLOCAL		0x10000000
#define W_NOASSNTILDE	0x20000000

/*
** Flags for the pflags arg to param_expand() and other
** param_brace_expand_... functions, as well as string_list_dollar_at
*/

#define PF_NOCOMSUB		0x01
#define PF_IGNUNBOUND	0x02
#define PF_NOSPLIT2		0x04
#define PF_ASSIGNRHS	0x08
#define PF_COMPLETE		0x10

/*
** Possible values for subshell_environment
*/

#define SUBSHELL_ASYNC		0x01
#define SUBSHELL_PAREN		0x02
#define SUBSHELL_COMSUB		0x04
#define SUBSHELL_FORK		0x08
#define SUBSHELL_PIPE		0x10
#define SUBSHELL_PROCSUB	0x20
#define SUBSHELL_COPROC		0x40
#define SUBSHELL_RESETTRAP	0x80

/*
** Structure which represents a word
*/

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
