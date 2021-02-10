#include "philosophers.h"
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
	if (box->temp_time - box->start > box->settings->t_die)
	{
		print_timestamp(box->num, DIE, DIE_L, box);
		box->is_dead = 1;
	}
	sem_post(box->dead);
	return (box->is_dead);
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
	return (0);
}

int simulation(t_ph *box)
{
	return (eat(box) || sleep_with_error(box->settings->t_sleep, SLEEP, SLEEP_L, box) ||
			sleep_with_error(0, THINK, THINK_L, box));
}

void start_simulation(void *arg)
{
	t_ph *box;
	box = (t_ph *)arg;

	box->eat_count = box->settings->e_count;
	while (box->eat_count)
	{
		if (simulation(box))
		{
			sem_post(box->settings->end);
			return ;
		}
	}
	sem_post(box->settings->end);
	return ;
}

int	set_box(t_ph *boxes , t_settings *settings)
{
	int i;
	int status;
	
	i = -1;
	while (++i < settings->p_count)
	{
		boxes[i].start = get_time();
		boxes[i].temp_time = boxes[i].start + 1;
		if ((settings->phs[i] = fork()))
		{
			settings->forks = sem_open("forks", 0);
			settings->out = sem_open("out", 0);
			settings->dead = sem_open("dead", 0);
			settings->end = sem_open("end", 0);
			start_simulation(boxes);
			sem_post(settings->end);
			sem_close(settings->forks);
			sem_close(settings->out);
			sem_close(settings->dead);
			sem_close(settings->end);
			exit (0);
		}
		else
			return (print_return(1, TH_CR, TH_CR_L, settings->out));
	}
	i = -1;
//	if (settings->e_count != -1)
//	{
//		while (++i < settings->e_count)
//			sem_wait(settings->end);
//		while (--i >= 0)
//			sem_post(settings->end);
//	}
//	else
//	{
//		sem_wait(settings->end);
//		sem_post(settings->end);
//	}
	while (++i < settings->p_count)
	{
		if (waitpid(settings->phs[i], &status, WUNTRACED) == -1)
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
	pid_t pids[settings.p_count];
	settings.phs = pids;
	sem_unlink("forks");
	sem_unlink("out");
	sem_unlink("dead");
	sem_unlink("end");
	settings.forks = sem_open("forks", O_CREAT | O_EXCL, 0644, settings.p_count);
	settings.out = sem_open("out", O_CREAT | O_EXCL, 0644, 1);
	settings.dead = sem_open("dead", O_CREAT | O_EXCL, 0644, 1);
	settings.end = sem_open("end", O_CREAT | O_EXCL, 0644, settings.p_count);
	sem_close(settings.forks);
	sem_close(settings.out);
	sem_close(settings.dead);
	sem_close(settings.end);
	set_philos(phs, &settings);
	ok = set_box(phs, &settings);
	return (ok);
}