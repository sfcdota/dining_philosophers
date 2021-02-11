/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo_two.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cbach <cbach@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/02/11 13:10:21 by cbach             #+#    #+#             */
/*   Updated: 2021/02/11 13:10:29 by cbach            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PHILOSOPHERS_PHILO_TWO_H
# define PHILOSOPHERS_PHILO_TWO_H
# include "philosophers_two.h"
#include <semaphore.h>
#include <fcntl.h>

typedef struct	s_ph
{
	t_settings 		*settings;
	long long		start;
	int				num;
	long long		temp_time;
	int				thread_started;
	sem_t			*forks;
	sem_t			*dead;
	long long 		eat_count;
}				t_ph;
int		print_return(int retval, char *msg, int msg_l, sem_t *out);
int		print_timestamp(int num, char *msg, int msg_l, t_ph *ph);
#endif
