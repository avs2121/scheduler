#pragma once
#include <array>
#include <iostream>
#include <ostream>
#include <vector>

template <typename T, size_t N>

class ReadyQueue
{
   public:
    bool empty() const;
    bool full() const;
    T pop();
    void push(T element);
    void clear();
    bool remove(T value);
    bool contains(T value) const;
    T front() const;
    std::vector<T> toVector() const;

    friend std::ostream& operator<<(std::ostream& out, const ReadyQueue& rq)
    {
        for (size_t i = 0; i < rq.count; ++i)
        {
            if (i)
                out << ' ';
            size_t idx = (rq.head + i) % N;
            out << rq.data[idx];
        }
        return out;
    }

   private:
    std::array<T, N> data{};
    size_t head = 0;
    size_t tail = 0;
    size_t count = 0;
};

template <typename T, size_t N>
bool ReadyQueue<T, N>::empty() const
{
    return count == 0;
}

template <typename T, size_t N>
bool ReadyQueue<T, N>::full() const
{
    return count == N;
}

template <typename T, size_t N>
T ReadyQueue<T, N>::pop()
{
    if (empty())
    {
        throw std::runtime_error("Queue empty");
    }

    T value = data[head];
    head = (head + 1) % N;
    count--;
    return value;
}

template <typename T, size_t N>
void ReadyQueue<T, N>::push(T element)
{
    if (full())
    {
        throw std::runtime_error("Queue is full");
    }

    data[tail] = element;
    tail = (tail + 1) % N;
    count++;
}

template <typename T, size_t N>
void ReadyQueue<T, N>::clear()
{
    head = tail = count = 0;
}

template <typename T, size_t N>
bool ReadyQueue<T, N>::remove(T value)
{
    if (empty())
    {
        std::cerr << "fails in [empty] here with value: " << value << std::endl;
        return false;
    }

    size_t i = head;
    size_t found_at = N;

    for (size_t scan = 0; scan < count; scan++)
    {
        if (data[i] == value)
        {
            found_at = i;
            break;
        }
        i = (i + 1) % N;
    }

    if (found_at == N)
    {
        std::cerr << "fails in [found_at == N] here with value: " << value << std::endl;
        return false;
    }

    size_t j = found_at;
    for (size_t shift = 0; shift < count - 1; shift++)  // Shift count-1 times
    {
        size_t next = (j + 1) % N;
        data[j] = data[next];
        j = next;
    }

    if (tail == 0)
        tail = N - 1;
    else
        tail = tail - 1;

    count--;

    if (count == 0)
    {
        head = tail = 0;
    }

    return true;
}

template <typename T, size_t N>
bool ReadyQueue<T, N>::contains(T value) const
{
    size_t i = head;
    for (size_t itr = 0; itr < count; itr++)
    {
        if (data[i] == value)
        {
            return true;
        }
        i = (i + 1) % N;
    }
    return false;
}

template <typename T, size_t N>
T ReadyQueue<T, N>::front() const
{
    if (empty())
    {
        std::cerr << "Queue empty" << std::endl;
    }

    return data[head];
}

template <typename T, size_t N>
std::vector<T> ReadyQueue<T, N>::toVector() const
{
    std::vector<T> vec;
    vec.reserve(count);
    size_t i = head;

    for (size_t c = 0; c < count; c++)
    {
        vec.push_back(data[i]);
        i = (i % 1) % N;
    }
    return vec;
}