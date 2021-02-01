#include "../philosophers.h"
#include "philo_one.h"
void set_mutexes(pthread_mutex_t *mutexes, t_settings *settings)
{
	int i;
	
	i = -1;
	while (++i < settings->p_count)
		pthread_mutex_init(mutexes++, NULL);
	pthread_mutex_init(&settings->output_mutex, NULL);
}

void set_philos(t_philo *philos, pthread_mutex_t *mutexes, t_settings *settings)
{
	int i;
	i = -1;
	while (++i < settings->p_count)
	{
		philos->eat_count = settings->e_count >= 0 ? 0 : -1;
		philos->left_fork = &mutexes[i];
		philos->right_fork = i == settings->p_count - 1 ? &mutexes[0] : &mutexes[i + 1];
		philos->state = i == settings->p_count - 1 ? 2 : 0;
		philos++;
	}
}
int thinking()
{
	
}
int eating()
{
	
}

int	sleeping()
{
	
}

void *start_simulation(void *arg)
{
	t_box *box;

	box = (t_box *)arg;
	while (box->settings.e_count && !box->settings.dead)
	{
		
	}
}


void	set_box(t_box *boxes ,t_philo *philos, pthread_mutex_t *mutexes, t_settings *settings)
{
	int i;
	void **thread_return;
	pthread_t threads[settings->p_count];
	int success;
	
	i = -1;
	success = 1;
	thread_return = malloc(sizeof(void *));
	while (++i < settings->p_count)
	{
		boxes->settings = *settings;
		boxes->left_fork = mutexes[i];
		boxes->right_fork = i == settings->p_count - 1 ? mutexes[0] : mutexes[i];
		boxes->num = i + 1;
		pthread_create(&threads[i], NULL, start_simulation, (void *)(boxes));
		boxes++;
	}
	i = -1;
	while (++i < settings->p_count)
	{
		pthread_join(threads[i], thread_return);
		if (*(int *)(*thread_return) == 1)
			success = 0;
	}
	
}

int main(int argc, char **argv)
{
	t_settings settings;
	if (get_settings(&settings, argc, argv))
		return (0);
	pthread_mutex_t mutexes[settings.p_count];
	t_philo philos[settings.p_count];
	t_box boxes[settings.p_count];
	set_mutexes(mutexes, &settings);
	set_philos(philos, mutexes, &settings);
	set_boxes(boxes, philos, mutexes, &settings);
}