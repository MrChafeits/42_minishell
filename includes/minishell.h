/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: callen <callen@student.42.us.org>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/04/12 15:08:30 by callen            #+#    #+#             */
/*   Updated: 2019/04/12 17:39:52 by callen           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H

/*
** Allowed Functions:
**    malloc, free
**     - stdlib.h
**    access
**     - unistd.h
**    open, close, read, write
**     - fcntl.h
**    opendir, readdir, closedir
**     - dirent.h
**    getcwd, chdir
**     - unistd.h
**    stat, lstat, fstat
**     - sys/stat.h
**    fork, execve
**     - unistd.h
**    wait, waitpid, wait3, wait4
**     - wait.h
**    signal, kill
**     - signal.h
**    exit
**  For bonuses
**    tcgetattr
**     - termios.h
*/

typedef struct s_mainargs	t_margs;
struct	s_mainargs
{
	int		c;
	char	**v;
	char	**e;
	char	**a;
};

/*
** Mandatory Part:
**  - Create a mini UNIX command interpreter
**  - Must display a prompt (ex: "$> "), wait till a command has been typed and
**    submitted by pressing enter
**  - Redisplay the prompt after the command has been executed
**  - Pipes, redirections, and advanced functions are not required
**  - Must use the PATH variable to locate executables
**  - If an executable can't be found, display an appropriate error message and
**    redisplay the prompt
**  - Must implement errno myself
**  - Deal with PATH and the environment correctly (copy of system char **evp)
**  - Must implement 'echo','cd','setenv','unsetenv','env','exit' as builtins
**  - Must manage $ and ~ expansions
*/

/*
** Bonus Part:
**  - Management of signals, specifically Ctrl-C. Global variables are allowed
**    for this bonus
**  - Management of execution rights in PATH
**  - Auto completion
**  - Separating commands with ";"
**  - Others that I think will be useful
*/

#endif
