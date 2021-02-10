#include "philosophers.h"
#include "philo_two.h"
#include <error.h>
#include <errno.h>
#include <err.h>

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
	}
}

int check_death(t_ph *box)
{
	if (sem_wait(box->dead))
	{
		if (box->settings->remain_philos > 0)
			box->settings->remain_philos = -box->num;
		return (print_return(1, M_LOCK, M_LOCK_L, box->settings->out));
	}
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
	return (check_death(box));
}

long long eat(t_ph *box)
{
	int ok = 0;
	if (take_fork(box))
		sem_post(box->forks);
	if (take_fork(box) || !(box->start = get_time()) || sleep_with_error(box->settings->t_eat, EAT, EAT_L, box))
	{
		sem_post(box->forks);
		sem_post(box->forks);
		return (1);
	}
	sem_post(box->forks);
	sem_post(box->forks);
	if (box->eat_count > 0)
		box->eat_count--;
	if (!box->eat_count)
		box->settings->remain_philos--;
	return (0);
}

int simulation(t_ph *box)
{
	return (eat(box) || sleep_with_error(box->settings->t_sleep, SLEEP, SLEEP_L, box) ||
			sleep_with_error(0, THINK, THINK_L, box));
}

void *start_simulation(void *arg)
{
	t_ph *box;
	box = (t_ph *)arg;

	box->eat_count = box->settings->e_count;
	while (box->settings->remain_philos > 0 && box->eat_count)
	{
		if (simulation(box))
			return (NULL);
	}
	box->settings->remain_philos--;
	return (NULL);
}

void monitor(t_ph *phs)
{
	int ok;
	ok = 1;
	int i;

	while (ok && phs->settings->remain_philos)
	{
		i = -1;
		while (++i < phs->settings->p_count && ok && phs->settings->remain_philos)
		{
			if (phs[i].thread_started && phs[i].eat_count)
			{
				phs[i].temp_time = get_time();
				if (phs[i].temp_time - phs[i].start > phs->settings->t_die)
				{
					print_timestamp(phs[i].num, DIE, DIE_L, &phs[i]);
					sem_wait(phs->dead);
					phs->settings->remain_philos = -phs[i].num;
					ok = 0;
				}
			}
		}
		usleep(5000);
	}
	if (!ok)
		sem_post(phs->dead);
}

int	set_box(t_ph *boxes , t_settings *settings)
{
	int i;

	i = -1;
	while (++i < settings->p_count)
	{
		boxes[i].start = get_time();
		boxes[i].temp_time = boxes[i].start + 1;
		if (pthread_create(&settings->phs[i], NULL, start_simulation, (void *)(&boxes[i])))
		{
			settings->remain_philos = -1;
			return (print_return(1, TH_CR, TH_CR_L, settings->out));
		}
		boxes[i].thread_started = 1;
//		pthread_detach(settings->phs[i]);
	}
	monitor(boxes);
	i = -1;
	while (++i < settings->p_count)
	{
//		printf("thread #%d join...\n", i);
		if (pthread_join(settings->phs[i], NULL))
			print_return(1, TH_JO, TH_JO_L, settings->out);
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
	pthread_t threads[settings.p_count + 1];
	settings.phs = threads;
	sem_unlink("forks");
	sem_unlink("out");
	sem_unlink("dead");
	settings.forks = sem_open("forks", O_CREAT | O_EXCL, 0644, settings.p_count);
	settings.out = sem_open("out", O_CREAT | O_EXCL, 0644, 1);
	settings.dead = sem_open("dead", O_CREAT | O_EXCL, 0644, 1);
	set_philos(phs, &settings);
	ok = set_box(phs, &settings);
	return (ok);
}