/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_atoi.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cbach <cbach@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/05/24 17:18:38 by cbach             #+#    #+#             */
/*   Updated: 2020/05/30 22:20:45 by cbach            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philosophers.h"
#include "philo_two.h"


unsigned int get_time()
{
    if (gettimeofday(&timev, &zonev))
    {
        write(1, GTOD, ft_strlen(GTOD));
        return (0);
    }
    return (timev.tv_sec * 1000 + timev.tv_usec / 1000);
}

int get_settings(t_settings *settings, int argc, char **argv)
{
	if (argc < 5 || argc > 6 || (settings->p_count = ft_atoi(argv[1])) < 2 ||
		(settings->t_die = ft_atoi(argv[2])) < 0 ||
		(settings->t_eat = ft_atoi(argv[3])) < 0 ||
		(settings->t_sleep = ft_atoi(argv[4])) < 0 ||
		(argc == 6 &&(settings->e_count = ft_atoi(argv[5])) < 0))
		return (write(1, INPUT, ft_strlen(INPUT)));
	if (argc == 5)
		settings->e_count = -1;
	else if (!settings->e_count)
		return (1);
	if (!(settings->start_time = get_time()))
		return (write(1, GTOD, ft_strlen(GTOD)));
	settings->remain_philos = settings->p_count;
	return (0);
}

int		print_return(int retval, char *msg, int msg_l, sem_t *out)
{
	if (!(sem_wait(out)))
	{
		if (retval)
			write(1, msg, msg_l);
	}
	else
		return (write(1, M_LOCK, M_LOCK_L));
	return (sem_post(out) ? (int)write(1, M_UNLOCK, M_UNLOCK_L) : retval);
}

int		print_timestamp(int num, char *msg, int msg_l, t_ph *ph)
{
	if (!(ph->temp_time = get_time()))
		return (write(1, GTOD, GTOD_L));
	if (!(pthread_mutex_lock(&ph->settings->output_mutex)))
	{
		if (ph->settings->remain_philos != -1)
		{
			ft_putnbr_fd(ph->temp_time - ph->settings->start_time, STDOUT_FILENO);
			write(1, " ", 1);
			ft_putnbr_fd(num, STDOUT_FILENO);
			write(1, " ", 1);
			write(1, msg, msg_l);
		}
	}
	else
		return (write(1, M_LOCK, M_LOCK_L));
	return (pthread_mutex_unlock(&ph->settings->output_mutex) ? (int)write(1, M_UNLOCK, M_UNLOCK_L) : 0);
}