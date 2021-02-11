/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo_three.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cbach <cbach@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/02/11 13:10:04 by cbach             #+#    #+#             */
/*   Updated: 2021/02/11 13:10:04 by cbach            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philosophers_three.h"
#include "philo_three.h"
#include <error.h>
#include <errno.h>
#include <err.h>
#include <sys/wait.h>
void set_philos(t_ph *phs, t_settings *settings)
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

int check_death(t_ph *box)
{
	if (sem_wait(box->dead))
		return (print_return(1, M_LOCK, M_LOCK_L, box->settings->out));
	sem_post(box->dead);
	return (box->settings->remain_philos <= 0);
}

int sleep_with_error(long val, char *msg, int msg_l, t_ph *box)
{
	print_timestamp(box->num, msg, msg_l, box);
	if (val && usleep(val * 1000))
		return (print_return(1, USLEEP, USLEEP_L, box->settings->out));
	return (check_death(box));
}

int take_fork(t_ph *box)
{
	if (sem_wait(box->forks))
	{
		if (box->settings->remain_philos > 0)
			box->settings->remain_philos = -box->num;
		return (print_return(1, M_LOCK, M_LOCK_L,
			box->settings->out));
	}
	if (check_death(box))
	{
		sem_post(box->forks);
		return (1);
	}
	return (0);
}

long long eat(t_ph *box)
{
	int ok = 0;
	sem_wait(box->settings->eat);
	if (take_fork(box))
	{
		sem_post(box->settings->eat);
		return (1);
	}
	if (take_fork(box))
	{
		sem_post(box->forks);
		sem_post(box->settings->eat);
		return (1);
	}
	sem_post(box->settings->eat);
	if (!(box->start = get_time()) || sleep_with_error(box->settings->t_eat, EAT, EAT_L, box))
	{
		sem_post(box->forks);
		sem_post(box->forks);
		return (1);
	}
	sem_post(box->forks);
	sem_post(box->forks);
	if (box->eat_count > 0)
		box->eat_count--;
	if (box->eat_count == -1)
		return (0);
	else
		return (box->eat_count ? 0 : 2);
}

int simulation(t_ph *box)
{
	return (eat(box) || sleep_with_error(box->settings->t_sleep, SLEEP, SLEEP_L, box) ||
			sleep_with_error(0, THINK, THINK_L, box));
}

int start_simulation(void *arg)
{
	t_ph *box;
	box = (t_ph *)arg;
	int status;

	status = 0;
	box->settings->forks = sem_open("forks", 0);
	box->settings->out = sem_open("out", 0);
	box->settings->dead = sem_open("dead", 0);
	box->eat_count = box->settings->e_count;
	while (box->eat_count && !status)
	{
		status = simulation(box);
	}
	sem_close(box->settings->forks);
	sem_close(box->settings->out);
	if (status == 1)
		return (box->num);
	sem_close(box->settings->dead);
	return (0);
}

void *monitor(void *arg)
{
	int ok;
	ok = 1;
	t_ph *phs;
	phs = (t_ph *)arg;
	while (ok && phs->settings->remain_philos && (phs->settings->e_count == -1 ? 1 : phs->settings->e_count))
	{
		phs->temp_time = get_time();
		if (phs->temp_time - phs->start > phs->settings->t_die)
		{
			print_timestamp(phs->num, DIE, DIE_L, phs);
			sem_wait(phs->dead);
			phs->settings->remain_philos = -phs->num;
			ok = 0;
		}
	}
	usleep(5000);
	return (NULL);
}

int	set_box(t_ph *boxes , t_settings *settings)
{
	int i;
	int status;
	int kek;
	i = -1;
	while (++i < settings->p_count)
	{
		boxes[i].start = get_time();
		boxes[i].temp_time = boxes[i].start + 1;
		if (!(settings->phs[i] = fork()))
		{
			pthread_create(&settings->threads[i], NULL, monitor, &boxes[i]);
			pthread_detach(settings->threads[i]);
			kek = start_simulation(&boxes[i]);
			exit (kek);
		}
		else if (settings->phs[i] == -1)
			return (print_return(1, TH_CR, TH_CR_L, settings->out));
	}
	i = -1;
	int j = 0;
	while (j < settings->p_count)
	{
		if (waitpid(-1, &status, WUNTRACED) == -1)
			print_return(1, TH_JO, TH_JO_L, settings->out);
		if (status == 1)
		{
			while (++i < settings->p_count)
				kill(settings->phs[i], SIGKILL);
			break ;
		}
		else
			j++;
	}
	return (0);
}

int main(int argc, char **argv)
{
	int ok;

	t_settings settings;
	if ((ok = get_settings(&settings, argc, argv)))
		return (ok);
	t_ph phs[settings.p_count];
	pid_t pids[settings.p_count];
	pthread_t threads[settings.p_count];
	settings.threads = threads;
	settings.phs = pids;
	sem_unlink("forks");
	sem_unlink("out");
	sem_unlink("dead");
	sem_unlink("eat");
	settings.forks = sem_open("forks", O_CREAT | O_EXCL, 0644, settings.p_count);
	settings.out = sem_open("out", O_CREAT | O_EXCL, 0644, 1);
	settings.dead = sem_open("dead", O_CREAT | O_EXCL, 0644, 1);
	settings.eat = sem_open("eat", O_CREAT | O_EXCL, 0644, 1);
	set_philos(phs, &settings);
	ok = set_box(phs, &settings);
	sem_close(settings.forks);
	sem_close(settings.out);
	sem_close(settings.dead);
	sem_unlink("eat");
	return (ok);
}
