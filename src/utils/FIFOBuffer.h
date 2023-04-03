/*
 * FIFOBuffer.h
 *
 * Implementation of a simple FIFO buffer structure with fixed length.
 *
 *  Created on: 27/05/2021
 *      Author: Tim Bullen
 */

#include <cstdint>
#include <memory>

#ifndef FIFOBUFFER_H_
#define FIFOBUFFER_H_

/*
 * Templates
 * class T          The data type that the buffer shall contain elements of.
 */
template <class T>
class FIFOBuffer
{
public:
    /*
     * uint32_t len     The fixed maximum length of the buffer.
     */
    FIFOBuffer(uint32_t len) : len(len) {
        elements = std::unique_ptr<T[]>(new T[len]);
    }

    /*
     * Adds a new element to the buffer
     */
    void add(T const& elem);

    /*
     * Clears all elements in the buffer.
     */
    void clear(void);

    /*
     * Retrieves the element at the specified index.
     */
    T at(uint32_t index) const;

    /*
     * Returns the number of elements in the buffer.
     */
    uint32_t size() const;

    /*
     * Returns the fixed length of the buffer.
     */
    uint32_t length() const;

    /*
     * Return true if the buffer has been filled.
     */
    bool isFull() const;

private:
    std::unique_ptr<T[]> elements;
    uint32_t len;
    uint32_t write_index = 0;
    uint32_t num = 0;
};


template <class T>
void FIFOBuffer<T>::add (T const& elem)
{
    // Increment write index
    write_index = (write_index + 1) % len;

    elements[write_index] = elem;

    if (num < len) {
        num++;
    }
}


template <class T>
T FIFOBuffer<T>::at (const uint32_t index) const
{
    uint32_t read_index = ((write_index + len) - index) % len;

    return elements[read_index];
}


template <class T>
void FIFOBuffer<T>::clear(void)
{
    write_index = 0;
    num = 0;
}


template <class T>
uint32_t FIFOBuffer<T>::size() const
{
    return num;
}


template <class T>
uint32_t FIFOBuffer<T>::length() const
{
    return len;
}


template <class T>
bool FIFOBuffer<T>::isFull() const
{
    return (num == len);
}

#endif /* FIFOBUFFER_H_ */
