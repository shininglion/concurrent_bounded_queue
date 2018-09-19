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


namespace dt
{
    // Concurrent bounded queue is a fixed-size and thread-safe queue that allows multiple threads reading/writing simultaneously.
    // The following class provides a lock-free implemetation
    template <typename T>
    class ConcurrentBoundedQueueLF
    {
    public:
        typedef T value_type;
        typedef T& reference;
        typedef const T& const_referece;

        ConcurrentBoundedQueueLF(const size_t desired_size);
        ~ConcurrentBoundedQueueLF() = default;
        ConcurrentBoundedQueueLF(const ConcurrentBoundedQueueLF&) = delete;
        ConcurrentBoundedQueueLF& operator= (const ConcurrentBoundedQueueLF&) = delete;

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
        void emplace(Args&& ... args);

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
        bool try_emplace(Args&& ... args);

        // reutnr the number of values currently held by the queue
        size_t size() const noexcept;
        // Maximum number of values that the queue can hold.
        // Equivalent to the value 'desired_size' given in constructor
        size_t capacity() const noexcept;
        // return true if the queue held no value; false otherwise
        size_t empty() const noexcept;
        void clear();

    private:
        std::vector<T>  m_storage;
        std::atomic_int m_head, m_tail;
    };
}


#include "cbqueue.hpp"


#endif
