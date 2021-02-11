/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo_three.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cbach <cbach@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/02/11 13:10:04 by cbach             #+#    #+#             */
/*   Updated: 2021/02/11 13:10:04 by cbach            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philosophers_three.h"
#include "philo_three.h"
#include <error.h>
#include <errno.h>
#include <err.h>

int		start_simulation(void *arg)
{
	t_ph	*box;
	int		status;

	box = (t_ph *)arg;
	status = 0;
	box->eat_count = box->settings->e_count;
	while (box->eat_count && !status)
	{
		status = eat(box) ||
		sleep_with_error(box->settings->t_sleep, SLEEP, SLEEP_L, box) ||
		sleep_with_error(0, THINK, THINK_L, box);
	}
	sem_close(box->settings->forks);
	sem_close(box->settings->out);
	if (status == 1)
		return (box->num);
	sem_close(box->settings->dead);
	sem_close(box->settings->eat);
	return (0);
}

void	*monitor(void *arg)
{
	int		ok;
	t_ph	*phs;

	ok = 1;
	phs = (t_ph *)arg;
	while (ok && phs->settings->remain_phs && (phs->settings->e_count == -1 ? 1
	: phs->settings->e_count))
	{
		phs->temp_time = get_time();
		if (phs->temp_time - phs->start > phs->settings->t_die)
		{
			print_timestamp(phs->num, DIE, DIE_L, phs);
			sem_wait(phs->dead);
			sem_wait(phs->settings->out);
			phs->settings->remain_phs = -phs->num;
			ok = 0;
//			sem_post(phs->dead);
		}
		usleep(1000);
	}
	usleep(5000);
	return (NULL);
}

int		sems(t_settings *settings)
{
	sem_unlink("forks");
	sem_unlink("out");
	sem_unlink("dead");
	sem_unlink("eat");
	settings->forks = sem_open("forks", O_CREAT | O_EXCL, 0644,
		settings->p_count);
	settings->out = sem_open("out", O_CREAT | O_EXCL, 0644, 1);
	settings->dead = sem_open("dead", O_CREAT | O_EXCL, 0644, 1);
	settings->eat = sem_open("eat", O_CREAT | O_EXCL, 0644, 1);
	sem_unlink("forks");
	sem_unlink("out");
	sem_unlink("dead");
	sem_unlink("eat");
	return (settings->forks == SEM_FAILED || settings->out == SEM_FAILED ||
			settings->dead == SEM_FAILED || settings->eat == SEM_FAILED);
}

int		start(t_settings *settings)
{
	t_ph		phs[settings->p_count];
	pid_t		pids[settings->p_count];
	pthread_t	threads[settings->p_count];
	int			ok;

	settings->threads = threads;
	settings->phs = pids;
	sems(settings);
	set_philos(phs, settings);
	ok = set_box(phs, settings);
	sem_close(settings->forks);
	sem_close(settings->out);
	sem_close(settings->dead);
	sem_close(settings->eat);
	close(0);
	close(1);
	close(2);
	return (ok);
}

int		main(int argc, char **argv)
{
	t_settings	settings;

	if (get_settings(&settings, argc, argv))
		return (write(1, INPUT, INPUT_L));
	return (start(&settings));
}
