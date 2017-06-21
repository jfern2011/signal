#ifndef __SIGNAL_H__
#define __SIGNAL_H__

#include <cstddef>
#include <tuple>
#include <utility>

namespace Signal
{
    /*
     * Utilities for generating integer sequences used for expanding
     * arguments to signal handlers:
     *
     * http://stackoverflow.com/questions/7858817/
     *         unpacking-a-tuple-to-call-a-matching-function-pointer
     */
    template<int...>
    struct seq
    {
    };

    template<int N, int... S>
    struct gens : gens<N-1, N-1, S...>
    {
    };

    template<int... S>
    struct gens<0, S...>
    {
        typedef seq<S...> type;
    };

    /*
     * A container for bound function arguments:
     */
    template <class... T>
    struct SignalArgs
    {
        std::tuple<typename std::remove_const<
                   typename std::remove_reference<T>::type>::type...>
            args;
    };

    /**
     ******************************************************************
     *
     * @class signal_base
     *
     * Allows for invoking signal handlers through a single (abstract)
     * class, regardless of signal type
     *
     ******************************************************************
     */
    class signal_base
    {
    public:
        virtual void v_raise() = 0;
        virtual ~signal_base() {}
    };

    /**
     ******************************************************************
     *
     * @class signal_t
     *
     * An interface that can be used to access your signal when it is
     * either a fcn_ptr or mem_ptr
     *
     * @tparam R  Signal handler return type
     * @tparam T  Specifies the type(s) of each input argument to the
     *            signal handler
     *
     ******************************************************************
     */
    template <class R, class... T>
    class signal_t : public signal_base
    {
    public:

        virtual R raise(T...) = 0;
        virtual bool detach() = 0;
        virtual bool is_connected() const = 0;
        virtual ~signal_t() {}
    };

    /**
     ******************************************************************
     *
     * @class mem_ptr
     *
     * Represents a signal whose handler is a member of a particular
     * class
     *
     * @tparam R  Signal handler return type
     * @tparam C  Class that owns the signal handler
     * @tparam T1 Specifies the type of the 1st input to pass to the
     *            handler
     * @tparam T2 Specifies the type(s) of any additional inputs the
     *            handler requires
     *
     ******************************************************************
     */
    template <class R, class C, class T1, class... T2>
    class mem_ptr : public signal_t<R,T1,T2...>
    {
        using Handler       = R(C::*)(T1,T2...);
        using const_Handler = R(C::*)(T1,T2...) const;

    public:

        /**
         * Constructor (1)
         *
         * @param [in] obj The object through which to invoke the signal
         *                 handler
         */
        mem_ptr(C& obj)
            : _const_func(NULL), _func(NULL), _is_init(false), _obj(obj)
        {
        }

        /**
         * Constructor (2)
         *
         * @param[in] obj  The object through which to invoke the signal
         *                 handler
         * @param[in] func A pointer to the handler which is a member of
         *                 class C
         */
        mem_ptr(C& obj, const Handler func)
            : _const_func(NULL), _func(func), _obj(obj)
        {
            _is_init = _func != NULL;
        }

        /**
         * Constructor (3)
         *
         * @param[in] obj  The object through which to invoke the signal
         *                 handler
         * @param[in] func A pointer to the handler which is a member of
         *                 class C
         */
        mem_ptr(C& obj, const const_Handler func)
            : _const_func(func), _func(NULL), _obj(obj)
        {
            _is_init = _const_func != NULL;
        }

        /**
         * Destructor
         */
        ~mem_ptr()
        {
        }

        /**
         * Attach a new signal handler
         *
         * @param[in] func A pointer to the handler which is a member of
         *                 class C
         *
         * @return True on success
         */
        bool attach(const Handler func)
        {
            if (func == NULL)
                return false;

            _func = func;
            _const_func = NULL;

            _is_init = true;
            return true;
        }

        /**
         * Attach a new signal handler
         *
         * @param[in] func A pointer to the handler which is a member of
         *                 class C
         *
         * @return True on success
         */
        bool attach(const const_Handler func)
        {
            if (func == NULL)
                return false;

            _func = NULL;
            _const_func = func;

            _is_init = true;
            return true;
        }

        /**
         * Bind arguments to the signal handler
         *
         * @param [in] args A set of arguments to implicitly pass to the
         *                  handler
         */
        template <typename... T>
        void bind(T&&... args)
        {
            _sargs.args = std::make_tuple(std::forward<T>(args)...);
        }

        /**
         * Detach the current signal handler
         *
         * @return True on success
         */
        bool detach()
        {
            _is_init = false;
            _const_func = NULL; _func = NULL;

            return true;
        }

        /**
         * Determine if this signal is currently attached
         *
         * @return True if attached
         */
        bool is_connected() const
        {
            return
                !(_const_func == NULL && _func == NULL);
        }

        /**
         * Forward a set of arguments to the signal handler. This will
         * fail if a handler is not attached, which can be verified by
         * calling is_connected()
         *
         * @param [in] arg1 First input argument to pass to the
         *                  signal handler
         * @param [in] args Additional arguments needed by the handler
         *
         * @return The return value of the attached handler
         */
        R raise(T1 arg1, T2... args)
        {
            if (_func != NULL)
                return (_obj.*_func)(arg1, args...);
            else
                return (_obj.*_const_func)(arg1, args...);
        }

        /**
         * Forwards "bound" arguments to the signal handler. This will
         * fail if a handler is not attached, which can be verified by
         * calling is_connected()
         *
         * @return The return value of the handler
         */
        R raise()
        {
            return
                run(typename gens<1+sizeof...(T2)>::type());
        }

        /**
         * Forwards "bound" arguments to the signal handler. This will
         * fail if a handler is not attached, which can be verified by
         * calling is_connected()
         */
        void v_raise()
        {
            raise();
        }

    protected:

        template<int... S>
        R run(seq<S...>)
        {
            if (_func != NULL)
                return (_obj.*_func)(std::get<S>( _sargs.args)...);
            else
                return
                 (_obj.*_const_func)(std::get<S>( _sargs.args)...);
        }

        const_Handler
                _const_func;
        Handler _func;
        bool    _is_init;
        C&      _obj;

        SignalArgs<T1,T2...> 
                _sargs;
    };

    /**
     ******************************************************************
     *
     * @class mem_ptr
     *
     * Represents a signal whose handler is a member of a particular
     * class
     *
     * This is a specialization where the signal handler requires no
     * arguments
     *
     * @tparam R Signal handler return type
     * @tparam C Class that owns the signal handler
     *
     ******************************************************************
     */
    template <class R, class C>
    class mem_ptr<R,C,void> : public signal_t<R>
    {
        using Handler       = R(C::*)();
        using const_Handler = R(C::*)() const;

    public:

        /**
         * Constructor (1)
         *
         * @param [in] obj The object through which to invoke the signal
         *                 handler
         */
        mem_ptr(C& obj)
            : _const_func(NULL), _func(NULL), _is_init(false), _obj(obj)
        {
        }

        /**
         * Constructor (2)
         *
         * @param[in] obj  The object through which to invoke the signal
         *                 handler
         * @param[in] func A pointer to the handler which is a member of
         *                 class C
         */
        mem_ptr(C& obj, const Handler func)
            : _const_func(NULL), _func(func), _obj(obj)
        {
            _is_init = _func != NULL;
        }

        /**
         * Constructor (3)
         *
         * @param[in] obj  The object through which to invoke the signal
         *                 handler
         * @param[in] func A pointer to the handler which is a member of
         *                 class C
         */
        mem_ptr(C& obj, const const_Handler func)
            : _const_func(func), _func(NULL), _obj(obj)
        {
            _is_init = _const_func != NULL;
        }

        /**
         * Destructor
         */
        ~mem_ptr()
        {
        }

        /**
         * Attach a new signal handler
         *
         * @param[in] func A pointer to the handler which is a member of
         *                 class C
         *
         * @return True on success
         */
        bool attach(const Handler func)
        {
            if (func == NULL)
                return false;

            _func = func;
            _const_func = NULL;

            _is_init = true;
            return true;
        }

        /**
         * Attach a new signal handler
         *
         * @param[in] func A pointer to the handler which is a member of
         *                 class C
         *
         * @return True on success
         */
        bool attach(const const_Handler func)
        {
            if (func == NULL)
                return false;

            _func = NULL;
            _const_func = func;

            _is_init = true;
            return true;
        }

        /**
         * Detach the current signal handler
         *
         * @return True on success
         */
        bool detach()
        {
            _is_init = false;
            _const_func = NULL; _func = NULL;

            return true;
        }

        /**
         * Determine if this signal is currently attached
         *
         * @return True if attached
         */
        bool is_connected() const
        {
            return
                !(_const_func == NULL && _func == NULL);
        }

        /**
         * Invoke the signal handler. This will fail if a handler is
         * not attached. Use is_connected() to determine if a
         * signal can safely be raised. Executing this on a detached
         * signal will raise a seg fault
         *
         * @return The return value of the handler
         */
        R raise()
        {
            if (_func != NULL)
                return (_obj.*_func)();
            else
                return (_obj.*_const_func)();
        }

        /**
         * Invoke the signal handler. This will fail if a handler is
         * not attached. Use is_connected() to determine if a
         * signal can safely be raised. Executing this on a detached
         * signal will raise a seg fault
         */
        void v_raise()
        {
            raise();
        }

    protected:

        const_Handler
                _const_func;
        Handler _func;
        bool    _is_init;
        C&      _obj;
    };

    /**
     ******************************************************************
     *
     * @class fcn_ptr
     *
     * Represents a signal whose handler is a C-style function pointer
     *
     * @tparam R  Signal handler return type
     * @tparam T1 Specifies the type of the first input to pass to the
     *            handler
     * @tparam T2 Specifies the type(s) of additional arguments needed
     *            by the handler
     *
     ******************************************************************
     */
    template <class R, class T1, class... T2>
    class fcn_ptr : public signal_t<R,T1,T2...>
    {
        using Handler = R(*)(T1,T2...);

    public:

        /**
         * Default constructor
         */
        fcn_ptr()
            : _func(NULL), _is_init(false)
        {
        }

        /**
         * Construct a fcn_ptr from a handler
         *
         * @param[in] func The handler, which is a C-style function
         *                 pointer
         */
        fcn_ptr(const Handler func)
            : _func(func)
        {
            _is_init = _func != NULL;
        }

        /**
         * Destructor
         */
        ~fcn_ptr()
        {
        }

        /**
         * Attach a new signal handler
         *
         * @param[in] func The handler, which is a C-style function
         *                 pointer
         *
         * @return True on success
         */
        bool attach(const Handler func)
        {
            if (func == NULL)
                return false;

            _func = func;
            _is_init = true;

            return true;
        }

        /**
         * Bind arguments to the signal handler. This avoids having
         * to call raise() with explicit inputs
         *
         * @param[in] args Input arguments to implicitly forward
         */
        template <typename... T>
        void bind(T&&... args)
        {
            _sargs.args = std::make_tuple(std::forward<T>(args)...);
        }

        /**
         * Detach the current signal handler
         *
         * @return True on success
         */
        bool detach()
        {
            _is_init = false; _func = NULL;

            return true;
        }

        /**
         * Determine if this signal is currently attached
         *
         * @return True if attached
         */
        bool is_connected() const
        {
            return _func != NULL;
        }

        /**
         * Forward a set of arguments to the signal handler. This will
         * fail if a handler is not attached, which can be verified by
         * calling is_connected()
         *
         * @param [in] arg1 First input argument to pass to the
         *                  signal handler
         * @param [in] args Additional arguments needed by the handler
         *
         * @return  The return value of the handler
         */
        R raise(T1 arg1, T2... args)
        {
            return _func(arg1, args...);
        }

        /**
         * Forwards "bound" arguments to the signal handler. This will
         * fail if a handler is not attached, which can be verified by
         * calling is_connected()
         *
         * @return The return value of the handler
         */
        R raise()
        {
            return
                run(typename gens<1+sizeof...(T2)>::type());
        }

        /**
         * Forwards "bound" arguments to the signal handler. This will
         * fail if a handler is not attached, which can be verified by
         * calling is_connected()
         */
        void v_raise()
        {
            raise();
        }

    protected:

        template<int... S>
        R run(seq<S...>)
        {
                return _func(std::get<S>( _sargs.args)... );
        }

        Handler _func;
        bool    _is_init;

        SignalArgs<T1,T2...> 
                _sargs;
    };

    /**
     ******************************************************************
     *
     * @class fcn_ptr
     *
     * Represents a signal whose handler is a C-style function pointer
     *
     * This is a specialization where the signal handler does not take
     * any input arguments
     *
     * @tparam R  Signal handler return type
     *
     ******************************************************************
     */
    template <class R>
    class fcn_ptr<R,void> : public signal_t<R>
    {
        using Handler = R(*)();

    public:

        /**
         * Default constructor
         */
        fcn_ptr()
            : _func(NULL), _is_init(false)
        {
        }

        /**
         * Construct a fcn_ptr from a handler
         *
         * @param[in] func The handler, which is a C-style function
         *                 pointer
         */
        fcn_ptr(const Handler func)
            : _func(func)
        {
            _is_init = _func != NULL;
        }

        /**
         * Destructor
         */
        ~fcn_ptr()
        {
        }

        /**
         * Attach a new signal handler
         *
         * @param[in] func The handler, which is a C-style function
         *                 pointer
         *
         * @return True on success
         */
        bool attach(const Handler func)
        {
            if (func == NULL)
                return false;

            _func = func;
            _is_init = true;

            return true;
        }

        /**
         * Detach the current signal handler
         *
         * @return True on success
         */
        bool detach()
        {
            _is_init = false; _func = NULL;

            return true;
        }

        /**
         * Determine if this signal is currently attached
         *
         * @return True if attached
         */
        bool is_connected() const
        {
            return _func != NULL;
        }

        /**
         * Invoke the signal handler. This will fail if a handler is
         * not attached. Use is_connected() to determine if a
         * signal can safely be raised. Executing this on a detached
         * signal will raise a seg fault
         *
         * @return The return value of the handler
         */
        R raise()
        {
            return _func();
        }

        /**
         * Invoke the signal handler. This will fail if a handler is
         * not attached. Use is_connected() to determine if a
         * signal can safely be raised. Executing this on a detached
         * signal will raise a seg fault
         */
        void v_raise()
        {
            raise();
        }

    protected:

        Handler _func;
        bool    _is_init;
    };

    /**
     ******************************************************************
     *
     * @class Signal
     *
     * Enables the creation of customized "signals" that trigger events
     * of interest which are application-specific. Each Signal wraps
     * a generic function pointer or some class method that is used as
     * a handler when that signal is raised. This concept is analogous
     * to using the standard signal library
     *
     * @tparam R  The signal handler's return type
     * @tparam T1 Specify the type of the first argument to pass to the
     *            signal handler
     * @tparam T2 Specify the type(s) of additional arguments needed by
     *            the handler
     *
     ******************************************************************
     */
    template <class R, class T1, class... T2>
    class Signal : public signal_base
    {
    public:

        /**
         * Default constructor
         */
        Signal()
            : _is_mem_ptr(false), _sig(NULL)
        {
        }

        /**
         * Create a signal with a handler
         *
         * @param[in] func A pointer to the signal handler
         */
        Signal(R(*func)(T1,T2...))
            : _is_mem_ptr(false)
        {
            _sig = new fcn_ptr<R,T1,T2...>(func);
        }

        /**
         * Create a signal whose handler is a member function of class C
         *
         * @tparam C Class that owns the handler 
         *
         * @param[in] obj  Object (of class C) through which to invoke
         *                 the handler
         * @param[in] func A pointer to the signal handler which belongs
         *                 to class C
         */
        template <typename C>
        Signal(C& obj, R(C::*func)(T1,T2...))
            : _is_mem_ptr(true)
        {
            _sig = new mem_ptr<R,C,T1,T2...>(obj, func);
        }

        /**
         * Create a signal whose handler is a member function of class C
         *
         * @tparam C Class that owns the handler 
         *
         * @param[in] obj  Object (of class C) through which to invoke
         *                 the handler
         * @param[in] func A pointer to the signal handler which belongs
         *                 to class C
         */
        template <typename C>
        Signal(C& obj, R(C::*func)(T1,T2...) const)
            : _is_mem_ptr(true)
        {
            _sig = new mem_ptr<R,C,T1,T2...>(obj, func);
        }

        /**
         * Destructor
         */
        ~Signal()
        {
            if (_sig) delete _sig;
        }

        /**
         * Attach a handler to this Signal, removing the previous
         * handler (if it exists)
         *
         * @param[in] func A pointer to the signal handler
         *
         * @return  True if the handler was successfully attached
         */
        bool attach(R(*func)(T1,T2...))
        {
            if (is_connected() && !detach())
                return false;

            _sig = new fcn_ptr<R,T1,T2...>(func);
            _is_mem_ptr = false;

            return _sig->is_connected();
        }

        /**
         * Attach a handler to this Signal, removing the previous
         * handler (if it exists)
         *
         * @tparam C Class that owns the handler 
         *
         * @param[in] obj  Object (of class C) through which to
         *                 invoke the handler
         * @param[in] func A pointer to the signal handler
         *
         * @return  True if the handler was successfully attached
         */
        template <typename C>
        bool attach(C& obj, R(C::*func)(T1,T2...))
        {
            if (is_connected() && !detach())
                return false;
            else
            {
                _sig = new mem_ptr<R,C,T1,T2...>(obj, func);
                _is_mem_ptr = true;
            }

            return _sig->is_connected();
        }

        /**
         * Associate a new handler with this Signal without changing 
         * the object through which it is called. This method will
         * fail if this signal was not initialized with an object of
         * class C
         *
         * @tparam C Class that owns the signal handler 
         *
         * @param[in] func A pointer to the handler
         *
         * @return True if the new handler was successfully attached
         */
        template <typename C>
        bool attach(R(C::*func)(T1,T2...))
        {
            if (!_is_mem_ptr || !is_connected())
                return false;
            else
                dynamic_cast<mem_ptr<R,C,T1,T2...>*>(_sig)->attach(func);

            return _sig->is_connected();
        }

        /**
         * Attach a handler to this Signal, removing the previous
         * handler (if it exists)
         *
         * @tparam C Class that owns the handler 
         *
         * @param[in] obj  Object (of class C) through which to
         *                 invoke the handler
         * @param[in] func A pointer to the signal handler
         *
         * @return True if the handler was successfully attached
         */
        template <typename C>
        bool attach(C& obj, R(C::*func)(T1,T2...) const)
        {
            if (is_connected() && !detach())
                return false;
            else
            {
                _sig = new mem_ptr<R,C,T1,T2...>(obj, func);
                _is_mem_ptr = true;
            }

            return _sig->is_connected();
        }

        /**
         * Associate a new handler with this Signal without changing 
         * the object through which it is called. This method will
         * fail if this signal was not initialized with an object of
         * class C
         *
         * @tparam C Class that owns the signal handler 
         *
         * @param[in] func A pointer to the handler
         *
         * @return True if the new handler was successfully attached
         */
        template <typename C>
        bool attach(R(C::*func)(T1,T2...) const)
        {
            if (!_is_mem_ptr || !is_connected())
                return false;
            else
                dynamic_cast<mem_ptr<R,C,T1,T2...>*>(_sig)->attach(func);

            return _sig->is_connected();
        }

        /**
         * Bind arguments to the signal handler
         *
         * @param[in] args The set of arguments to implicitly pass to
         *                 the handler
         */
        template <typename... T>
        void bind(T&&... args)
        {
            _sargs.args =
                std::make_tuple(std::forward<T>(args)...);
        }

        /**
         * Detach a signal handler
         *
         * @return True on success, or false if no handler is attached
         */
        bool detach()
        {
            if (!_sig) return false;

            delete _sig; _sig = NULL;
            _is_mem_ptr = false;

            return true;
        }

        /**
         * Check whether a handler is attached to this Signal
         *
         * @return True if a handler is currently attached
         */
        bool is_connected() const
        {
            if (!_sig)
                return false;

            return _sig->is_connected();
        }

        /**
         * Forward a set of arguments to the signal handler. This will
         * fail if a handler is not attached. Use is_connected() to
         * determine if a signal can safely be raised. Calling this
         * on a detached signal will raise a seg fault (which probably
         * isn't the signal you intended)
         *
         * @param [in] arg1 First input argument to pass to the
         *                  signal handler
         * @param [in] args Additional arguments needed by the handler
         * 
         * @return The handler's return value
         */
        R raise(T1 arg1, T2... args)
        {
            return _sig->raise(arg1, args...);
        }

        /**
         * Forwards "bound" arguments to the signal handler. This will
         * fail if a handler is not attached, which can be verified by
         * calling is_connected()
         *
         * @return The return value of the handler
         */
        R raise()
        {
            return
                run(typename gens<1+sizeof...(T2)>::type());
        }

        /**
         * Forwards "bound" arguments to the signal handler. This will
         * fail if a handler is not attached, which can be verified by
         * calling is_connected()
         */
        void v_raise()
        {
            raise();
        }

        /*
         * Forbid copy construction/assignment:
         */
        Signal(const Signal<R,T1,T2...>& rhs)        = delete;
        Signal<R,T1,T2...>&
            operator=(const Signal<R,T1,T2...>& rhs) = delete;

    private:

        template<int... S>
        R run(seq<S...>)
        {
            return _sig->raise(std::get<S>( _sargs.args)... );
        }

        bool    _is_mem_ptr;

        SignalArgs<T1,T2...> 
                _sargs;

        signal_t<R,T1,T2...>*
                _sig;
    };

    /**
     ******************************************************************
     *
     * @class Signal
     *
     * Enables the creation of customized "signals" that trigger events
     * of interest which are application-specific. Each Signal wraps
     * a generic function pointer or some class method that is used as
     * a handler when that signal is raised. This concept is analogous
     * to using the standard signal library.
     *
     * This is a specialization where the signal handler takes no input
     * arguments
     *
     * @tparam R  The signal handler's return type
     *
     ******************************************************************
     */
    template <class R>
    class Signal<R,void> : public signal_base
    {
    public:

        /**
         * Default constructor
         */
        Signal()
            : _is_mem_ptr(false), _sig(NULL)
        {
        }

        /**
         * Create a signal with a handler
         *
         * @param[in] func C-style function pointer to the signal handler
         */
        Signal(R(*func)())
            : _is_mem_ptr(false)
        {
            _sig = new fcn_ptr<R,void>(func);
        }

        /**
         * Create a signal whose handler is a member function of class C
         *
         * @tparam C Class that owns the handler 
         *
         * @param[in] obj  Object (of class C) through which to invoke
         *                 the handler
         * @param[in] func A pointer to the signal handler which belongs
         *                 to class C
         */
        template <typename C>
        Signal(C& obj, R(C::*func)())
            : _is_mem_ptr(true)
        {
            _sig = new mem_ptr<R,C,void>(obj, func);
        }

        /**
         * Create a signal whose handler is a member function of class C
         *
         * @tparam C Class that owns the handler 
         *
         * @param[in] obj  Object (of class C) through which to invoke
         *                 the handler
         * @param[in] func A pointer to the signal handler which belongs
         *                 to class C
         */
        template <typename C>
        Signal(C& obj, R(C::*func)() const)
            : _is_mem_ptr(true)
        {
            _sig = new mem_ptr<R,C,void>(obj, func);
        }

        /**
         * Destructor
         */
        ~Signal()
        {
            if (_sig) delete _sig;
        }

        /**
         * Attach a handler to this Signal, removing the previous
         * handler (if it exists)
         *
         * @param[in] func A pointer to the signal handler
         *
         * @return  True if the handler was successfully attached
         */
        bool attach(R(*func)())
        {
            if (is_connected() && !detach())
                return false;

            _sig = new fcn_ptr<R,void>(func);
            _is_mem_ptr = false;

            return _sig->is_connected();
        }

        /**
         * Attach a handler to this Signal, removing the previous
         * handler (if it exists)
         *
         * @tparam C Class that owns the handler 
         *
         * @param[in] obj  Object (of class C) through which to
         *                 invoke the handler
         * @param[in] func A pointer to the signal handler
         *
         * @return  True if the handler was successfully attached
         */
        template <typename C>
        bool attach(C& obj, R(C::*func)())
        {
            if (is_connected() && !detach())
                return false;
            else
            {
                _sig = new mem_ptr<R,C,void>(obj, func);
                _is_mem_ptr = true;
            }

            return _sig->is_connected();
        }

        /**
         * Associate a new handler with this Signal without changing 
         * the object through which it is called. This method will
         * fail if this signal was not initialized with an object of
         * class C
         *
         * @tparam C Class that owns the signal handler 
         *
         * @param[in] func A pointer to the handler
         *
         * @return True if the new handler was successfully attached
         */
        template <typename C>
        bool attach(R(C::*func)())
        {
            if (!_is_mem_ptr || !is_connected())
                return false;
            else
                dynamic_cast<mem_ptr<R,C,void>*>(_sig)->attach(func);

            return _sig->is_connected();
        }

        /**
         * Attach a handler to this Signal, removing the previous
         * handler (if it exists)
         *
         * @tparam C Class that owns the handler 
         *
         * @param[in] obj  Object (of class C) through which to
         *                 invoke the handler
         * @param[in] func A pointer to the signal handler
         *
         * @return  True if the handler was successfully attached
         */
        template <typename C>
        bool attach(C& obj, R(C::*func)() const)
        {
            if (is_connected() && !detach())
                return false;
            else
            {
                _sig = new mem_ptr<R,C,void>(obj, func);
                _is_mem_ptr = true;
            }

            return _sig->is_connected();
        }

        /**
         * Associate a new handler with this Signal without changing 
         * the object through which it is called. This method will
         * fail if this signal was not initialized with an object of
         * class C
         *
         * @tparam C Class that owns the signal handler 
         *
         * @param[in] func A pointer to the handler
         *
         * @return True if the new handler was successfully attached
         */
        template <typename C>
        bool attach(R(C::*func)() const)
        {
            if (!_is_mem_ptr || !is_connected())
                return false;
            else
                dynamic_cast<mem_ptr<R,C,void>*>(_sig)->attach(func);

            return _sig->is_connected();
        }

        /**
         * Detach a signal handler
         *
         * @return True on success, or false if no handler is attached
         */
        bool detach()
        {
            if (!_sig) return false;

            delete _sig; _sig = NULL;
            _is_mem_ptr = false;

            return true;
        }

        /**
         * Check whether a handler is attached to this Signal
         *
         * @return True if a handler is currently attached
         */
        bool is_connected() const
        {
            if (!_sig)
                return false;

            return _sig->is_connected();
        }

        /**
         * Invoke the signal handler. This will fail if a handler is
         * not attached. Use is_connected() to determine if a
         * signal can safely be raised. Executing this on a detached
         * signal will raise a seg fault
         * 
         * @return The handler's return value
         */
        R raise()
        {
            return _sig->raise();
        }

        /**
         * Invoke the signal handler. This will fail if a handler is
         * not attached. Use is_connected() to determine if a
         * signal can safely be raised. Executing this on a detached
         * signal will raise a seg fault
         */
        void v_raise()
        {
            raise();
        }

        /*
         * Forbid copy construction/assignment:
         */
        Signal(const Signal<R,void>& rhs)        = delete;
        Signal<R,void>&
            operator=(const Signal<R,void>& rhs) = delete;

    private:

        bool         _is_mem_ptr;
        signal_t<R>* _sig;
    };

    /**
     ******************************************************************
     *
     * @class Callable
     *
     * Works similarly to the Signal class, except that the handler is
     * a generic callable object
     *
     * @tparam Func The signal handler
     *
     ******************************************************************
     */
    template <class Func>
    class Callable
    {
    public:

        /**
         * Default constructor
         */
        Callable()
            : _is_init(false)
        {
        }

        /**
         * Constructor
         *
         * @param [in] handler A callable object to be used as a signal
         *                     handler
         */
        Callable(Func handler)
            : _handler(handler), _is_init(true)
        {
        }

        /**
         * Destructor
         */
        ~Callable()
        {
        }

        /**
         * Attach a new signal handler
         *
         * @param [in] handler A callable object to be used as a signal
         *                     handler
         */
        bool attach(Func handler)
        {
            _handler = handler; _is_init = true;
            return true;
        }

        /**
         * Detach the current signal handler
         *
         * @return True on success
         */
        bool detach()
        {
            _is_init = false;
            return true;
        }

        /**
         * Check whether a handler is attached to this signal
         *
         * @return True if a handler is currently attached
         */
        bool is_connected() const
        {
            return _is_init;
        }

        /**
         * Invoke the signal handler
         *
         * @param[in] args Arguments to forward to the signal
         *                 handler
         *
         * @return The handler's return value
         */
        template <typename... T>
        auto raise(T&&... args)
            -> decltype(std::declval<Func&>()(std::forward<T>(args)...))
        {
            return _handler(std::forward<T>(args)...);
        }

    private:

        Func _handler;
        bool _is_init;
    };
}

#endif // __SIGNAL_H__
