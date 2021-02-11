/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   struct_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cbach <cbach@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/02/11 18:30:49 by cbach             #+#    #+#             */
/*   Updated: 2021/02/11 18:30:49 by cbach            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philosophers_three.h"
#include "philo_three.h"
#include <sys/wait.h>

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
		phs[i].is_dead = 0;
	}
}

int		waitchilds(t_settings *settings)
{
	int	j;
	int	status;
	int	i;

	i = -1;
	j = 0;
	while (j < settings->p_count)
	{
		if (waitpid(-1, &status, WUNTRACED) == -1)
			print_return(1, TH_JO, TH_JO_L, settings->out);
		status = WEXITSTATUS(status);
		if (status == 1)
		{
			while (++i < settings->p_count)
				kill(settings->phs[i], SIGINT);
			break ;
		}
		else
			j++;
	}
	usleep(1000 * settings->p_count);
	return (0);
}

int		set_box(t_ph *boxes, t_settings *settings)
{
	int	i;
	int	kek;

	i = -1;
	while (++i < settings->p_count)
	{
		if (!(settings->phs[i] = fork()))
		{
			boxes[i].start = get_time();
			boxes[i].temp_time = boxes[i].start + 1;
			pthread_create(&settings->threads[i], NULL, monitor, &boxes[i]);
			pthread_detach(settings->threads[i]);
			kek = start_simulation(&boxes[i]);
			close(0);
			close(1);
			close(2);
			sem_close(settings->forks);
			sem_close(settings->dead);
			sem_close(settings->eat);
			exit(kek);
		}
		else if (settings->phs[i] == -1)
			return (print_return(1, TH_CR, TH_CR_L, settings->out));
	}
	waitchilds(settings);
	return (0);
}
