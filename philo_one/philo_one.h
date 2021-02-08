#ifndef PHILOSOPHERS_PHILO_ONE_H
# define PHILOSOPHERS_PHILO_ONE_H
# include "../philosophers.h"

typedef struct	s_box
{
	t_settings 		*settings;
	pthread_mutex_t *left_fork;
	pthread_mutex_t *right_fork;
	long long		start;
	int				num;
	long long		temp_time;
	pthread_mutex_t eating;

}				t_box;
int		print_message(long num1, t_box *box, char *msg, pthread_mutex_t *out);

#endif
