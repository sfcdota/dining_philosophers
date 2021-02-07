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

static int	is_over(long long int i, int multiplier, int current)
{
	if ((i == 922337203685477580 && current > 54 && multiplier == 1)
		|| (i == 922337203685477580 && current > 55 && multiplier == -1)
		|| i > 922337203685477580)
		return (1);
	else
		return (0);
}

int			ft_isdigit(int c)
{
	return (c > 47 && c < 58 ? 1 : 0);
}


int			ft_atoi(const char *nptr)
{
	long long int	i;
	int				multiplier;

	i = 0;
	multiplier = 1;
	while ((*nptr > 6 && *nptr < 14) || *nptr == 32)
		nptr++;
	if (*nptr == 45 || *nptr == 43)
	{
		if (*nptr == 45)
			multiplier *= -1;
		nptr++;
	}
	while (ft_isdigit(*nptr))
	{
		if (is_over(i, multiplier, *nptr))
			return (multiplier == 1 ? -1 : 0);
		i = i * 10 + (*nptr++ - 48);
	}
	return (i * multiplier);
}

int get_settings(t_settings *settings, int argc, char **argv)
{
	if (argc < 5 || argc > 6 || (settings->p_count = ft_atoi(argv[1])) < 2 ||
		(settings->t_die = ft_atoi(argv[2])) < 0 ||
		(settings->t_eat = ft_atoi(argv[3])) < 0 ||
		(settings->t_sleep = ft_atoi(argv[4])) < 0 ||
		(argc == 6 &&(settings->e_count = ft_atoi(argv[5])) < 0))
	{
		ft_putendl_fd("There is an error in parameters input of a program.\nExit", STDOUT_FILENO);
		return (1);
	}
	settings->t_eat *= 1000;
	settings->t_sleep *= 1000;
	settings->t_die *= 1000;
	if (argc == 5)
		settings->e_count = -1;
	else if (!settings->e_count)
		return (1);
	settings->remain_philos = settings->p_count;
	if (gettimeofday(&settings->timeval, &settings->timezone))
	{
		ft_putendl_fd("Error in gettimeofday func", STDOUT_FILENO);
		return (1);
	}
	return (0);
}