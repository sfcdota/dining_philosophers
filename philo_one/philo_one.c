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
		print_message(0, 0, "There is an error in mutex destroy func", &settings->output_mutex);
}

int set_mutexes(pthread_mutex_t *mutexes, t_settings *settings)
{
	int i;
	int ok;
	pthread_mutex_t *start;
	
	start = mutexes;
	ok = 1;
	i = -1;
	while (++i < settings->p_count && ok)
		ok = pthread_mutex_init(mutexes++, NULL);
	if (!ok)
		destroy_mutexes(start, settings, i);
	return (!ok && pthread_mutex_init(&settings->output_mutex, NULL));
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

int time_with_error(t_box *box)
{
	int ok;
	if ((ok = print_message(0, gettimeofday(&box->time, &box->timezone),
		"Error in gettimeofday func", &box->settings->output_mutex)))
		box->settings->remain_philos = -1;
	return (ok);
}

int sleep_with_error(long val, char *message, t_box *box)
{
	if (val && usleep(val))
		return (print_message(0, 1, "Error in usleep", &box->settings->output_mutex));
	print_message((box->time.tv_usec - box->settings->timeval.tv_usec) / 1000, box->num, message, &box->settings->output_mutex);
	return (0);
}



int check_death(t_box *box, long start)
{
	if (time_with_error(box))
		return (1);
	if ((box->time.tv_usec - start) > box->settings->t_die)
	{
		print_message(box->time.tv_usec, box->num, "died", &box->settings->output_mutex);
		box->settings->remain_philos = -1;
		return (1);
	}
	return (0);
}

int take_a_fork(t_box *box, long start, pthread_mutex_t *fork)
{
	if (pthread_mutex_lock(fork))
	{
		box->settings->remain_philos = -1;
		print_message(0, 1, "Error in mutex lock func", &box->settings->output_mutex);
		return (1);
	}
	return (check_death(box, start) || print_message((box->time.tv_usec - box->settings->timeval.tv_usec) / 1000, box->num, " has taken a fork", &box->settings->output_mutex));
}

int simulation(t_box *box, long *start)
{
	if ((take_a_fork(box, *start, box->left_fork) || take_a_fork(box, *start, box->right_fork) || time_with_error(box)))
		return (1);
	*start = box->time.tv_usec;
	return (sleep_with_error(box->settings->t_eat, "is eating", box) || print_message(0, pthread_mutex_unlock(box->left_fork) ||
	pthread_mutex_unlock(box->right_fork), "There is an error in mutex unlock func", &box->settings->output_mutex) ||
	check_death(box, *start) || sleep_with_error(box->settings->t_sleep, "is sleeping", box) || check_death(box, *start) ||
	sleep_with_error(0, "is thinking", box));
	
}

void *start_simulation(void *arg)
{
	t_box *box;
	long	start;
	box = (t_box *)arg;
	while (box->settings->e_count && box->settings->remain_philos != -1 && !time_with_error(box))
	{
		start = box->time.tv_usec;
		simulation(box, &start);
	}
	return (NULL);
}


int	set_box(t_box *boxes ,t_philo *philos, pthread_mutex_t *mutexes, t_settings *settings)
{
	int i;
	pthread_t threads[settings->p_count];
	int ok;
	
	i = -1;
	ok = 0;
	while (++i < settings->p_count && !ok)
	{
		boxes->settings = settings;
		boxes->left_fork = mutexes + i;
		boxes->right_fork = i == settings->p_count - 1 ? mutexes : mutexes + i + 1;
		boxes->num = i + 1;
		if (pthread_create(&threads[i], NULL, start_simulation, (void *)(boxes))
		|| pthread_detach(threads[i]))
		{
			settings->remain_philos = -1;
			return (print_message(0, 1, "Error in thread create/detach", &settings->output_mutex));
		}
		boxes++;
	}
	while (settings->remain_philos != -1 || settings->e_count != -1 && settings->remain_philos == 0 && !ok)
		ok = usleep(1000);
	if (ok || usleep(1000000))
		return (1);
	return (0);
}

int main(int argc, char **argv)
{
	int ok;

	t_settings settings;
	if ((ok = get_settings(&settings, argc, argv)))
		return (ok);
	pthread_mutex_t mutexes[settings.p_count];
	t_philo philos[settings.p_count];
	t_box boxes[settings.p_count];
	if (set_mutexes(mutexes, &settings))
		return (print_message(0, 1, "Error in mutex init", &settings.output_mutex));
//	set_philos(philos, mutexes, &settings);
	ok = set_box(boxes, philos, mutexes, &settings);
	destroy_mutexes(mutexes, &settings, settings.p_count);
	usleep(1000000);
	return (ok);
}