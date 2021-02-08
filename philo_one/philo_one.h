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
	pthread_mutex_t *eat;
}				t_box;
int		print_return(int retval, char *msg, int msg_l, pthread_mutex_t *out);
int		print_timestamp(int num, char *msg, int msg_l, t_settings *settings);
#endif
