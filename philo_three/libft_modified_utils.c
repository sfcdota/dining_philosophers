/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   libft_modified_utils.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cbach <cbach@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/02/11 13:09:59 by cbach             #+#    #+#             */
/*   Updated: 2021/02/11 13:10:29 by cbach            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philosophers_three.h"

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

int		ft_strlen(char *s)
{
	int i;
	i = 0;
	while (s && *s && *s++)
		i++;
	return (i);
}

void	ft_putnbr_fd(long n, int fd)
{
	unsigned char c;

	if (n > 9)
		ft_putnbr_fd(n / 10, fd);
	c = (unsigned char)(n % 10 + 48);
	write(fd, &c, 1);
}
