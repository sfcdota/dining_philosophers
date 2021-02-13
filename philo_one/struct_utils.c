/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   struct_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cbach <cbach@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/02/11 17:31:16 by cbach             #+#    #+#             */
/*   Updated: 2021/02/11 17:31:16 by cbach            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philosophers_one.h"
#include "philo_one.h"

void	destroy_mutexes(pthread_mutex_t *mutexes,
	t_settings *settings, int index)
{
	int i;

	i = -1;
	while (++i < index)
		if ((pthread_mutex_destroy(mutexes++)))
			print_return(1, M_DESTROY, M_DESTROY_L, &settings->output_mutex);
}

int		set_mutexes(pthread_mutex_t *mutexes, t_settings *settings, int count)
{
	int				i;
	int				ok;
	pthread_mutex_t	*start;

	start = mutexes;
	ok = 0;
	i = -1;
	if (pthread_mutex_init(&settings->output_mutex, NULL))
		return (print_return(1, M_INIT_E, M_INIT_E_L, &settings->output_mutex));
	while (++i < count && !ok)
		ok = pthread_mutex_init(&mutexes[i], NULL);
	if (ok)
		destroy_mutexes(start, settings, i);
	return (ok);
}

void	set_philos(t_ph *phs, t_settings *settings, pthread_mutex_t *mutexes)
{
	int i;
	int num;
	i = -1;
	while (++i < settings->p_count)
	{
		num = (((i + 1) % 2 == 0 ? i - 1 : i) % (i + 1)) / 2;
		phs[i].settings = settings;
		phs[i].left_fork = &mutexes[i];
		phs[i].right_fork =
			i == settings->p_count - 1 ? &mutexes[0] : &mutexes[i + 1];
		phs[i].eat =
			&settings->eat_mutexes[num];
		phs[i].dead = settings->dead;
		phs[i].num = i + 1;
		phs[i].thread_started = 0;
		phs[i].eat_count = settings->e_count;
	}
}

int		set_box(t_ph *boxes, pthread_mutex_t *mutexes, t_settings *settings)
{
	int i;

	i = -1;
	while (++i < settings->p_count)
	{
		boxes[i].start = get_time();
		boxes[i].temp_time = boxes[i].start + 1;
		if (pthread_create(&settings->phs[i], NULL, start_simulation,
			&boxes[i]))
		{
			settings->remain_phs = -1;
			return (print_return(1, TH_CR, TH_CR_L, &settings->output_mutex));
		}
		boxes[i].thread_started = 1;
	}
	monitor(boxes);
	i = -1;
	while (++i < settings->p_count)
	{
		if (pthread_join(settings->phs[i], NULL))
			print_return(1, TH_JO, TH_JO_L, &settings->output_mutex);
	}
	return (0);
}
