#ifndef SINGLETON_H
#define SINGLETON_H

template <typename T>
struct singleton
{
    static T& reference()
    {
        static T t;
        return t;
    }
};

#endif