/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo_three.h                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cbach <cbach@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/02/11 13:10:06 by cbach             #+#    #+#             */
/*   Updated: 2021/02/11 13:10:29 by cbach            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PHILOSOPHERS_PHILO_THREE_H
# define PHILOSOPHERS_PHILO_THREE_H
# include "philosophers_three.h"
# include <semaphore.h>
# include <fcntl.h>

typedef struct		s_ph
{
	t_settings		*settings;
	long long		start;
	int				num;
	long long		temp_time;
	int				thread_started;
	sem_t			*forks;
	sem_t			*dead;
	long long		eat_count;
	int				is_dead;
	sem_t			*eat;
}					t_ph;
int					print_return(int retval, char *msg, int msg_l, sem_t *out);
int					print_timestamp(int num, char *msg, int msg_l, t_ph *ph);
void				destroy_mutexes(pthread_mutex_t *mutexes,
	t_settings *settings, int index);
int					set_mutexes(pthread_mutex_t *mutexes, t_settings *settings);
void				set_philos(t_ph *phs, t_settings *settings);
int					set_box(t_ph *boxes, t_settings *settings);
int					check_death(t_ph *box);
int					sleep_with_error(long val, char *msg, int msg_l, t_ph *box);
int					take_a_fork(t_ph *box, long start, pthread_mutex_t *fork);
int					unlock_eat(t_ph *box);
long long			eat(t_ph *box);
int					start_simulation(void *arg);
void				*monitor(void *arg);
#endif
