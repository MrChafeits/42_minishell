# minishell
Creating a simple, interactive shell following the 42 norme style guide.

# Usage
To get started, you must first clone the repository and compile the code as follows:
```
git clone https://github.com/MrChafeits/42_minishell.git minishell
cd minishell
make
```
Afterwards, the program can be run by typing `./minishell` inside the directory.

# Features
This shell can execute programs that are included in the PATH variable, and it displays an error in case the user types a command that is not recognized. It also manages execution rights in PATH.

The $ and ~ expansions are supported, and the following built-in commands are implemented: `echo`,`cd`,`setenv`,`unsetenv`,`env`,`exit`.

Signals, such as Ctrl-C, are handled, and multiple commands can be separated with `;`.



Please note that it does not currently support pipes, redirections or other advanced functions.





Click [here][1] for more information.

[1]:https://github.com/MrChafeits/42_minishell/blob/master/minishell.en.pdf
