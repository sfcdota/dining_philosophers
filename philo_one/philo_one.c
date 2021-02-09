#include "../philosophers.h"
#include "philo_one.h"
void destroy_mutexes(pthread_mutex_t *mutexes, t_settings *settings, int index)
{
	int i;
	int ok;

	ok = 1;
	i = -1;
	while (++i < index && ok)
		ok = pthread_mutex_destroy(mutexes++);
	if (!ok)
		print_return(1, M_DESTROY, M_DESTROY_L, &settings->output_mutex);
}

int set_mutexes(pthread_mutex_t *mutexes, t_settings *settings)
{
	int i;
	int ok;
	pthread_mutex_t *start;
	start = mutexes;
	ok = 0;
	i = -1;
	while (++i < settings->p_count && !ok)
		ok = pthread_mutex_init(&mutexes[i], NULL);
	if (ok)
		destroy_mutexes(start, settings, i);
	
	return (ok ? ok : pthread_mutex_init(&settings->output_mutex, NULL) || pthread_mutex_init(&settings->die_mutex, NULL));
}

//void set_philos(t_philo *philos, pthread_mutex_t *mutexes, t_settings *settings)
//{
//	int i;
//	i = -1;
//	while (++i < settings->p_count)
//	{
//		philos->eat_count = settings->e_count >= 0 ? 0 : -1;
//		philos->left_fork = &mutexes[i];
//		philos->right_fork = i == settings->p_count - 1 ? &mutexes[0] : &mutexes[i + 1];
//		philos->state = i == settings->p_count - 1 ? 2 : 0;
//		philos++;
//	}
//}

unsigned int time_with_error(t_box *box)
{
	int ok;
	if ((ok = !(box->temp_time = get_time()) && print_return(1, GTOD, GTOD_L,
		&box->settings->output_mutex)))
		box->settings->remain_philos = -box->num;
	return (ok ? ok : box->temp_time);
}

int check_death(t_box *box)
{
	if (!(time_with_error(box)) || (box->temp_time - box->start) > box->settings->t_die || box->settings->remain_philos < 0)
	{
		if (box->settings->remain_philos >= 0)
			box->settings->remain_philos = -box->num;
		return (1);
	}
	return (0);
}

int sleep_with_error(long val, char *msg, int msg_l, t_box *box)
{
	if (!(time_with_error(box)))
		return (1);
	print_timestamp(box->num, msg, msg_l, box->settings);
	if (val && usleep(val * 1000))
		return (print_return(1, USLEEP, USLEEP_L, &box->settings->output_mutex));
	return (check_death(box));
}





int take_a_fork(t_box *box, long start, pthread_mutex_t *fork)
{
	if (box->settings->remain_philos <= 0)
		return (1);
	if (pthread_mutex_lock(fork))
	{
		box->settings->remain_philos = -box->num;
		return (print_return(1, M_LOCK, M_LOCK_L, &box->settings->output_mutex));
	}
	return (check_death(box) || print_timestamp(box->num, FORK, FORK_L, box->settings));
}

int simulation(t_box *box, int i)
{
	if (!(box->start = time_with_error(box)))
		return (1);
	if (pthread_mutex_lock(box->eat))
	{
		box->settings->remain_philos = -box->num;
		return (print_return(1, M_LOCK, M_LOCK_L, &box->settings->output_mutex));
	}
	if (check_death(box))
	{
		pthread_mutex_unlock(box->eat);
		return (1);
	}
    if ((take_a_fork(box, box->start, box->left_fork) || take_a_fork(box, box->start, box->right_fork)) ||
		!(box->start = time_with_error(box)))
	{
		pthread_mutex_unlock(box->eat);
		pthread_mutex_unlock(box->left_fork);
		pthread_mutex_unlock(box->right_fork);
		return (1);
	}
	if (i == box->settings->e_count)
		box->settings->remain_philos--;
	
	
//	
//	int ok;
//	ok = sleep_with_error(box->settings->t_eat, EAT, EAT_L, box);
//	pthread_mutex_lock(&box->settings->output_mutex);
//	write(1, "eat end val = ", 14);
//	ft_putnbr_fd(ok, STDOUT_FILENO);
//	write(1, "\n", 1);
//	pthread_mutex_unlock(&box->settings->output_mutex);
//	
//	
//	ok = pthread_mutex_unlock(box->left_fork);
//	pthread_mutex_lock(&box->settings->output_mutex);
//	write(1, "left end val = ", 15);
//	ft_putnbr_fd(ok, STDOUT_FILENO);
//	write(1, "\n", 1);
//	pthread_mutex_unlock(&box->settings->output_mutex);
//
//	
//	ok = pthread_mutex_unlock(box->right_fork);
//	pthread_mutex_lock(&box->settings->output_mutex);
//	write(1, "right end val = ", 16);
//	ft_putnbr_fd(ok, STDOUT_FILENO);
//	write(1, "\n", 1);
//	pthread_mutex_unlock(&box->settings->output_mutex);
//
//	
//	ok = pthread_mutex_unlock(box->eat);
//	write(1, "eatmut end val = ", 17);
//	ft_putnbr_fd(ok, STDOUT_FILENO);
//	write(1, "\n", 1);
//	pthread_mutex_unlock(&box->settings->output_mutex);
//	ok = sleep_with_error(box->settings->t_sleep, SLEEP, SLEEP_L, box);
//	ok = sleep_with_error(0, THINK, THINK_L, box);
//	return (0);
	sleep_with_error(box->settings->t_eat, EAT, EAT_L, box);


	return (
	print_return(pthread_mutex_unlock(box->left_fork) || pthread_mutex_unlock(box->right_fork)
	|| pthread_mutex_unlock(box->eat), M_UNLOCK, M_UNLOCK_L, &box->settings->output_mutex)
	|| sleep_with_error(box->settings->t_sleep, SLEEP, SLEEP_L, box) ||
	sleep_with_error(0, THINK, THINK_L, box));
}

void *start_simulation(void *arg)
{
	t_box *box;
	box = (t_box *)arg;
	int i;

	i = 0;
	while (box->settings->remain_philos > 0 && time_with_error(box))
	{
		if (simulation(box, i))
		{
			box->settings->remain_philos = -box->num;
			return (NULL);
		}
		if (box->settings->e_count != -1)
		    i++;
	}
	return (NULL);
}

void *monitor(void *arg)
{
	t_box *boxes = (t_box *)arg;
	int ok;
	ok = 1;
	int i;
	while (ok)
	{
		usleep(1000);
		i = -1;
		while (++i < boxes->settings->p_count && ok)
		{
			boxes[i].temp_time = get_time();
			if (boxes[i].temp_time - boxes[i].start > boxes->settings->t_die)
			{
				boxes->settings->remain_philos = -boxes[i].num;
				pthread_mutex_lock(&boxes->settings->output_mutex);
				ft_putnbr_fd(boxes[i].temp_time - boxes[i].settings->start_time, STDOUT_FILENO);
				write(1, " ", 1);
				ft_putnbr_fd(boxes[i].num, STDOUT_FILENO);
				write(1, " ", 1);
				write(1, DIE, DIE_L);
				pthread_mutex_unlock(&boxes->settings->output_mutex);
				ok = 0;
			}
		}
	}
	while (boxes->settings->p_count-- > 0)
		ok = ok || pthread_join(boxes->settings->threads[boxes->settings->p_count - 1], NULL);
	if (ok)
		print_return(1, TH_JO, TH_JO_L, &boxes->settings->output_mutex);
	return (NULL);
}

int	set_box(t_box *boxes ,t_philo *philos, pthread_mutex_t *mutexes, t_settings *settings)
{
	int i;
	pthread_t threads[settings->p_count + 1];
	
	i = -1;
	settings->threads = threads;
	if (pthread_create(&threads[settings->p_count], NULL, monitor, (void *)boxes))
		return (write(1, TH_CR, TH_CR_L));
	while (++i < settings->p_count)
	{
		boxes->settings = settings;
		boxes->left_fork = mutexes + i;
		boxes->right_fork = i == settings->p_count - 1 ? mutexes : mutexes + i + 1;
		boxes->eat = &settings->eat_mutexes[((i + 1)% 2 == 0 ? i - 1 : i) % (i + 1)];
		boxes->num = i + 1;
		if (pthread_create(&threads[i], NULL, start_simulation, (void *)(boxes)))
		{
			settings->remain_philos = -1;
			return (print_return(1, TH_CR, TH_CR_L, &settings->output_mutex));
		}
		boxes++;
	}
	if (pthread_join(threads[settings->p_count], NULL))
		return (write(1, TH_JO, TH_JO_L));
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
	t_philo philos[settings.p_count];
	t_box boxes[settings.p_count];
	if (set_mutexes(mutexes, &settings) || set_mutexes(eat_mutexes, &settings))
		return (print_return(1, M_INIT_E, M_INIT_E_L, &settings.output_mutex));
	settings.eat_mutexes = eat_mutexes;
//	set_philos(philos, mutexes, &settings);
	ok = set_box(boxes, philos, mutexes, &settings);
	destroy_mutexes(mutexes, &settings, settings.p_count);
	destroy_mutexes(eat_mutexes, &settings, settings.p_count);
	return (ok);
}