/*
 * =====================================================================================
 *
 *       Filename:  cbqueue.hpp
 *
 *    Description:  Implementation of concurrent bounded queue
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


#ifndef _CONCURRENT_BOUNDED_QUEUE_HPP_
#define _CONCURRENT_BOUNDED_QUEUE_HPP_


#include <utility>
#include <cstring>


namespace dt
{

    template <typename T>
    ConcurrentBoundedQueue<T>::ConcurrentBoundedQueue(const size_t desired_size)
    : storage(desired_size + 1), head(0), tail(0), storage_capacity(desired_size), base_value(desired_size + 1), end_signal(false) {}


    template <typename T>
    size_t ConcurrentBoundedQueue<T>::size() const noexcept
    {
        lock_type head_lock(this->head_mutex);
        lock_type tail_lock(this->tail_mutex);
        return (head - tail);
    }


    template <typename T>
    bool ConcurrentBoundedQueue<T>::empty() const noexcept
    {
        lock_type head_lock(this->head_mutex);
        lock_type tail_lock(this->tail_mutex);
        return (head == tail);
    }


    template <typename T>
    void ConcurrentBoundedQueue<T>::clear() noexcept
    {
        lock_type head_lock(this->head_mutex);
        lock_type tail_lock(this->tail_mutex);
        this->head = 0;
        this->tail = 0;
        this->storage_capacity = 0;
        this->base_value = 0;
        this->storage.clear();
        this->end_signal = true;
        this->read_cv.notify_all();
        this->write_cv.notify_all();
    }


    template <typename T>
    inline void ConcurrentBoundedQueue<T>::update_ptr()
    {
        if (this->tail >= this->base_value) {
            lock_type head_lock(this->head_mutex);
            lock_type tail_lock(this->tail_mutex);
            this->head -= this->base_value;
            this->tail -= this->base_value;
        }
    }


    template <typename T>
    bool ConcurrentBoundedQueue<T>::try_push(const T& src)
    {
        lock_type head_lock(this->head_mutex);
        const size_t index = this->head;
        if ((index - this->tail) >= this->capacity()) { return false; }

        this->storage.at((index + 1) % this->base_value) = src;
        ++this->head;
        this->read_cv.notify_one();
        return true;
    }


    template <typename T>
    bool ConcurrentBoundedQueue<T>::try_push(T&& src)
    {
        lock_type head_lock(this->head_mutex);
        const size_t index = this->head;
        if ((index - this->tail) >= this->capacity()) { return false; }

        this->storage.at((index + 1) % this->base_value) = src;
        ++this->head;
        this->read_cv.notify_one();
        return true;
    }


    template <typename T>
    template <typename ... Args>
    inline bool ConcurrentBoundedQueue<T>::try_emplace(Args&& ... args)
    {
        return this->try_push(T(std::forward<Args>(args)...));
    }


    template <typename T>
    bool ConcurrentBoundedQueue<T>::try_pop(T& dest)
    {
        lock_type tail_lock(this->tail_mutex);
        const size_t index = this->tail;
        if (this->head == index) { return false; }

        dest = this->storage.at((index + 1) % this->base_value);
        this->storage.at((index + 1) % this->base_value) = T();
        ++this->tail;
        this->update_ptr();
        this->write_cv.notify_one();
        return true;
    }


    template <typename T>
    void ConcurrentBoundedQueue<T>::push(const T& src)
    {
        while ((this->try_push(src) == false) && (this->end_signal == false)) {
            std::unique_lock<std::mutex> lock(write_mutex);
            write_cv.wait(lock, [this]() -> bool { return (this->size() < this->capacity()) || this->end_signal; });
        }
    }


    template <typename T>
    void ConcurrentBoundedQueue<T>::push(T&& src)
    {
        while ((this->try_push(std::move(src)) == false) && (this->end_signal == false)) {
            std::unique_lock<std::mutex> lock(this->write_mutex);
            this->write_cv.wait(lock, [this]() -> bool { return (this->size() < this->capacity()) || this->end_signal; });
        }
    }


    template <typename T>
    template <typename ... Args>
    inline void ConcurrentBoundedQueue<T>::emplace(Args&& ... args)
    {
        this->push(T(std::forward<Args>(args)...));
    }


    template <typename T>
    void ConcurrentBoundedQueue<T>::pop(T& dest)
    {
        while ((this->try_pop(dest) == false) && (this->end_signal == false)) {
            std::unique_lock<std::mutex> lock(this->read_mutex);
            this->read_cv.wait(lock, [this]() -> bool { return (this->empty() == false) || this->end_signal; });
        }
    }

}


#endif
