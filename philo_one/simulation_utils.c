/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   simulation_utils.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cbach <cbach@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/02/11 17:31:27 by cbach             #+#    #+#             */
/*   Updated: 2021/02/11 17:31:27 by cbach            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philosophers_one.h"
#include "philo_one.h"

int	check_death(t_ph *box)
{
	if (pthread_mutex_lock(box->dead))
	{
		if (box->settings->remain_phs > 0)
			box->settings->remain_phs = -box->num;
		return (print_return(1, ML, ML_L, &box->settings->output_mutex));
	}
	pthread_mutex_unlock(box->dead);
	return (box->settings->remain_phs <= 0);
}

int	sleep_with_error(long val, char *msg, int msg_l, t_ph *box)
{
	print_timestamp(box->num, msg, msg_l, box);
	if (val && usleep(val * 1000))
		return (print_return(1, SLP, SLP_L, &box->settings->output_mutex));
	return (check_death(box));
}

int	take_a_fork(t_ph *box, long start, pthread_mutex_t *fork)
{
	if (pthread_mutex_lock(fork))
		return (print_return(2, ML, ML_L, &box->settings->output_mutex));
	if (check_death(box))
		return (1);
	return (print_timestamp(box->num, FORK, FORK_L, box));
}

int	forks(t_ph *box)
{
	int status;

	if (pthread_mutex_lock(box->eat))
		return (print_return(1, ML, ML_L, &box->settings->output_mutex));
	if (check_death(box))
		return (pthread_mutex_unlock(box->eat) || 1);
	if ((status = take_a_fork(box, box->start, box->left_fork)))
	{
		pthread_mutex_unlock(box->eat);
		if (status == 1)
			pthread_mutex_unlock(box->left_fork);
		return (1);
	}
	if ((status = take_a_fork(box, box->start, box->right_fork)))
	{
		pthread_mutex_unlock(box->eat);
		pthread_mutex_unlock(box->left_fork);
		if (status == 1)
			pthread_mutex_unlock(box->right_fork);
		return (1);
	}
	return (0);
}

int	eat(t_ph *box)
{
	int kek;

	if ((kek = forks(box)) || !(box->start = get_time())
		|| sleep_with_error(box->settings->t_eat, EAT, EAT_L, box))
	{
		if (kek)
			return (1);
		kek = pthread_mutex_unlock(box->left_fork) +
			pthread_mutex_unlock(box->right_fork) +
			pthread_mutex_unlock(box->eat);
		if (kek)
			print_return(1, M_UNLOCK, M_UNLOCK_L, &box->settings->output_mutex);
		return (1);
	}
	if (box->eat_count > 0)
		box->eat_count--;
	if (!box->eat_count)
		box->settings->remain_phs--;
	kek = pthread_mutex_unlock(box->left_fork) +
		pthread_mutex_unlock(box->right_fork) + pthread_mutex_unlock(box->eat);
	if (kek)
		print_return(1, M_UNLOCK, M_UNLOCK_L, &box->settings->output_mutex);
	return (kek);
}
