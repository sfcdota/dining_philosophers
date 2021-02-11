/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo_two.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cbach <cbach@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/02/11 13:10:19 by cbach             #+#    #+#             */
/*   Updated: 2021/02/11 13:41:28 by cbach            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philosophers_two.h"
#include "philo_two.h"

void	*start_simulation(void *arg)
{
	t_ph *box;

	box = (t_ph *)arg;
	while (box->settings->remain_phs > 0)
	{
		if (box->eat_count && eat(box) ||
		sleep_with_error(box->settings->t_sleep, SLEEP, SLEEP_L, box) ||
			sleep_with_error(0, THINK, THINK_L, box))
			return (NULL);
	}
	box->settings->remain_phs--;
	return (NULL);
}

void	monitor(t_ph *phs)
{
	int	ok;
	int	i;

	ok = 1;
	while (ok && phs->settings->remain_phs)
	{
		i = -1;
		while (++i < phs->settings->p_count && ok && phs->settings->remain_phs)
		{
			if (phs[i].thread_started && phs[i].eat_count)
			{
				phs[i].temp_time = get_time();
				if (phs[i].temp_time - phs[i].start > phs->settings->t_die)
				{
					print_timestamp(phs[i].num, DIE, DIE_L, &phs[i]);
					sem_wait(phs->dead);
					phs->settings->remain_phs = -phs[i].num;
					ok = 0;
				}
			}
		}
		usleep(5000);
	}
	if (!ok)
		sem_post(phs->dead);
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
	pthread_t	threads[settings->p_count];
	int			ok;

	settings->phs = threads;
	sems(settings);
	set_philos(phs, settings);
	ok = set_box(phs, settings);
	sem_close(settings->forks);
	sem_close(settings->out);
	sem_close(settings->dead);
	sem_close(settings->eat);
	return (ok);
}

int		main(int argc, char **argv)
{
	t_settings	settings;
	if (get_settings(&settings, argc, argv))
		return (write(1, INPUT, INPUT_L));
	start(&settings);
	close(0);
	close(1);
	close(2);
	return (0);
}
