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

typedef struct	s_ph
{
	t_settings 		*settings;
	pthread_mutex_t *left_fork;
	pthread_mutex_t *right_fork;
	long long		start;
	int				num;
	long long		temp_time;
	int				thread_started;
	long long		eat_count;
	pthread_mutex_t *eat;
	pthread_mutex_t *dead;
}				t_ph;
int		print_return(int retval, char *msg, int msg_l, pthread_mutex_t *out);
int		print_timestamp(int num, char *msg, int msg_l, t_ph *ph);
#endif
