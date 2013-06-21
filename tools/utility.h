#ifndef UTILITY_H
#define UTILITY_H

#include <QHash>
#include <QDataStream>
#include <QDebug>

class Utility
{
public:
    template <class Key, class T>
    static QHash<Key,T> derefItems(const QHash<Key,T*> input)
    {
        QHash<Key,T> out;

        QHashIterator<Key,T*> it(input);

        while(it.hasNext())
        {
            it.next();
            out[it.key()] = *it.value();
        }

        return out;
    }

    template <class Key, class T>
    static QHash<Key,T*> refItems(const QHash<Key,T> input)
    {
        QHash<Key,T*> out;

        QHashIterator<Key,T> it(input);

        while(it.hasNext())
        {
            it.next();

            T* tmp = new T();
            *tmp = it.value();

            out[it.key()] = tmp;
        }

        return out;
    }

    template <class Key, class T>
    static void saveHash(QDataStream &out, const QHash<Key,T*> hash)
    {
        qint32 size = hash.size();

        out << size;
        T* t;

        QHashIterator<Key,T*> it(hash);

        while(it.hasNext())
        {
            it.next();

            out << it.key();

            t = it.value();
            out << T();
        }

    }

    template <class Key, class T>
    static QHash<Key,T*> loadHash(QDataStream &in)
    {
        qint32 size;
        QHash<Key,T*> ret;
        Key k;
        T* t;

        in >> size;


        for(int i = 0; i < size; ++i)
        {
            in >> k;

            t = new T();
            //in >> (*t);

            ret.insert(k,t);
        }

        return ret;
    }

private: //only static class
    Utility() {}
};

#endif // UTILITY_H
