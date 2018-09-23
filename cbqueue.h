/*
 * =====================================================================================
 *
 *       Filename:  cbqueue.h
 *
 *    Description:  Protoypes of Concurrent Bounded Queue (CBQueue)
 *
 *        Version:  1.0
 *        Created:  2018/09/19 (yyyy/mm/dd)
 *       Revision:  none
 *       Compiler:  g++ (C++14)
 *
 *         Author:  lionking
 *   Organization:  None
 *
 * =====================================================================================
 */


#ifndef _CONCURRENT_BOUNDED_QUEUE_H_
#define _CONCURRENT_BOUNDED_QUEUE_H_


#include <vector>
#include <atomic>
#include <mutex>
#include <condition_variable>


namespace dt
{
    // Concurrent bounded queue is a fixed-size and thread-safe queue that allows multiple threads reading/writing simultaneously.
    template <typename T>
    class ConcurrentBoundedQueue
    {
    public:
        typedef T value_type;
        typedef T& reference;
        typedef const T& const_referece;

        ConcurrentBoundedQueue(const size_t desired_size);
        ~ConcurrentBoundedQueue() = default;
        ConcurrentBoundedQueue(const ConcurrentBoundedQueue&) = delete;
        ConcurrentBoundedQueue& operator= (const ConcurrentBoundedQueue&) = delete;

        // Maximum number of values that the queue can hold.
        // Equivalent to the value 'desired_size' given in constructor
        inline size_t capacity() const noexcept { return storage_capacity; }
        // reutnr the number of values currently held by the queue
        size_t size() const noexcept;
        // return true if the queue held no value; false otherwise
        bool empty() const noexcept;
        // return true if the queue cannot push any data; false otherwise
        bool full() const noexcept;
        void clear() noexcept;

        /****************************************************************************************
         * Following functions are blocking calls, and calling thread will wait until available *
         ****************************************************************************************/
        // Waits until size < capacity, and then pushes a copy of src onto back of the queue.
        void push(const T& src);
        // Similar to above, but move src instead.
        void push(T&& src);
        // Waits until a value becomes available and pops it from the queue. Assigns it to dest. Destroys the original value.
        void pop(T& dest);
        // Waits until size < capacity, and then pushes a new element into the queue. The new element is constructed with given arguments.
        template <typename ... Args>
        inline void emplace(Args&& ... args);

        /**********************************************************************************************************************
         * Following functions are non-blocking calls, and calling thread can continue no matter current call success or fail *
         **********************************************************************************************************************/
        // If size < capacity, pushes a copy of src onto back of the queue. Otherwise does nothing.
        // Return value: true if a copy was pushed; false otherwise.
        bool try_push(const T& src);
        // Similar to above, but move src instead.
        bool try_push(T&& src);
        // If a value is available, pops it from the queue, assigns it to dest, and destroys the original value. Otherwise does nothing.
        // Return value: true if a copy was poped; false otherwise.
        bool try_pop(T& dest);
        // If size < capacity, pushes a new element into the queue. The new element is constructed with given arguments.
        // Return value: true if a new element was pushed; false otherwise.
        template <typename ... Args>
        inline bool try_emplace(Args&& ... args);

    private:
        typedef std::lock_guard<std::recursive_mutex> lock_type;

        inline void update_ptr();

        std::vector<T>                  storage;
        std::atomic_size_t              head, tail, storage_capacity, base_value;
        mutable std::recursive_mutex    head_mutex, tail_mutex;
        std::mutex                      read_mutex, write_mutex;
        std::condition_variable         read_cv, write_cv;
        std::atomic_bool                end_signal;
    };
}


#include "cbqueue.hpp"


#endif
