#include <iostream>
#include <fstream>
#include <string>
#include <queue>
#include <stdlib.h>
#include <pthread.h>

using namespace std;

/*
2. Producer-Consumer
Design a producer class, a consumer class and a buffer class. Producer reads from a file and writes into a
buffer and consumer reads from the buffer and writes into a file. Make the classes thread safe.

Using multiple producers merge different files into a single output file. Ask the user for input files. After
3 files, start dumping the combined content into a single put file. Continue to take input files from the
user and keep adding them to the process. Ensure that no single producer reading from an infinite
source (e.g. /dev/urandom) causes output application to ignore data from the other sources.
*/

//mutex to lock buffer
pthread_mutex_t lock;


/*
 * Class: Buffer
 *
 * Purpose: Buffer class will be shared by both producer and consumer classes. Producer write item
 *			to the buffer and consumer read item from buffer
 * 
 * Class variable: buf - it will hold the item produce by producer in FIFO fashion
 *				   capacity - maximum item can hold buffer 	 
 */
class Buffer
{
	queue<char> buf;
	const unsigned int capacity;

	public:
		Buffer(unsigned int size);
		void produce(char produce_item);
		char consume();
};

/*
 * Function: Buffer::Buffer()
 *
 * Purpose: Buffer class constructor it will initialize buffer capacity 
 *
 * Arguments: size - capacity of buffer 
 *
 * Returns: None
 */
Buffer::Buffer(unsigned int size):capacity(size)
{

}

/*
 * Function: Buffer::produce()
 *
 * Purpose: it will write produce itemed into buffer produced by producer 
 *
 * Arguments: produce_item - produced by producer class
 *
 * Returns:  void 
 */
void Buffer::produce(char produce_item)
{
	//wait until buffer is not free
	while(buf.size() == capacity);
	
	//aquired mutex lock
	pthread_mutex_lock(&lock);
	    //wrote item into buffer produce by producer
		buf.push(produce_item);
	//relase mutex lock
	pthread_mutex_unlock(&lock);
}

/*
 * Function: Buffer::consume()
 *
 * Purpose: it will consume item from buffer 
 *
 * Arguments: None
 *
 * Returns:  consume_item 
 */
char Buffer::consume()
{
	while(buf.size() == 0);

	pthread_mutex_lock(&lock);
		char consume_item = buf.front();
		buf.pop();
	pthread_mutex_unlock(&lock);
	
	return consume_item;
}

/*
 * Class: Producer
 *
 * Purpose: Producer class will read item from source file and write that item into buffer
 *			
 * Class variable: fin - ifstream object to open file into read mode
 *				   source_file - contains name of source file  	 
 */
class Producer
{
	ifstream fin;
	string source_file;
	public:
		Producer();
		Producer(const string& file_name);
		void read(Buffer& buf);
		~Producer();
};

/*
 * Function: Producer::Producer()
 *
 * Purpose: Producer constructor, it will take input file and open it into read mode, 
 			raised exception if file Doesn't exist or don't have read permission !  
 *
 * Arguments: input file
 *
 * Returns:  None 
 */
Producer::Producer(const string& file_name)
{
	source_file = file_name;
	//opening file into reading mode 
	fin.open(source_file.c_str());
	//raied exception if file doesn't exist or don't have read permission !
	if(!fin.is_open())
		throw string("file Doesn't Exist or Don't have read permission !");
}

/*
 * Function: Producer::read()
 *
 * Purpose: read item from input file and write it into buffer   
 *
 * Arguments: Buffer object 
 *
 * Returns:  void 
 */
void Producer::read(Buffer& buf)
{
	char ch;

	//checked file is already opened or not ?
	if(fin.is_open())
	{
		//read input file till the end 
		while(fin.good())
		{
			//get item from input file
    		fin.get(ch);
    		//produced item to buffer from input file
    		buf.produce(ch);
    	}
	}	
}  

/*
 * Function: Producer::~Producer()
 *
 * Purpose: Producer destructor, it will closed input file as soon producer obj destroyed.  
 *
 * Arguments: None
 *
 * Returns:  None 
 */
Producer::~Producer()
{
	fin.close();
}

/*
 * Class: Consumer
 *
 * Purpose: Consumer class will read item from buffer write back it into destination file
 *			
 * Class variable: fout - ofstream object to write data into destination file
 *				   des_file - contains name of destination file  	 
 */
class Consumer
{
	ofstream fout;
	string des_file;
	public:
		Consumer();
		Consumer(const string& file_name);
		void write(Buffer& buf);
		~Consumer();
};

/*
 * Function: Consumer::Consumer()
 *
 * Purpose: Consumer constructor, it will take the output file and open it into write mode,
 *			raised exception if directory don't have write permission or file doesn't created  
 *
 * Arguments: output file
 *
 * Returns:  None 
 */
Consumer::Consumer(const string& file_name)
{
	des_file = file_name;
	fout.open(des_file.c_str());
	
	if(!fout.is_open())
			throw string("Doesn't created, directory don't have permission !");
}

/*
 * Function: Consumer::write()
 *
 * Purpose: read item from buffer and write it into output file   
 *
 * Arguments: Buffer object 
 *
 * Returns:  void 
 */
void Consumer::write(Buffer& buf)
{
	char ch;
	//checked output file is already opened or not ?
	if(fout.is_open())
	{
		//read item from buffer until producer produces item
		while((ch = buf.consume()) != EOF)
		{
			//write item to output file
			fout<<ch;
		}
	}	
}  

/*
 * Function: Consumer::~Consumer()
 *
 * Purpose: Consumer destructor, it will closed output file as soon consumer obj destroyed.  
 *
 * Arguments: None
 *
 * Returns:  None 
 */
Consumer::~Consumer()
{
	fout.close();
}


/*
it will hold buffer object and file name and it will be passed
function as argument with every thread call 
*/
struct Pairs
{
	Buffer *buf;
	string file;
};

/*
 * Function: Producer_thread()
 *
 * Purpose: it will create producer object and call read function 
 *			to read item from input file 
 *
 * Arguments: Pairs obj
 *
 * Returns:  NULL
 */ 
void* producer_thread(void *pair)
{
	Pairs *mypair = (Pairs*)pair;
	
	string input_file = (*mypair).file;
	
	//raised exception of file doesn't exist or don't have read permission
	try
	{
		//create producer object
		Producer p1(input_file.c_str());
		//calling read funtion of producer class
		p1.read(*((*mypair).buf));
	}
	catch(string& e)
	{
		cout<<"\nException caused : "<<input_file<<" "<<e<<endl;
	}	

	return NULL;
}

/*
 * Function: Consumer_thread()
 *
 * Purpose: it will create consumer object and call write function 
 *			to write data to output file from buffer
 *
 * Arguments: Pairs obj 
 *
 * Returns:  NULL
 */ 
void* consumer_thread(void *pair)
{
	
	Pairs *mypair = (Pairs*)pair;
	
	string output_file = (*mypair).file;
	//raised exception if output directory don't have write permission
	try
	{
		//create Cosumer object
		Consumer c1(output_file.c_str());
		//calling write function of consumer object
		c1.write((*(*mypair).buf));
	}
	catch(string& e)
	{
		cout<<"\nException caused : "<<output_file<<" "<<e<<endl;
	}	
	
	return NULL;
}

/*
 * Function: main()
 *
 * Purpose: Execution will start from here first it will create buffer class object 
 *			then create producer and consumer thread   
 *
 * Arguments: None
 *
 * Returns:  0
 */ 
int main(int argc, char** argv) 
{
	//Pairs structure will hold buf object and file and pass to thread function
	Pairs *pair = new Pairs();

	//create buffer with 10 character block capacity
	Buffer *buf =  new Buffer(10);

	int thread_counter = 0;
	
	string input_file;
	//consumer thread id's
	pthread_t consumer_thread_id;
	
	//producer thread id's maximum limit 100 allowed
	pthread_t producer_thread_id[100];
	
	//take file from user till user doesn't enter NULL
	while(true)
	{
		//Enter file or type <NULL> to exit from merging process 
		cout<<"\nEnter input file or type < NULL > to exit : ";
		cin>>input_file;


		//break loops if user completed their input file
		if(input_file == "NULL")
		{
			//write EOF (-1) into buffer to ensure that there is no input file left to write into buffer 
			//Hence producer has stop to produces items and now consumer 
			//see EOF into buffer immediatlly it will stop.  
			buf->produce(EOF);
			break;
		}
		pair->buf = buf;
		pair->file = input_file;
		//create producer thread for each input file
		pthread_create(&producer_thread_id[thread_counter],NULL,producer_thread,(void*)pair);
		
		//keep track of producer thread count
		thread_counter++;
		
		//As soon as user enter 3 files start consumer thread to consume item from buffer  
		if(thread_counter == 3)
		{
			Pairs *consumer_pair = new Pairs();
			consumer_pair->buf = buf;
			//default output file
			consumer_pair->file = "output";
			//create consumer thread then running it 
			pthread_create(&consumer_thread_id,NULL,consumer_thread,(void*)consumer_pair);
		}

	}	

	//joining consumer thread
	pthread_join(consumer_thread_id,NULL);

	//joining all producer thread
	for(int i=0;i<thread_counter;++i)
		pthread_join(producer_thread_id[i],NULL);
	

	return 0;
}
