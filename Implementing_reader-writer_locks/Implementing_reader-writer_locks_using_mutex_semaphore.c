/*  
 *  Write a program where multiple readers allowed to access shared data concurrently
 *  but only one writer allowed to access the shared data, if there is writer thread
 *  accessing the shared data then no others readers / writers allowed to access
 *  the shared data.
 *
 *  Basically we have to implement reader-writer locks 
 *
 *  In this post we are going to implement reader-writer lock
 *  using mutex and semaphore.
 *
 *  To compile this program run below cmd
 *  gcc <program-name> -lpthread  -o <output-file-name>
 *
 *  To run this program run below cmd
 *  ./<output-file-name>
 */

# include <stdio.h>
# include <pthread.h>
# include <semaphore.h> 

// mutex variable
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
// binary semaphore
sem_t sem;
// reader count variable to track how many reader are reading the val
int reader_count;
// shared variable between reader and writer threads
int shared_data;

void *writer(void *arg) {
	// execute wait semaphore to stop all writers and readers
	// thread to enter into the critcal section
	sem_wait(&sem);
	
	// modify the shared variable
	shared_data = shared_data + 1;
	
	// Give signal to the writers and readers thread to 
	// enter into the critical section
	sem_post(&sem);
}

void *reader(void *arg) {
	// taking mutex lock
	pthread_mutex_lock(&lock);
	// incrementing the reader count
	reader_count = reader_count + 1;
	// if this is the first reader then execute wait semaphore to stop 
	// all the writer thread to goes into the critcal section
	if (reader_count == 1) {
		sem_wait(&sem);
	}
	// releasing mutex lock	
	pthread_mutex_unlock(&lock);

	// reading the shared variable
	printf("%d ", shared_data);
	
	// taking mutex lock
	pthread_mutex_lock(&lock);
	// decrementing the reader count	
	reader_count = reader_count - 1;
	// if the reader count is zero then give signal to the writer thread to
	// enter into the critical section  
	if (reader_count == 0) {
		sem_post(&sem);
	}	
	// releasing mutex lock
	pthread_mutex_unlock(&lock);		
}	

int main() {
	// initialize the semaphore variable
	sem_init(&sem, 0, 1);
	
	// create 10 readers thread
	int noOfReaders = 10;
	pthread_t readerThread[noOfReaders];
	// create 5 writers thread
	int noOfWriters = 5;
	pthread_t writerThread[noOfWriters];
	// initialize reader count with Zero
	reader_count = 0;	

	int i;

	for (i = 0; i < noOfReaders; ++i) {
		pthread_create(&readerThread[i], NULL, reader, NULL);
	}

	for (i = 0; i < noOfWriters; ++i) {
                pthread_create(&writerThread[i], NULL, writer, NULL);
        }

	for (i = 0; i < noOfReaders; ++i) {
                pthread_join(readerThread[i], NULL);
        }

	for (i = 0; i < noOfWriters; ++i) {
                pthread_join(writerThread[i], NULL);
        }
	
	sem_destroy(&sem);
	
	printf("\nAll readers-writers threads exited.\n");

	return 0;

}

