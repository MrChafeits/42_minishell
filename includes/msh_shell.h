#ifndef MSH_SHELL_H
# define MSH_SHELL_H

# include <unistd.h>

typedef struct s_fdbmp	t_fdbmp;
struct	s_fdbmp
{
	int		size;
	char	*bmp;
};

# define FDBMP_SIZE 32

# define CTLESC '\001'
# define CTLNUL '\177'

typedef struct s_uinfo	t_uinfo;
struct	s_uinfo
{
	uid_t	uid;
	uid_t	euid;
	gid_t	gid;
	gid_t	egid;
	char	*user_name;
	char	*shell;
	char	*home_dir;
};

#endif
