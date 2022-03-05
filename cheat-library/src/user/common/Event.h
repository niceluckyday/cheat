#pragma once
#include <list>

#ifdef NDEBUG
#  define assert(condition) ((void)0)
#else
#  define assert(condition) /*implementation defined*/
#endif

template<class ...TParams>
class AbstractEventHandler
{
public:
    virtual bool call(TParams... params) = 0;
protected:
    AbstractEventHandler() {}
};

template<class ...TParams>
class TEvent
{
    using TEventHandler = AbstractEventHandler<TParams...>;
public:
    TEvent() :
        m_handlers()
    { }

    ~TEvent()
    {
        for (TEventHandler* oneHandler : m_handlers)
            delete oneHandler;
        m_handlers.clear();
    }

    bool operator()(TParams... params)
    {
        bool continued = true;
        for (TEventHandler* oneHandler : m_handlers)
            continued = continued && oneHandler->call(params...);
        return continued;
    }

    void operator+=(TEventHandler& eventHandler)
    {
        m_handlers.push_back(&eventHandler);
    }
private:
    std::list<TEventHandler*> m_handlers;
};

template<class TObject, class ...TParams>
class MethodEventHandler : public AbstractEventHandler<TParams...>
{
    using TMethod = bool(TObject::*)(TParams...);
public:
    MethodEventHandler(TObject& object, TMethod method) :
        AbstractEventHandler<TParams...>(),
        m_object(object),
        m_method(method)
    {
        assert(m_method != nullptr);
    }
    virtual bool call(TParams... params) override final
    {
        return (m_object.*m_method)(params...);
    }
private:
    TObject& m_object;
    TMethod m_method;
};

template<class ...TParams>
class FreeMethodEventHandler : public AbstractEventHandler<TParams...> 
{
    using TMethod = bool(*)(TParams...);
public:
    FreeMethodEventHandler(TMethod method) :
        AbstractEventHandler<TParams...>(),
        m_method(method)
    {
        assert(m_method != nullptr);
    }
    virtual bool call(TParams... params) override final
    {
        return (*m_method)(params...);
    }
private:
    TMethod m_method;
};

template<class TObject, class ...TParams>
AbstractEventHandler<TParams...>& createMethodEventHandler(TObject& object, void(TObject::* method)(TParams...))
{
    return *new MethodEventHandler<TObject, TParams...>(object, method);
}

template<class ...TParams>
AbstractEventHandler<TParams...>& createFreeMethodEventHandler(bool(*method)(TParams...))
{
    return *new FreeMethodEventHandler<TParams...>(method);
}


#define METHOD_HANDLER( Object, Method ) createMethodEventHandler( Object, &Method )
#define MY_METHOD_HANDLER( Method ) METHOD_HANDLER( *this, Method )
#define FREE_METHOD_HANDLER(Method) createFreeMethodEventHandler(&Method)