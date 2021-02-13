/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo_one.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cbach <cbach@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/02/11 13:09:51 by cbach             #+#    #+#             */
/*   Updated: 2021/02/11 13:10:29 by cbach            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PHILOSOPHERS_PHILO_ONE_H
# define PHILOSOPHERS_PHILO_ONE_H
# include "philosophers_one.h"

/*
** state: 0 - init value, 1 - want to eat, 2 - eating
*/

typedef struct		s_ph
{
	t_settings		*settings;
	pthread_mutex_t *left_fork;
	pthread_mutex_t *right_fork;
	long long		start;
	int				num;
	long long		temp_time;
	int				thread_started;
	long long		eat_count;
	pthread_mutex_t *eat;
	pthread_mutex_t *dead;
}					t_ph;
int					print_return(int retval, char *msg, int msg_l,
	pthread_mutex_t *out);
int					print_timestamp(int num, char *msg, int msg_l, t_ph *ph);
void				destroy_mutexes(pthread_mutex_t *mutexes,
	t_settings *settings, int index);
int					set_mutexes(pthread_mutex_t *mutexes, t_settings *settings,
	int count);
void				set_philos(t_ph *phs, t_settings *settings,
	pthread_mutex_t *mutexes);
int					set_box(t_ph *boxes, pthread_mutex_t *mutexes,
	t_settings *settings);
int					check_death(t_ph *box);
int					sleep_with_error(long val, char *msg, int msg_l, t_ph *box);
int					take_a_fork(t_ph *box, long start, pthread_mutex_t *fork);
int					unlock_eat(t_ph *box);
int					eat(t_ph *box);
void				*start_simulation(void *arg);
void				monitor(t_ph *phs);
#endif
