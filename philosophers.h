#ifndef PHILOSOPHERS_PHILOSOPHERS_H
# define PHILOSOPHERS_PHILOSOPHERS_H
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/time.h>
#include <stdlib.h>

typedef struct		s_settings
{
	int				p_count;
	int				t_die;
	int				t_eat;
	int				t_sleep;
	int				e_count;
	pthread_mutex_t	output_mutex;
	time_t			time;
	int				dead;
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
#endif
