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

#include "../philosophers.h"
#include "../philo_one/philo_one.h"


unsigned int get_time()
{
    if (gettimeofday(&timev, &zonev))
    {
        write(1, "Error in gettimeofday func\n", 27);
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
	{
		write(1, "There is an error in parameters input of a program.\nExit\n", 57);
		return (1);
	}
	if (argc == 5)
		settings->e_count = 0;
	else if (!settings->e_count)
		return (1);
	if (!(settings->start_time = get_time()))
		return (write(1, "There is an error in gettimeofday func\nExit\n", 44));
	settings->remain_philos = settings->p_count;
	return (0);
}

int		print_message(long num1, t_box *box, char *msg, pthread_mutex_t *out)
{
	int ok;

	if (num1 < 0)
	{
		if (!box)
		{
			write(1, msg, str_len(msg));
			return (-(int) num1);
		}
		return (0);
	}
	if (!(ok = pthread_mutex_lock(out)) && box->settings->remain_philos != -1)
	{
		ft_putnbr_fd(num1, STDOUT_FILENO);
		write(1, " ", STDOUT_FILENO);
		ft_putnbr_fd(box->num, STDOUT_FILENO);
		write(1, " ", STDOUT_FILENO);
		write(1, msg, str_len(msg));
	}
	if ((ok = (pthread_mutex_unlock(out) || ok)) && box->settings->remain_philos != -1)
		write(1, "Error in mutex lock/unlock functions\n", 37);
	return (ok);
}