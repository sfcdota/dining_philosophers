#include "../philosophers.h"
#include "philo_one.h"
    #include <error.h>
	 #include <errno.h>
	 #include <err.h>
void destroy_mutexes(pthread_mutex_t *mutexes, t_settings *settings, int index)
{
	int i;
	int status;
	i = -1;
	while (++i < index)
		if ((status = pthread_mutex_destroy(mutexes++)))
//			print_return(1, M_DESTROY, M_DESTROY_L, &settings->output_mutex);
			printf("index = %i Error in mutex destroy, status = %d\n", i, status);
}

int set_mutexes(pthread_mutex_t *mutexes, t_settings *settings)
{
	int i;
	int ok;
	pthread_mutex_t *start;
	start = mutexes;
	ok = 0;
	i = -1;
	if (pthread_mutex_init(&settings->output_mutex, NULL))
		return (print_return(1, M_INIT_E, M_INIT_E_L, &settings->output_mutex));
	while (++i < settings->p_count && !ok)
		ok = pthread_mutex_init(&mutexes[i], NULL);
	if (ok)
		destroy_mutexes(start, settings, i);
	return (ok);
}

void set_philos(t_ph *phs, t_settings *settings, pthread_mutex_t *mutexes)
{
	int i;

	i = -1;
	while (++i < settings->p_count)
	{
		phs[i].settings = settings;
		phs[i].left_fork = &mutexes[i];
		phs[i].right_fork = i == settings->p_count - 1 ? &mutexes[0] : &mutexes[i + 1];
		phs[i].eat = &settings->eat_mutexes[((i + 1)% 2 == 0 ? i - 1 : i) % (i + 1)];
		phs[i].dead = &settings->die_mutexes[i];
		phs[i].num = i + 1;
		phs[i].thread_started = 0;
	}
}

int check_death(t_ph *box)
{
	if (pthread_mutex_lock(box->dead))
	{
		if (box->settings->remain_philos >= 0)
			box->settings->remain_philos = -box->num;
		return (print_return(1, M_LOCK, M_LOCK_L, &box->settings->output_mutex));
	}
	pthread_mutex_unlock(box->dead);
	return (box->settings->remain_philos <= 0);
}

int sleep_with_error(long val, char *msg, int msg_l, t_ph *box)
{
	print_timestamp(box->num, msg, msg_l, box);
	if (val && usleep(val * 1000))
		return (print_return(1, USLEEP, USLEEP_L, &box->settings->output_mutex));
	return (check_death(box));
}





int take_a_fork(t_ph *box, long start, pthread_mutex_t *fork)
{
	if (pthread_mutex_lock(fork))
		return (print_return(1, M_LOCK, M_LOCK_L, &box->settings->output_mutex));
	if (check_death(box))
		return (1);
	return (print_timestamp(box->num, FORK, FORK_L, box));
}

int unlock_eat(t_ph *box)
{
	return (pthread_mutex_unlock(box->left_fork) + pthread_mutex_unlock(box->right_fork) + pthread_mutex_unlock(box->eat));
}


int eat(t_ph *box, long long i)
{
	int ok = 0;
	if (pthread_mutex_lock(box->eat))
	{
		if (box->settings->remain_philos >= 0)
			box->settings->remain_philos = -box->num;
		return (print_return(1, M_LOCK, M_LOCK_L,
			&box->settings->output_mutex));
	}
	if (check_death(box))
	{
		pthread_mutex_unlock(box->eat);

		return (1);
	}
	if (take_a_fork(box, box->start, box->left_fork))
	{
		pthread_mutex_unlock(box->eat);
		pthread_mutex_unlock(box->left_fork);
		return (1);
	}
	if (take_a_fork(box, box->start, box->right_fork) || !(box->start = get_time())
	|| sleep_with_error(box->settings->t_eat, EAT, EAT_L, box))
	{
		if (unlock_eat(box))
			print_return(1, M_UNLOCK, M_UNLOCK_L, &box->settings->output_mutex);
		return (1);
	}
	unlock_eat(box);
	return (0);
}

int simulation(t_ph *box, long long i)
{
	return (eat(box, i) || sleep_with_error(box->settings->t_sleep, SLEEP, SLEEP_L, box) ||
	sleep_with_error(0, THINK, THINK_L, box));
}

void *start_simulation(void *arg)
{
	t_ph *box;
	box = (t_ph *)arg;
	long long i;

	i = box->settings->e_count;
	while (box->settings->remain_philos > 0 && i)
	{
		if (simulation(box, i))
			return (NULL);
		if (box->settings->e_count != -1)
		    i--;
	}
	box->settings->remain_philos--;
	return (NULL);
}

void monitor(t_ph *phs)
{
	int ok;
	ok = 1;
	int i;
	int j;

	while (ok && phs->settings->remain_philos)
	{
		i = -1;
		while (++i < phs->settings->p_count && ok)
		{
			if (phs[i].thread_started)
			{
				phs[i].temp_time = get_time();
				if (phs[i].temp_time - phs[i].start > phs->settings->t_die)
				{
					print_timestamp(phs[i].num, DIE, DIE_L, &phs[i]);
					pthread_mutex_lock(phs[i].dead);
					phs->settings->remain_philos = -phs[i].num;
					j = -1;
					while (++j < phs->settings->p_count)
						if (j != i)
							pthread_mutex_lock(phs[j].dead);
					ok = 0;
				}
			}
		}
		usleep(1000);
	}
	i = phs->settings->p_count;
	while (!ok && --i >= 0)
		pthread_mutex_unlock(phs[i].dead);
}

int	set_box(t_ph *boxes , pthread_mutex_t *mutexes, t_settings *settings)
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
			return (print_return(1, TH_CR, TH_CR_L, &settings->output_mutex));
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
			print_return(1, TH_JO, TH_JO_L, &settings->output_mutex);
	}
	return (0);
}

int main(int argc, char **argv)
{
	int ok;

	t_settings settings;
	if ((ok = get_settings(&settings, argc, argv)))
		return (ok);
	pthread_mutex_t mutexes[settings.p_count];
	pthread_mutex_t eat_mutexes[settings.p_count];
	pthread_mutex_t die_mutexes[settings.p_count];
	t_ph phs[settings.p_count];
	pthread_t threads[settings.p_count + 1];
	if (set_mutexes(mutexes, &settings) || set_mutexes(eat_mutexes, &settings) || set_mutexes(die_mutexes, &settings))
		return (print_return(1, M_INIT_E, M_INIT_E_L, &settings.output_mutex));
	settings.eat_mutexes = eat_mutexes;
	settings.die_mutexes = die_mutexes;
	settings.phs = threads;
	set_philos(phs, &settings, mutexes);
	ok = set_box(phs,  mutexes, &settings);
	printf("fork mutex destroy:\n");
	destroy_mutexes(mutexes, &settings, settings.p_count);
	printf("eat mutex destroy:\n");

	destroy_mutexes(eat_mutexes, &settings, settings.p_count);
	printf("die mutex destroy:\n");

	destroy_mutexes(die_mutexes,&settings, settings.p_count);
	return (ok);
}