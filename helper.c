// [Hanfei He] , [hjh5321@psu.edu]    individual

#include "buffer.h"



// ---------------------------------- helper functions ----------------------------------- //

// wrap pthread_mutex_init
void mutex_init_check(pthread_mutex_t* mutex)
{
    if (pthread_mutex_init(mutex, NULL) != 0) {
        exit(EXIT_FAILURE);
    }
}

// wrap pthread_mutex_lock
void mutex_lock_check(pthread_mutex_t* mutex)
{
    if (pthread_mutex_lock(mutex) != 0) {
        exit(EXIT_FAILURE);
    }
}

// wrap pthread_mutex_unlock
void mutex_unlock_check(pthread_mutex_t* mutex)
{
    if (pthread_mutex_unlock(mutex) != 0) {
        exit(EXIT_FAILURE);
    }
}

// wrap pthread_cond_init
void cond_init_check(pthread_cond_t* cond)
{
    if (pthread_cond_init(cond, NULL) != 0) {
        exit(EXIT_FAILURE);
    }
}

// wrap pthread_cond_wait
void cond_wait_check(pthread_cond_t* cond, pthread_mutex_t* mutex)
{
    if (pthread_cond_wait(cond, mutex) != 0) {
        exit(EXIT_FAILURE);
    }
}

// wrap pthread_cond_signal
void cond_signal_check(pthread_cond_t* cond)
{
    if (pthread_cond_signal(cond) != 0) {
        exit(EXIT_FAILURE);
    }
}

// wrap pthread_cond_broadcast
void cond_broadcast_check(pthread_cond_t* cond)
{
    if (pthread_cond_broadcast(cond) != 0) {
        exit(EXIT_FAILURE);
    }
}

// ----------------------------------- helper functions ----------------------------------- //



// Creates a buffer with the given capacity
state_t* buffer_create(int capacity)
{
    state_t* buffer = (state_t*) malloc(sizeof(state_t));
    buffer->fifoQ = (fifo_t *) malloc ( sizeof (fifo_t));
    fifo_init(buffer->fifoQ,capacity);
    buffer->isopen = true;

    // init mutexes and conditional variables
    mutex_init_check(&buffer->chmutex);
    mutex_init_check(&buffer->chclose);
    cond_init_check(&buffer->chconsend);
    cond_init_check(&buffer->chconrec);
    return buffer;
}


// Writes data to the given buffer
// This is a blocking call i.e., the function only returns on a successful completion of send
// In case the buffer is full, the function waits till the buffer has space to write the new data
// Returns BUFFER_SUCCESS for successfully writing data to the buffer,
// CLOSED_ERROR if the buffer is closed, and
// BUFFER_ERROR on encountering any other generic error of any sort
enum buffer_status buffer_send(state_t *buffer, void* data)
{
    enum buffer_status status;

    // lock buffer
    mutex_lock_check(&buffer->chmutex);

    if(!buffer->isopen)
    {
        // buffer is already closed
        mutex_unlock_check(&buffer->chmutex);
        return CLOSED_ERROR;
    }
    int msg_size = get_msg_size(data);

    // wait until there is enough spaces in fifo
    while (msg_size >= fifo_avail_size(buffer->fifoQ)) {
        // cond wait for signal
        cond_wait_check(&buffer->chconsend, &buffer->chmutex);

        // the thread is woken up, check whether buffer closed
        if (!buffer->isopen) {
            mutex_unlock_check(&buffer->chmutex);
            return CLOSED_ERROR;
        }
    }
    
    // now has enough spaces to send
    status = buffer_add_Q(buffer,data);
    if (status == BUFFER_SUCCESS) {
        // send success, wake up thread to receive
        cond_signal_check(&buffer->chconrec);
    }

    // unlock buffer
    mutex_unlock_check(&buffer->chmutex);

    return status;
}
// test_send_correctness 1
// Reads data from the given buffer and stores it in the function’s input parameter, data (Note that it is a double pointer).
// This is a blocking call i.e., the function only returns on a successful completion of receive
// In case the buffer is empty, the function waits till the buffer has some data to read
// Return BUFFER_SPECIAL_MESSSAGE for successful retrieval of special data "splmsg"
// Returns BUFFER_SUCCESS for successful retrieval of any data other than "splmsg"
// CLOSED_ERROR if the buffer is closed, and
// BUFFER_ERROR on encountering any other generic error of any sort

enum buffer_status buffer_receive(state_t* buffer, void** data)
{
    enum buffer_status status;

    // lock buffer
    mutex_lock_check(&buffer->chmutex);

    if(!buffer->isopen)
    {
        // already closed
        mutex_unlock_check(&buffer->chmutex);
        return CLOSED_ERROR;
    }
    
    // wait for messages
    while (buffer->fifoQ->avilSize >= buffer->fifoQ->size) {
        cond_wait_check(&buffer->chconrec, &buffer->chmutex);

        // the thread is woken up, check whether buffer closed
        if (!buffer->isopen) {
            mutex_unlock_check(&buffer->chmutex);
            return CLOSED_ERROR;
        }
    }

    // receive messages
    status = buffer_remove_Q(buffer, data);
    if (status == BUFFER_SUCCESS) {
        // check whether is special message
        char* message = *(char**)data;
        if (strcmp(message, "splmsg") == 0) {
            status = BUFFER_SPECIAL_MESSSAGE;
        }

        // signal a thread to send message
        cond_signal_check(&buffer->chconsend);
    }

    // unlock buffer
    mutex_unlock_check(&buffer->chmutex);

    return status;
}


// Closes the buffer and informs all the blocking send/receive/select calls to return with CLOSED_ERROR
// Once the buffer is closed, send/receive/select operations will cease to function and just return CLOSED_ERROR
// Returns BUFFER_SUCCESS if close is successful,
// CLOSED_ERROR if the buffer is already closed, and
// BUFFER_ERROR in any other error case
enum buffer_status buffer_close(state_t* buffer)
{

    // lock mutex for close and buffer
    mutex_lock_check(&buffer->chclose);
    mutex_lock_check(&buffer->chmutex);

    if(!buffer->isopen)
    {
        // already closed, unlock in reverse order
        mutex_unlock_check(&buffer->chmutex);
        mutex_unlock_check(&buffer->chclose);
        return CLOSED_ERROR;
    }
    buffer->isopen = false;

    // wake up all threads to exit
    cond_broadcast_check(&buffer->chconsend);
    cond_broadcast_check(&buffer->chconrec);

    // unlock buffer and mutex for close
    mutex_unlock_check(&buffer->chmutex);
    mutex_unlock_check(&buffer->chclose);

    return BUFFER_SUCCESS;
    
}

// Frees all the memory allocated to the buffer , using own version of sem flags
// The caller is responsible for calling buffer_close and waiting for all threads to finish their tasks before calling buffer_destroy
// Returns BUFFER_SUCCESS if destroy is successful,
// DESTROY_ERROR if buffer_destroy is called on an open buffer, and
// BUFFER_ERROR in any other error case

enum buffer_status buffer_destroy(state_t* buffer)
{
    
    if(buffer->isopen)
    {
        return DESTROY_ERROR;
    }
    fifo_free(buffer->fifoQ);

    // free mutexes and conditional variables
    pthread_mutex_destroy(&buffer->chmutex);
    pthread_mutex_destroy(&buffer->chclose);
    pthread_cond_destroy(&buffer->chconsend);
    pthread_cond_destroy(&buffer->chconrec);
    free(buffer);
    return BUFFER_SUCCESS;
}