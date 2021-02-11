/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   struct_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cbach <cbach@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/02/11 18:18:15 by cbach             #+#    #+#             */
/*   Updated: 2021/02/11 18:18:15 by cbach            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philosophers_two.h"
#include "philo_two.h"

void	set_philos(t_ph *phs, t_settings *settings)
{
	int i;

	i = -1;
	while (++i < settings->p_count)
	{
		phs[i].settings = settings;
		phs[i].forks = settings->forks;
		phs[i].dead = settings->dead;
		phs[i].num = i + 1;
		phs[i].thread_started = 0;
		phs[i].eat_count = settings->e_count;
	}
}

int		set_box(t_ph *boxes, t_settings *settings)
{
	int			i;

	i = -1;
	while (++i < settings->p_count)
	{
		boxes[i].start = get_time();
		boxes[i].temp_time = boxes[i].start + 1;
		if (pthread_create(&settings->phs[i], NULL, start_simulation,
			(&boxes[i])))
		{
			settings->remain_phs = -1;
			return (print_return(1, TH_CR, TH_CR_L, settings->out));
		}
		boxes[i].thread_started = 1;
	}
	monitor(boxes);
	i = -1;
	while (++i < settings->p_count)
	{
		if (pthread_join(settings->phs[i], NULL))
			print_return(1, TH_JO, TH_JO_L, settings->out);
	}
	return (0);
}
