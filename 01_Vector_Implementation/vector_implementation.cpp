#include <iostream>
#include <cmath>
#include <cstdlib>     
#include <ctime> 
#include <climits>
#include <ctime>

using namespace std;

/*
1. Int vector implementation
C arrays have size determined at compile time. Implement a C++ class that provides you with objects
that behave like arrays of int except that their size is determined at run time. Explain the reasons for
your design decisions.
*/

class Vector
{
    int *buffer;
    unsigned int _size;
    unsigned int _capacity;
    int factor;

public:
	
    inline Vector(); 
    inline Vector(const unsigned int size);
    inline Vector(const unsigned int size, int value);
    void push_back(const int& value);
    inline void pop_back();
    inline unsigned int size() const;
    inline unsigned int capacity() const;
    inline int& front();
    inline int& back();
    inline int* begin();
    inline int* end();
    inline bool empty() const;
    int& operator[](unsigned int index);
    void reserve(unsigned int capacity);
    void resize(unsigned int size);
    inline void clear();
    inline ~Vector();   
};


/*
Defalut contructor it will initialized 
data member with default value
*/
Vector::Vector() 
{
   	buffer = NULL;
   	_capacity = 0;
   	_size = 0;
   	factor = 0;
}

/*
Parameterized contructor it will allocate memory  
with next power of 2 of given size in buffered
*/
Vector:: Vector(unsigned int size) 
{
   	_size = size;
   	factor = ceil(log((double) size) / log(2.0));
   	_capacity = 1 << factor;
   	buffer = new int[_capacity];
}

/*
Parameterized contructor it will allocate memory  
with next power of 2 of given size in buffred 
and initialzed with given value 
*/
Vector:: Vector(unsigned int size, int value) 
{
   	_size = size;
   	factor = ceil(log((double) size) / log(2.0));
   	_capacity = 1 << factor;
   	buffer = new int[_capacity];

   	//initizing all memery with given value
   	for(unsigned int i=0;i<_size;++i)
   		buffer[i] = value;
}

/*
First it will check free space, if available it will insert value 
othereise it will double the size of buffer then insert value 
*/
void Vector::push_back(const int& value) 
{
    /*
        buffer will grow in double the size as needed.
        This is so that if we are inserting n items at most only O(log n) regrowths are performed
        and at most O(n) space is wasted.
    */
    if (_size >= _capacity) 
    {
        //reserve space by double if buffer is full  
        reserve(1 << factor);
        factor++;
    }
    buffer [_size++] = value;
}

/*
it will decrease the sise of the vector by 1
if size <= 0 it will raised exception !
*/
inline void Vector::pop_back() 
{
	//if there is not any element to pop_back raised exception
	if(_size <= 0)
		throw string("vector is empty !");
	else
		_size--;	
}


/*
return reference of first element, raised exception if vector is empty
*/
inline int& Vector::front() 
{

	//if there is not any element to pop_back raised exception
	if(_size <= 0)
		string("vector is empty !");
	
	return buffer[0];
}

/*
return reference of last element
*/
inline int& Vector::back() 
{
    return buffer[_size - 1];
}

/*
return base address of the vector
*/
inline int* Vector::begin() 
{
    return buffer;
}

/*
return just next address of last element of the vector   
*/
inline int* Vector::end() 
{
    return (buffer + _size);
}

/*
return size of vector i.e; no. of element 
currently in the vector
*/
inline unsigned int Vector::size() const 
{
    return _size;
} 

/*
It will give the maximum size of vector that has been 
allocated till now, In this implementation it always power of 2
*/
inline unsigned int Vector::capacity() const 
{
    return _capacity;
}

/*
Overloading subs script operator([]) that will return 
reference of the element on given index, 
raised index out of bound exception for invalid index 
*/
inline int& Vector::operator[](unsigned int index) 
{
    
    //if index less than 0
	if(index < 0)  
		throw string("negative index !");
	//index greater than size
	else if(index >= _size)
		throw string("index larger than vector size !");

	//if index with the range
	return buffer[index];
}

//check where vector obj empty or not 
inline bool Vector:: empty() const 
{
   	return _size == 0;
}

/*
It informs the vector of a planned change in size. 
This enables the vector to manage the storage allocation accordingly. 
reserve does not change the size of the vector and reallocation happens 
if and only if the current capacity is less than the argument of reserve
*/
void Vector::reserve(unsigned int capacity) 
{
    int * newBuffer = new int[capacity];

    for (unsigned int i = 0; i < _size; i++)
        newBuffer[i] = buffer[i];

    _capacity = capacity;
    delete[] buffer;
    buffer = newBuffer;
}

/*
It informs the vector of a planned change in size. 
*/
void Vector::resize(unsigned int size) 
{
    factor = ceil(log((double) size) / log(2.0));
    reserve(1 << factor);
    _size = size;
}

/*
cleared the content vector object 
*/
inline void Vector::clear() 
{
    _capacity = 0;
    _size = 0;
    buffer = NULL;
    factor = 0;
}

/*
vector destructor automatically called when object will go out of scope
it will delete memory reserved by object
*/
inline Vector::~Vector() 
{
    delete[] buffer;
}




int main()
{
	
	/* 
	Test Case 1 : Create empty vector then push back some value and print it's size and value 
 	Again, pop back some value and print it's size and value 
	*/

	cout<<"<<---------- Test Case : 1 ---------->>";

	//create empty vector
	Vector vector_test_case_1;
	
	vector_test_case_1.push_back(100);
	vector_test_case_1.push_back(200);
	vector_test_case_1.push_back(300);
	vector_test_case_1.push_back(400);
	vector_test_case_1.push_back(500);
	vector_test_case_1.push_back(600);
	
	//exception will raised if try to access invalid index
	try
	{
		vector_test_case_1[3] = 900;
	}
	catch(string& e)
	{
		cout<<"Exception caused : "<<e<<endl;
	}	
	
	cout<<"\n\nSize of the vector : "<<vector_test_case_1.size();
	cout<<"\n\nContent of vector :"<<endl;

	//printing content of vector
    for(unsigned int i=0;i < vector_test_case_1.size();++i)
        cout<<vector_test_case_1[i]<<" ";

    //exception will raised if try to pop back empty vector 
	try
	{
		//pop back some value 
		vector_test_case_1.pop_back();
		vector_test_case_1.pop_back();
		vector_test_case_1.pop_back();
	}
	catch(string& e)
	{
		cout<<"Exception caused : "<<e<<endl;
	}	

	cout<<"\n\nSize of the vector : "<<vector_test_case_1.size();
	cout<<"\n\nFront value of vector : "<<vector_test_case_1.front();
	cout<<"\n\nBack value of vector : "<<vector_test_case_1.back();
        cout<<"\n\nContent of vector :"<<endl;

    //printing content of vector after pop back
    for(unsigned int i=0;i<vector_test_case_1.size();++i)
        cout<<vector_test_case_1[i]<<" ";



	/*
        Test Case 2 : Create vector of size 10 and initilized with default value and print it's size and value
        Again, pop back some value then push back some value and print it's size and value
        */
	
	cout<<"\n\n<<---------- Test Case : 2 ---------->>";

	//create vector of size 10 and initialized all value with 100
	Vector vector_test_case_2(10,100);
	
	cout<<"\n\nSize of the vector : "<<vector_test_case_2.size();
        cout<<"\n\nContent of vector :"<<endl;

    //printing content of vector
    for(unsigned int i=0;i < vector_test_case_2.size();++i)
        cout<<vector_test_case_2[i]<<" ";

	try
	{
		//pop back some value
		vector_test_case_2.pop_back();
    	        vector_test_case_2.pop_back();
    	        vector_test_case_2.pop_back();
		vector_test_case_2.pop_back();
    	        vector_test_case_2.pop_back();
    	        vector_test_case_2.pop_back();
        }
        catch(string& e)
	{
		cout<<"Exception caused : "<<e<<endl;
	}
	
    //push back some value
    vector_test_case_2.push_back(300);
    vector_test_case_2.push_back(400);
    vector_test_case_2.push_back(500);

    cout<<"\n\nSize of the vector : "<<vector_test_case_2.size();
    cout<<"\n\nFront value of vector : "<<vector_test_case_2.front();
    cout<<"\n\nBack value of vector : "<<vector_test_case_2.back();
    cout<<"\n\nContent of vector :"<<endl;

    //printing content of vector
    for(unsigned int i=0;i < vector_test_case_2.size();++i)
        cout<<vector_test_case_2[i]<<" ";


	cout<<endl;
	
	return 0;
}
