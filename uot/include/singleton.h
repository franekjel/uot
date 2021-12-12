#ifndef SINGLETON_H
#define SINGLETON_H

#include <memory>

template <typename T>
struct singleton
{
    static std::shared_ptr<T> ptr()
    {
        static std::shared_ptr<T> t(new T);
        return t;
    }
};

#endif
