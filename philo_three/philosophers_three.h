/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philosophers_three.h                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cbach <cbach@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/02/11 13:10:08 by cbach             #+#    #+#             */
/*   Updated: 2021/02/11 13:10:29 by cbach            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PHILOSOPHERS_PHILOSOPHERS_THREE_H
# define PHILOSOPHERS_PHILOSOPHERS_THREE_H
# include <stdio.h>
# include <string.h>
# include <unistd.h>
# include <pthread.h>
# include <sys/time.h>
# include <stdlib.h>
# include <semaphore.h>
# define INPUT "There is an error in parameters input of a program.\nExit\n"
# define INPUT_L 57
# define FORK "has taken a fork\n"
# define FORK_L 17
# define EAT "is eating\n"
# define EAT_L 10
# define SLEEP "is sleeping\n"
# define SLEEP_L 12
# define THINK "is thinking\n"
# define THINK_L 12
# define DIE "died\n"
# define DIE_L 5
# define M_INIT_E "Mutex init func error\n"
# define M_INIT_E_L 22
# define ML "Error in mutex lock func\n"
# define ML_L 25
# define M_UNLOCK "There is an error in mutex unlock func\n"
# define M_UNLOCK_L 39
# define M_DESTROY "There is an error in mutex destroy func\n"
# define M_DESTROY_L 40
# define TH_CR "Error in thread create\n"
# define TH_CR_L 23
# define TH_JO "Error in thread join\n"
# define TH_JO_L 21
# define SLP "Error in usleep\n"
# define SLP_L 16
# define GTOD "Error in gettimeofday func\n"
# define GTOD_L 27

struct timeval timev;
struct timezone zonev;
/*
** remain_phs: -1 = there is a dead philo, 0 all philos have eaten e_count
** times, > 0 all alive and doing their things
*/

typedef struct		s_settings
{
	int				p_count;
	long long		t_die;
	long long		t_eat;
	long long		t_sleep;
	long long		e_count;
	int				remain_phs;
	unsigned int	start_time;
	pid_t			*phs;
	pthread_t		*threads;
	sem_t			*forks;
	sem_t			*out;
	sem_t			*dead;
	sem_t			*eat;
}					t_settings;

int					ft_atoi(const char *nptr);
int					get_settings(t_settings *settings, int argc, char **argv);
unsigned int		get_time(void);
int					ft_strlen(char *s);
void				ft_putnbr_fd(long n, int fd);
#endif
