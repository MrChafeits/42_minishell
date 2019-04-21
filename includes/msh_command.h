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
** TODO: this
*/

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
