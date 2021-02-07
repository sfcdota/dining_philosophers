#include "../philosophers.h"

void	ft_putchar_fd(char c, int fd)
{
	write(fd, &c, 1);
}

void	ft_putstr_fd(char *s, int fd)
{
	while (s && *s)
		ft_putchar_fd(*s++, fd);
}

void	ft_putendl_fd(char *s, int fd)
{
	ft_putstr_fd(s, fd);
	ft_putchar_fd('\n', fd);
}

void	ft_putnbr_fd(long n, int fd)
{
	if (n == -9223372036854775807)
		ft_putstr_fd("-9223372036854775807", fd);
	else
	{
		if (n < 0)
		{
			ft_putchar_fd('-', fd);
			n *= -1;
		}
		if (n > 9)
			ft_putnbr_fd(n / 10, fd);
		ft_putchar_fd(n % 10 + 48, fd);
	}
}

int		print_message(long num1, int num2, char *msg, pthread_mutex_t *out)
{
	int ok;

	if (num1 <= 0)
	{
		if (num2)
			ft_putendl_fd(msg, STDOUT_FILENO);
		return (num2);
	}
	if (!(ok = pthread_mutex_lock(out)))
	{
		ft_putnbr_fd(num1, STDOUT_FILENO);
		ft_putchar_fd(' ', STDOUT_FILENO);
		ft_putnbr_fd(num2, STDOUT_FILENO);
		ft_putchar_fd(' ', STDOUT_FILENO);
		ft_putendl_fd(msg, STDOUT_FILENO);
	}
	if ((ok = (pthread_mutex_unlock(out) || ok)))
		ft_putendl_fd("Error in mutex lock/unlock functions", STDOUT_FILENO);
	return (ok);
}