#ifndef PHILOSOPHERS_PHILOSOPHERS_H
# define PHILOSOPHERS_PHILOSOPHERS_H
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/time.h>
#include <stdlib.h>

struct timeval timev;
struct timezone zonev;
/*
** remain_philos: -1 = there is a dead philo, 0 all philos have eaten e_count times, > 0 all alive and doing their things
*/

typedef struct		s_settings
{
	int				p_count;
	long long		t_die;
	long long		t_eat;
	long long		t_sleep;
	long long		e_count;
	pthread_mutex_t	output_mutex;
	pthread_mutex_t *eat_mutexes;
	int				remain_philos;
	unsigned int	start_time;
}					t_settings;

/*
** state: -1 init, 0 = eating, 1 = sleeping, 2 = thinking
** [optional]eat_count: -1 init, 0-[settings.e_count] = count 
*/
typedef struct		s_philo
{
	pthread_mutex_t	*left_fork;
	pthread_mutex_t	*right_fork;
	int				state;
	int				eat_count;
	
}					t_philo;

int					ft_atoi(const char *nptr);
int					get_settings(t_settings *settings, int argc, char **argv);
unsigned int get_time();
int		str_len(char *s);
void	ft_putnbr_fd(long n, int fd);
#endif
