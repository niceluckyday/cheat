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
    virtual void call(TParams... params) = 0;
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

    virtual void operator()(TParams... params)
    {
        for (TEventHandler* oneHandler : m_handlers)
            oneHandler->call(params...);
    }

    void operator+=(TEventHandler& eventHandler)
    {
        m_handlers.push_back(&eventHandler);
    }
protected:
    std::list<TEventHandler*> m_handlers;
};

template<class ...TParams>
class TCancelableEvent : public TEvent<TParams..., bool&>
{
    using TEventHandler = AbstractEventHandler<TParams..., bool&>;
    using TBase = TEvent<TParams..., bool&>;
public:
    bool operator()(TParams... params)
    {
        bool canceled = false;
        for (TEventHandler* oneHandler : TBase::m_handlers)
            oneHandler->call(params..., canceled);
        return !canceled;
    }
};

template<class TObject, class ...TParams>
class MethodEventHandler : public AbstractEventHandler<TParams...>
{
    using TMethod = void(TObject::*)(TParams...);
public:
    MethodEventHandler(TObject& object, TMethod method) :
        AbstractEventHandler<TParams...>(),
        m_object(object),
        m_method(method)
    {
        assert(m_method != nullptr);
    }
    virtual void call(TParams... params) override final
    {
        (m_object.*m_method)(params...);
    }
private:
    TObject& m_object;
    TMethod m_method;
};

template<class ...TParams>
class FreeMethodEventHandler : public AbstractEventHandler<TParams...> 
{
    using TMethod = void(*)(TParams...);
public:
    FreeMethodEventHandler(TMethod method) :
        AbstractEventHandler<TParams...>(),
        m_method(method)
    {
        assert(m_method != nullptr);
    }
    virtual void call(TParams... params) override final
    {
        (*m_method)(params...);
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
AbstractEventHandler<TParams...>& createFreeMethodEventHandler(void(*method)(TParams...))
{
    return *new FreeMethodEventHandler<TParams...>(method);
}


#define METHOD_HANDLER( Object, Method ) createMethodEventHandler( Object, &Method )
#define MY_METHOD_HANDLER( Method ) METHOD_HANDLER( *this, Method )
#define FREE_METHOD_HANDLER(Method) createFreeMethodEventHandler(&Method)