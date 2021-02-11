/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo_one.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cbach <cbach@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/02/11 13:09:49 by cbach             #+#    #+#             */
/*   Updated: 2021/02/11 13:21:57 by cbach            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philosophers_one.h"
#include "philo_one.h"

void	*start_simulation(void *arg)
{
	t_ph *box;

	box = (t_ph *)arg;
	box->eat_count = box->settings->e_count;
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
					pthread_mutex_lock(phs->dead);
					phs->settings->remain_phs = -phs[i].num;
					ok = 0;
				}
			}
		}
		usleep(5000);
	}
	if (!ok)
		pthread_mutex_unlock(phs->dead);
}

int		start(t_settings *settings)
{
	pthread_mutex_t	mutexes[settings->p_count];
	pthread_mutex_t	eat_mutexes[settings->p_count];
	pthread_t		threads[settings->p_count];
	t_ph			phs[settings->p_count];
	pthread_mutex_t die_mutex;
	
	settings->dead = &die_mutex;
	if (set_mutexes(mutexes, settings) || set_mutexes(eat_mutexes, settings)
		|| pthread_mutex_init(settings->dead, NULL))
		return (print_return(1, M_INIT_E, M_INIT_E_L, &settings->output_mutex));
	settings->eat_mutexes = eat_mutexes;
	settings->phs = threads;
	set_philos(phs, settings, mutexes);
	settings->e_count = set_box(phs, mutexes, settings);
	destroy_mutexes(mutexes, settings, settings->p_count);
	destroy_mutexes(eat_mutexes, settings, settings->p_count);
	destroy_mutexes(settings->dead, settings, 1);
	return (0);
}

int		main(int argc, char **argv)
{
	t_settings		settings;

	if ((get_settings(&settings, argc, argv)))
		return (write(1, INPUT, INPUT_L));
	start(&settings);
	close(0);
	close(1);
	close(2);
	return (0);
}
