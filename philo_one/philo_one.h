#ifndef PHILOSOPHERS_PHILO_ONE_H
# define PHILOSOPHERS_PHILO_ONE_H
# include "../philosophers.h"

typedef struct	s_box
{
	t_settings 		*settings;
	pthread_mutex_t *left_fork;
	pthread_mutex_t *right_fork;
	struct timeval	time;
	struct timezone	timezone;
	int				num;
}				t_box;
#endif
