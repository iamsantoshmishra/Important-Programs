/*  
 *  Write a program where multiple readers allowed to access shared data concurrently
 *  but only one writer allowed to access the shared data, if there is writer thread
 *  accessing the shared data then no others readers / writers allowed to access
 *  the shared data.
 *
 *  Basically we have to implement reader-writer locks 
 *
 *  In this post we are going to implement reader-writer lock
 *  using mutex and conditional variable.
 *
 *  To compile this program run below cmd
 *  gcc <program-name> -lpthread  -o <output-file-name>
 *
 *  To run this program run below cmd
 *  ./<output-file-name>
 */

# include <stdio.h>
# include <pthread.h>

// mutex variable
pthread_mutex_t reader_mutex_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t writer_mutex_lock = PTHREAD_MUTEX_INITIALIZER;
// conditional variable
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
// reader count variable to track how many readers are reading the shared data
int reader_count, writer_count;
// shared variable between reader and writer threads
int shared_data;

void *writer(void *arg) {
	pthread_mutex_lock(&writer_mutex_lock);
        // if there is any readers or writers are into the critical section
	// then sleep the writer thread and wait for readers or writer
	// thread signal	
	while (reader_count > 0) {
		pthread_cond_wait(&cond, &writer_mutex_lock);
	}
	// update the write count so that other reader/writer thread
	// should not enter into the critical section i.e set the 
	// writer count
	writer_count = 1;	

	// execute wait semaphore to stop all writers and readers
	// thread to enter into the critcal section
	// modify the shared data
	shared_data = shared_data + 1;

	// reset the writer count
	writer_count = 0;
	// Give signal to the writers and readers thread to 
	// enter into the critical section
	pthread_cond_signal(&cond);
	pthread_mutex_unlock(&writer_mutex_lock);
}

void *reader(void *arg) {
	// taking mutex lock
	pthread_mutex_lock(&reader_mutex_lock);
	// incrementing the reader count
	reader_count = reader_count + 1;
	// if the writer thead accessig the critical section then  
	// sleep the reader thread and wait for writer thread signal
	while (writer_count == 1) {
		pthread_cond_wait(&cond, &writer_mutex_lock);
	}
	// releasing mutex lock	
	pthread_mutex_unlock(&reader_mutex_lock);

	// reading the shared variable
	printf("%d ", shared_data);
	
	// taking mutex lock
	pthread_mutex_lock(&reader_mutex_lock);
	// decrementing the reader count	
	reader_count = reader_count - 1;
	// if the reader count is zero then give signal to the writer thread to
	// enter into the critical section  
	if (reader_count == 0) {
		pthread_cond_signal(&cond);
	}	
	// releasing mutex lock
	pthread_mutex_unlock(&reader_mutex_lock);		
}	

int main() {
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
	
	printf("\nAll readers-writers threads exited.\n");

	return 0;

}

