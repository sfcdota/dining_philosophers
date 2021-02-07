#ifndef PHILOSOPHERS_PHILOSOPHERS_H
# define PHILOSOPHERS_PHILOSOPHERS_H
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/time.h>
#include <stdlib.h>

/*
** remain_philos: -1 = there is a dead philo, 0 all philos have eaten e_count times, > 0 all alive and doing their things
*/

typedef struct		s_settings
{
	int				p_count;
	int				t_die;
	int				t_eat;
	int				t_sleep;
	int				e_count;
	pthread_mutex_t	output_mutex;
	pthread_mutex_t *mutexes;
	int				remain_philos;
	struct timeval	timeval;
	struct timezone	timezone;
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
int					print_message(long num1, int num2, char *msg, pthread_mutex_t *out);
void				ft_putendl_fd(char *s, int fd);
#endif
