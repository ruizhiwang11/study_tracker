#include <array>
#include <bits/stdc++.h>
#include <cassert>
#include <cstddef>
#include <optional>
#include <print>

template <typename T, size_t Capacity>
class Ringbuffer
{
private:
    size_t m_send_seq{0};
    size_t m_rece_seq{0};
    std::array<T, Capacity> m_buffer{};

public:
    
    size_t index(size_t seq){
        
        // return seq &(Capacity -1); only if it's pow 2
        return seq % Capacity;
    }

    bool isFull() const 
    {
        return m_send_seq - m_rece_seq == Capacity;
    }

    bool isEmpty() const
    {
        return m_send_seq ==  m_rece_seq;
    }

    bool push(T &&t)
    {
        if(isFull())
        {
            return false;
        }
        m_buffer[index(m_send_seq)] = t;
        m_send_seq++;
        return true;
    }

    std::optional<T> pop()
    {
        if(isEmpty())
        {
            return std::nullopt;
        }
        T val = m_buffer[index(m_rece_seq)];
        m_rece_seq++;
        return val;
    }

};

int main()
{
    Ringbuffer<int, 5> buf;
    std::println("{}", buf.isEmpty());
    std::println("{}", buf.isFull());
    assert(buf.push(1));
    assert(buf.push(2));
    assert(buf.push(3));
    assert(buf.push(4));
    // assert(not buf.push(5));
    // assert(buf.isFull());
    assert(buf.pop() == 1);
    assert(buf.pop() == 2);
    assert(buf.pop() == 3);
    assert(buf.pop() == 4);

}