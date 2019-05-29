/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   msh_command.h                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: callen <callen@student.42.us.org>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/04/16 19:07:27 by callen            #+#    #+#             */
/*   Updated: 2019/05/28 20:17:58 by callen           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MSH_COMMAND_H
# define MSH_COMMAND_H

# include <unistd.h>

/*
** -- Redirections -- TODO
*/

/*
** Instructions describing what kind of thing to do for a redirection.
*/

enum		e_instruction {
	r_output_direction,
	r_input_direction,
	r_inputa_direction,
	r_appending_to,
	r_reading_until,
	r_reading_string,
	r_duplicating_input,
	r_duplicating_output,
	r_deblank_reading_until,
	r_close_this,
	r_err_and_out,
	r_input_output,
	r_output_force,
	r_duplicating_input_word,
	r_duplicating_output_word,
	r_move_input,
	r_move_output,
	r_move_input_word,
	r_move_output_word,
	r_append_err_and_out
};

/*
** Redirection flags; values for rflags
*/

# define REDIR_VARASSIGN		0x01

/*
** Redirection errors.
**
** RESTRICTED_REDIRECT - can only happen in restricted shells.
** HEREDOC_REDIRECT    - here-doc temp file can't be created
** BADVAR_REDIRECT     - something wrong with {varname}redir
*/

# define AMBIGUOUS_REDIRECT  -1
# define NOCLOBBER_REDIRECT  -2
# define RESTRICTED_REDIRECT -3
# define HEREDOC_REDIRECT    -4
# define BADVAR_REDIRECT	    -5

# define CLOBBERING_REDIRECT(i) (i == r_output_direction || i == r_err_and_out)

# define OUT_RDIR_A(i) (i == r_output_direction || i == r_input_output)
# define OUT_RDIR_B(i) (i == r_err_and_out || i == r_append_err_and_out)
# define OUTPUT_REDIRECT(i) (OUT_RDIR_A(i) || OUT_RDIR_B(i))
# undef OUT_RDIR_A
# undef OUT_RDIR_B

# define IN_RDA(i) (i == r_input_direction)
# define IN_RDB(i) (i == r_inputa_direction)
# define IN_RDC(i) (i == r_input_output)
# define INPUT_REDIRECT(i) (IN_RDA(i) || IN_RDB(i) || IN_RDC(i))
# undef IN_RDA
# undef IN_RDB
# undef IN_RDC

# define WR_RDA(i)(i == r_output_direction || i == r_input_output)
# define WR_RDB(i)(i == r_err_and_out || i == r_appending_to)
# define WR_RDC(i)(i == r_append_err_and_out || i == r_output_force)
# define WRITE_REDIRECT(ri) (WR_RDA(ri) || WR_RDB(ri) || WR_RDC(ri))
# undef WR_RDA
# undef WR_RDB
# undef WR_RDC

/*
** redirection needs translation
*/

# define TR_RDA(i)(ri == r_duplicating_input_word)
# define TR_RDB(i)(ri == r_duplicating_output_word)
# define TR_RDC(i)(ri == r_move_input_word || ri == r_move_output_word)
# define TRANSLATE_REDIRECT(ri) (TR_RDA(ri) || TR_RDB(ri) || TR_RDC(ri))
# undef TR_RDA
# undef TR_RDB
# undef TR_RDC

/*
** -- Command types --
*/

enum		e_cmdtype {cm_for, cm_case, cm_while, cm_if, cm_simple, cm_select,
	cm_connection, cm_function_def, cm_until, cm_group, cm_arith, cm_cond,
	cm_arith_for, cm_subshell, cm_coproc};

/*
** Possible values for the flags field of s_wdtk
**
** W_HASDOLLAR     - Dollar sign present.
** W_QUOTED        - Some form of quote character is present.
** W_ASSIGNMENT    - This word is a variable assignment.
** W_SPLITSPACE    - Split this word on " " regardless of IFS
** W_NOSPLIT       - Do not perform word splitting on this word because ifs is
**                   empty string.
** W_NOGLOB        - Do not perform globbing on this word.
** W_NOSPLIT2      - Don't split word except for $@ expansion (using spaces)
**                   because context does not allow it.
** W_TILDEEXP      - Tilde expand this assignment word
** W_DOLLARAT      - $@ and its special handling
** W_DOLLARSTAR    - $* and its special handling
** W_NOCOMSUB      - Don't perform command substitution on this word
** W_ASSIGNRHS     - Word is rhs of an assignment statement
** W_NOTILDE       - Don't perform tilde expansion on this word
** W_ITILDE        - Internal flag for word expansion
** W_NOEXPAND      - Don't expand at all -- do quote removal
** W_COMPASSIGN    - Compound assignment
** W_ASSNBLTIN     - word is a builtin command that takes assignments
** W_ASSIGNARG     - word is assignment argument to command
** W_HASQUOTEDNULL - word contains a quoted null character
** W_DQUOTE        - word should be treated as if double-quoted
** W_NOPROCSUB     - don't perform process substitution
** W_SAWQUOTEDNULL - word contained a quoted null that was removed
** W_ASSIGNASSOC   - word looks like associative array assignment
** W_ASSIGNARRAY   - word looks like a compound indexed array assignment
** W_ARRAYIND      - word is an array index being expanded
** W_ASSNGLOBAL    - word is a global assignment to declare (declare/typeset -g)
** W_NOBRACE       - Don't perform brace expansion
** W_COMPLETE      - word is being expanded for completion
** W_CHKLOCAL      - check for local vars on assignment
** W_NOASSNTILDE   - don't do tilde expansion like an assignment statement
*/

# define W_HASDOLLAR		0x00000001
# define W_QUOTED			0x00000002
# define W_ASSIGNMENT		0x00000004
# define W_SPLITSPACE		0x00000008
# define W_NOSPLIT			0x00000010
# define W_NOGLOB			0x00000020
# define W_NOSPLIT2			0x00000040
# define W_TILDEEXP			0x00000080
# define W_DOLLARAT			0x00000100
# define W_DOLLARSTAR		0x00000200
# define W_NOCOMSUB			0x00000400
# define W_ASSIGNRHS		0x00000800
# define W_NOTILDE			0x00001000
# define W_ITILDE			0x00002000
# define W_NOEXPAND			0x00004000
# define W_COMPASSIGN		0x00008000
# define W_ASSNBLTIN		0x00010000
# define W_ASSIGNARG		0x00020000
# define W_HASQUOTEDNULL	0x00040000
# define W_DQUOTE			0x00080000
# define W_NOPROCSUB		0x00100000
# define W_SAWQUOTEDNULL	0x00200000
# define W_ASSIGNASSOC		0x00400000
# define W_ASSIGNARRAY		0x00800000
# define W_ARRAYIND			0x01000000
# define W_ASSNGLOBAL		0x02000000
# define W_NOBRACE			0x04000000
# define W_COMPLETE			0x08000000
# define W_CHKLOCAL			0x10000000
# define W_NOASSNTILDE		0x20000000

/*
** Flags for the pflags arg to param_expand() and other
** param_brace_expand_... functions, as well as string_list_dollar_at
**
** PF_NOCOMSUB   - Do not perform command substitution
** PF_IGNUNBOUND - ignore unbound vars even if -u set
** PF_NOSPLIT2   - same as W_NOSPLIT2
** PF_ASSIGNRHS  - same as W_ASSIGNRHS
** PF_COMPLETE   - same as W_COMPLETE, sets SX_COMPLETE
*/

# define PF_NOCOMSUB	0x01
# define PF_IGNUNBOUND	0x02
# define PF_NOSPLIT2	0x04
# define PF_ASSIGNRHS	0x08
# define PF_COMPLETE	0x10

/*
** Possible values for subshell_environment
**
** SUBSHELL_ASYNC     - subshell caused by `command &'
** SUBSHELL_PAREN     - subshell caused by ( ... )
** SUBSHELL_COMSUB    - subshell caused by `command` or $(command)
** SUBSHELL_FORK      - subshell caused by executing a disk command
** SUBSHELL_PIPE      - subshell from a pipeline element
** SUBSHELL_PROCSUB   - subshell caused by <(command) or >(command)
** SUBSHELL_COPROC    - subshell from a coproc pipeline
** SUBSHELL_RESETTRAP - subshell needs to reset trap strs on first call to trap
*/

# define SUBSHELL_ASYNC		0x01
# define SUBSHELL_PAREN		0x02
# define SUBSHELL_COMSUB	0x04
# define SUBSHELL_FORK		0x08
# define SUBSHELL_PIPE		0x10
# define SUBSHELL_PROCSUB	0x20
# define SUBSHELL_COPROC	0x40
# define SUBSHELL_RESETTRAP	0x80

/*
** struct s_wdtk : Structure which represents a word
**
** word  - Zero terminated string.
** flags - Flags associated with this word.
*/

typedef struct s_wdtk	t_wdtk;
struct	s_wdtk
{
	char	*word;
	int		flags;
};

/*
** struct s_wlst : A linked list of words
*/

typedef struct s_wlst	t_wlst;
struct	s_wlst
{
	t_wlst	*next;
	t_wdtk	*word;
};

/*
** -- Shell Command Structs -- TODO
*/

/*
** What a redirection descriptor looks like.  If the redirection instruction
** is ri_duplicating_input or ri_duplicating_output, use DEST, otherwise
** use the file in FILENAME.  Out-of-range descriptors are identified by a
** negative DEST.
**
** dest - Place to redirect t_redir to, or ...
** filename - filename to redirect to.
*/

typedef union
{
	int		dest;
	t_wdtk	*filename;
}				t_rdtgt;

/*
** Structure describing a redirection.  If t_redir is negative, the parser
** (or translator in redir.c) encountered an out-of-range file descriptor.
**
** next - Next element, or NULL.
** redirector - Descriptor or varname to be redirected.
** rflags - Private flags for this redirection
** flags - Flag value for 'open'.
** ins - What to do with the information.
** redirectee - File descriptor or filename
** here_doc_eof - The word that appeared in <<foo.
*/

typedef struct	s_redirect
{
	struct s_redirect	*next;
	t_rdtgt				redirector;
	int					rflags;
	int flags;
	enum e_instruction ins;
	t_rdtgt redirectee;
	char *here_doc_eof;
}				t_redir;

/*
** An element used in parsing.  A single word or a single redirection.
** This is an ephemeral construct.
*/

typedef struct	s_element
{
	t_wdtk *word;
	t_redir *redirect;
}				t_rdelm;

/*
** Possible values for command->flags.
**
** CMD_WANT_SUBSHELL     - User wants a subshell: ( command )
** CMD_FORCE_SUBSHELL    - Shell needs to force a subshell.
** CMD_INVERT_RETURN     - Invert the exit value.
** CMD_IGNORE_RETURN     - Ignore the exit value.  For set -e.
** CMD_NO_FUNCTIONS      - Ignore functions during command lookup.
** CMD_INHIBIT_EXPANSION - Do not expand the command words.
** CMD_NO_FORK           - Don't fork; just call execve
** CMD_TIME_PIPELINE     - Time a pipeline
** CMD_TIME_POSIX        - time -p; use POSIX.2 time output spec.
** CMD_AMPERSAND         - command &
** CMD_STDIN_REDIR       - async command needs implicit </dev/null
** CMD_COMMAND_BUILTIN   - command executed by `command' builtin
** CMD_STDPATH           - use standard path for command lookup
*/

# define CMD_WANT_SUBSHELL     0x0001
# define CMD_FORCE_SUBSHELL    0x0002
# define CMD_INVERT_RETURN     0x0004
# define CMD_IGNORE_RETURN     0x0008
# define CMD_NO_FUNCTIONS      0x0010
# define CMD_INHIBIT_EXPANSION 0x0020
# define CMD_NO_FORK           0x0040
# define CMD_TIME_PIPELINE     0x0080
# define CMD_TIME_POSIX        0x0100
# define CMD_AMPERSAND         0x0200
# define CMD_STDIN_REDIR       0x0400
# define CMD_COMMAND_BUILTIN   0x0800
# define CMD_COPROC_SUBSHELL   0x1000
# define CMD_LASTPIPE          0x2000
# define CMD_STDPATH           0x4000

typedef union	u_cmd_val
{
		struct t_for_com		*For;
		struct t_case_com		*Case;
		struct t_while_com		*While;
		struct t_if_com			*If;
		struct t_connection		*Connection;
		struct t_simple_com		*Simple;
		struct t_function_def	*Function_def;
		struct t_group_com		*Group;
# ifdef SELECT_COMMAND
		struct t_select_com		*Select;
# endif
# ifdef DPAREN_ARITHMETIC
		struct t_arith_com		*Arith;
# endif
# ifdef COND_COMMAND
		struct t_cond_com		*Cond;
# endif
# ifdef ARITH_FOR_COMMAND
		struct t_arith_for_com	*ArithFor;
# endif
		struct t_subshell_com	*Subshell;
		struct t_coproc_com		*Coproc;
}				t_cmd_val;

/*
** struct s_command : What a command looks like.
**
** type      - FOR CASE WHILE IF CONNECTION or SIMPLE.
** flags     - Flags controlling execution environment.
** line      - line number the command starts on
** redirects - Special redirects for FOR CASE, etc.
*/

typedef struct	s_command
{
	enum e_cmdtype	type;
	int				flags;
	int				line;
	t_redir			*redirects;
	t_cmd_val		value;
}				t_cmd;

/*
** Structure used to represent the CONNECTION type.
**
** ignore    - Unused; simplifies make_command ().
** *first    - Pointer to the first command.
** *second   - Pointer to the second command.
** connector - What separates this command from others.
*/

typedef struct	s_connection
{
	int		ignore;
	t_cmd	*first;
	t_cmd	*second;
	int		connector;
}				t_connection;

/*
** Structures used to represent the CASE command.
*/

/*
** Values for FLAGS word in a PATTERN_LIST
*/

# define CASEPAT_FALLTHROUGH 0x01
# define CASEPAT_TESTNEXT    0x02

/*
** Pattern/action structure for CASE_COM.
**
** next     - Clause to try in case this one failed.
** patterns - Linked list of patterns to test.
** action   - Thing to execute if a pattern matches.
*/

typedef struct	s_pattern_list
{
	struct s_pattern_list	*next;
	t_wlst					*patterns;
	t_cmd					*action;
	int						flags;
}				t_patlst;

/*
** The CASE command.
**
** flags   - See description of CMD flags.
** line    - line number the `case' keyword appears on
** word    - The thing to test.
** clauses - The clauses to test against, or NULL.
*/

typedef struct	s_case_com
{
	int			flags;
	int			line;
	t_wdtk		*word;
	t_patlst	*clauses;
}				t_case_com;

/*
** FOR command.
**
** flags    - See description of CMD flags.
** line     - line number the 'for' keyword appears on
** name     - The variable name to get mapped over.
** map_list - The things to map over.  This is never NULL.
** action   - The action to execute. During execution, NAME is bound to
**            successive members of MAP_LIST.
*/

typedef struct	s_for_com
{
	int		flags;
	int		line;
	t_wdtk	*name;
	t_wlst	*map_list;
	t_cmd	*action;
				
				
}				t_for_com;

# ifdef ARITH_FOR_COMMAND

/*
** ARITH_FOR command.
**
** line - generally used for error messages
*/

typedef struct	s_arith_for_com
{
	int		flags;
	int		line;
	t_wlst	*init;
	t_wlst	*test;
	t_wlst	*step;
	t_cmd	*action;
}				t_arith_for_com;

# endif

# ifdef SELECT_t_cmd

/*
** KSH SELECT command.
**
** flags    - See description of CMD flags.
** line     - line number the `select' keyword appears on
** name     - The variable name to get mapped over.
** map_list - The things to map over.  This is never NULL.
** action   - The action to execute. During execution, NAME is bound to the
**            member of MAP_LIST chosen by the user.
*/

typedef struct	s_select_com
{
	int		flags;
	int		line;
	t_wdtk	*name;
	t_wlst	*map_list;
	t_cmd	*action;	
}				t_select_com;

/*
** SELECT_COMMAND
*/

# endif

/*
** IF command.
**
** flags      - See description of CMD flags.
** test       - Thing to test.
** true_case  - What to do if the test returned non-zero.
** false_case - What to do if the test returned zero.
*/

typedef struct	s_if_com {
	int		flags;
	t_cmd	*test;
	t_cmd	*true_case;
	t_cmd	*false_case;
}				t_if_com;

/*
** WHILE command.
**
** flags  - See description of CMD flags.
** test   - Thing to test.
** action - Thing to do while test is non-zero.
*/

typedef struct	s_while_com
{
	int		flags;
	t_cmd	*test;
	t_cmd	*action;
}				t_while_com;

# ifdef DPAREN_ARITHMETIC

/*
** The arithmetic evaluation command, ((...)).  Just a set of flags and
** a t_wlst, of which the first element is the only one used, for the
** time being.
*/

typedef struct	s_arith_com
{
	int flags;
	int line;
	t_wlst *exp;
}				t_arith_com;

/*
** DPAREN_ARITHMETIC
*/

# endif

/*
** The conditional command, [[...]].  This is a binary tree -- we slipped
** a recursive-descent parser into the YACC grammar to parse it.
*/

# define COND_AND		1
# define COND_OR		2
# define COND_UNARY		3
# define COND_BINARY	4
# define COND_TERM		5
# define COND_EXPR		6

typedef struct	s_cond_com
{
	int					flags;
	int					line;
	int					type;
	t_wdtk				*op;
	struct s_cond_com	*left;
	struct s_cond_com	*right;
}				t_cond_com;

/*
** The "simple" command.  Just a collection of words and redirects.
**
** flags     - See description of CMD flags.
** line      - line number the command starts on
** words     - The program name, the arguments, variable assignments, etc.
** redirects - Redirections to perform.
*/

typedef struct	s_simple_com
{
	int		flags;
	int		line;
	t_wlst	*words;
	t_redir	*redirects;
}				t_simple_com;

/*
** The "function definition" command.
**
** flags       - See description of CMD flags.
** line        - Line number the function def starts on.
** name        - The name of the function.
** command     - The parsed execution tree.
** source_file - file in which function was defined, if any
*/

typedef struct	s_function_def
{
	int		flags;
	int		line;
	t_wdtk	*name;
	t_cmd	*command;
	char	*source_file;
}				t_fndef;

/*
** A command that is 'grouped' allows pipes and redirections to affect all
** commands in the group.
**
** ignore - See description of CMD flags.
*/

typedef struct	s_group_com
{
	int		ignore;
	t_cmd	*command;
}				t_group_com;

typedef struct	s_subshell_com
{
	int		flags;
	int		line;
	t_cmd	*command;
}				t_subshell_com;

# define COPROC_RUNNING	0x01
# define COPROC_DEAD	0x02

typedef struct	s_coproc
{
	char	*c_name;
	pid_t	c_pid;
	int		c_rfd;
	int		c_wfd;
	int		c_rsave;
	int		c_wsave;
	int		c_flags;
	int		c_status;
	int		c_lock;
}				t_coproc;

typedef struct	s_coproc_com
{
	int		flags;
	char	*name;
	t_cmd	*command;
}				t_coproc_com;

extern t_cmd	*global_command;
extern t_coproc	sh_coproc;

/*
** Possible command errors
*/

# define CMDERR_DEFAULT	0
# define CMDERR_BADTYPE	1
# define CMDERR_BADCONN	2
# define CMDERR_BADJUMP	3

# define CMDERR_LAST	3

/*
** Forward declarations of functions declared in copy_cmd.c.
*/

extern t_fndef	*copy_fndef_contents(t_fndef *d, t_fndef *f);
extern t_fndef	*copy_fndef(t_fndef *f);

extern t_wdtk	*copy_word(t_wdtk *w);
extern t_wlst	*copy_word_list(t_wlst *l);
extern t_redir	*copy_redirect(t_redir *r);
extern t_redir	*copy_redirects(t_redir *r);
extern t_cmd	*copy_command(t_cmd *c);

#endif
