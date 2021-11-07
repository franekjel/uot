#ifndef UNCOPYABLE_H
#define UNCOPYABLE_H

struct uncopiable
{
   private:
    uncopiable(const uncopiable &uc);

    const uncopiable &operator=(const uncopiable &uc);

   protected:
    uncopiable() {}
};

#endif