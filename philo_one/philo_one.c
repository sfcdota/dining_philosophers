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
		print_message(-1, NULL, "There is an error in mutex destroy func\n", &settings->output_mutex);
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
	
	return (ok ? ok : pthread_mutex_init(&settings->output_mutex, NULL));
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
	if ((ok = !(box->temp_time = get_time()) && print_message(-1, NULL,
		"Error in gettimeofday func\n", &box->settings->output_mutex)))
		box->settings->remain_philos = -1;
	return (ok ? ok : box->temp_time);
}

int sleep_with_error(long val, char *message, t_box *box)
{
	if (!(time_with_error(box)))
		return (1);
	print_message((box->temp_time - box->settings->start_time), box, message, &box->settings->output_mutex);
	if (val && usleep(val * 1000))
		return (print_message(-1, NULL, "Error in usleep\n", &box->settings->output_mutex));
	return (0);
}



int check_death(t_box *box, long long start)
{
	if (!(time_with_error(box)))
		return (1);
	if ((box->temp_time - start) > box->settings->t_die)
	{
		print_message(box->temp_time - box->settings->start_time, box, "died\n", &box->settings->output_mutex);
		box->settings->remain_philos = -1;
		pthread_mutex_unlock(box->left_fork);
		pthread_mutex_unlock(box->right_fork);
		return (1);
	}
	return (0);
}

int take_a_fork(t_box *box, long start, pthread_mutex_t *fork)
{
	if (box->settings->remain_philos == -1)
		return (-1);
	if (pthread_mutex_lock(fork))
	{
		box->settings->remain_philos = -1;
		print_message(-1, NULL, "Error in mutex lock func\n", &box->settings->output_mutex);
		return (1);
	}
	return (check_death(box, start) ? pthread_mutex_unlock(fork) || 1 : print_message(box->temp_time - box->settings->start_time, box, "has taken a fork\n", &box->settings->output_mutex));
}

int simulation(t_box *box, long long *start, int i)
{
	int eat_num;
	
	eat_num = (box->num % 2 == 0 ? box->num - 2 : box->num - 1) % box->num;
	if (pthread_mutex_lock(&box->settings->eat_mutexes[eat_num]))
	{
		box->settings->remain_philos = -1;
		print_message(-1, NULL, "Error in mutex lock func\n", &box->settings->output_mutex);
		return (1);
	}
    if ((take_a_fork(box, *start, box->left_fork) || take_a_fork(box, *start, box->right_fork)))
            return (1);
	if (!(*start = box->temp_time))
		return (1);
	if (i == box->settings->e_count)
		box->settings->remain_philos--;
	return (sleep_with_error(box->settings->t_eat, "is eating\n", box) ||
	print_message(-1, pthread_mutex_unlock(&box->settings->eat_mutexes[eat_num]) || pthread_mutex_unlock(box->left_fork) ||
	pthread_mutex_unlock(box->right_fork) ? NULL : box, "There is an error in mutex unlock func\n", &box->settings->output_mutex) ||
	check_death(box, *start) || sleep_with_error(box->settings->t_sleep, "is sleeping\n", box) || check_death(box, *start) ||
	sleep_with_error(0, "is thinking\n", box));
	
}

void *start_simulation(void *arg)
{
	t_box *box;
	long long	start;
	box = (t_box *)arg;
	int i;

	i = 0;
	while (box->settings->remain_philos > 0 && time_with_error(box))
	{
		if (!(start = get_time()) || simulation(box, &start, i))
			return (NULL);
		if (box->settings->e_count != -1)
		    i++;
	}
	return (NULL);
}


int	set_box(t_box *boxes ,t_philo *philos, pthread_mutex_t *mutexes, t_settings *settings)
{
	int i;
	pthread_t thread;
	int ok;
	
	i = -1;
	ok = 0;
	while (++i < settings->p_count)
	{
		boxes->settings = settings;
		boxes->left_fork = mutexes + i;
		boxes->right_fork = i == settings->p_count - 1 ? mutexes : mutexes + i + 1;
		boxes->num = i + 1;
		if (pthread_create(&thread, NULL, start_simulation, (void *)(boxes))
		|| pthread_detach(thread))
		{
			settings->remain_philos = -1;
			return (print_message(-1, NULL, "Error in thread create/detach\n", &settings->output_mutex));
		}
		boxes++;
	}
	while (settings->remain_philos != -1 || settings->e_count != -1 && settings->remain_philos == 0 && !ok)
		ok = usleep(1000);
	if (ok || usleep(1000))
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
	pthread_mutex_t eat_mutexes[settings.p_count];
	t_philo philos[settings.p_count];
	t_box boxes[settings.p_count];
	if (set_mutexes(mutexes, &settings) || set_mutexes(eat_mutexes, &settings))
		return (print_message(-1, NULL, "Error in mutex init\n", &settings.output_mutex));
	settings.eat_mutexes = eat_mutexes;
//	set_philos(philos, mutexes, &settings);
	ok = set_box(boxes, philos, mutexes, &settings);
	destroy_mutexes(mutexes, &settings, settings.p_count);
	destroy_mutexes(eat_mutexes, &settings, settings.p_count);
	return (ok);
}