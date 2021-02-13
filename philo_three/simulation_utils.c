/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   simulation_utils.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cbach <cbach@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/02/11 18:30:46 by cbach             #+#    #+#             */
/*   Updated: 2021/02/11 18:30:46 by cbach            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philosophers_three.h"
#include "philo_three.h"

int			check_death(t_ph *box)
{
	if (sem_wait(box->dead))
		return (print_return(1, ML, ML_L, box->settings->out));
	sem_post(box->dead);
	return (box->settings->remain_phs <= 0);
}

int			sleep_with_error(long val, char *msg, int msg_l, t_ph *box)
{
	print_timestamp(box->num, msg, msg_l, box);
	if (val && usleep(val * 1000))
		return (print_return(1, SLP, SLP_L, box->settings->out));
	return (check_death(box));
}

int			take_fork(t_ph *box)
{
	if (sem_wait(box->forks))
	{
		if (box->settings->remain_phs > 0)
			box->settings->remain_phs = -box->num;
		return (print_return(1, ML, ML_L,
			box->settings->out));
	}
	if (check_death(box))
	{
		sem_post(box->forks);
		return (1);
	}
	print_timestamp(box->num, FORK, FORK_L, box);
	return (0);
}

int			forks(t_ph *box)
{
	sem_wait(box->settings->eat);
	if (check_death(box))
		return (1);
	if (take_fork(box))
	{
		sem_post(box->settings->eat);
		return (1);
	}
	if (take_fork(box))
	{
		sem_post(box->forks);
		sem_post(box->settings->eat);
		return (1);
	}
	sem_post(box->settings->eat);
	return (0);
}

long long	eat(t_ph *box)
{
	if (forks(box) || !(box->start = get_time()) ||
	sleep_with_error(box->settings->t_eat, EAT, EAT_L, box))
	{
		sem_post(box->forks);
		sem_post(box->forks);
		return (1);
	}
	sem_post(box->forks);
	sem_post(box->forks);
	if (box->eat_count > 0)
		box->eat_count--;
	if (box->eat_count == -1)
		return (0);
	else
		return (box->eat_count ? 0 : 2);
}
