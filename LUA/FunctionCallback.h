#ifndef FUNCTION_CALLBACK_H
#define FUNCTION_CALLBACK_H



#include <functional>


template<int... Is> struct seq {};

template<int N, int... Is>
struct gen_seq2 : gen_seq2 < N - 1, N - 1, Is... > {};

template<int ...S>
struct gen_seq2 < 0, S... > { typedef seq<S...> type; };

class IFunction
{
public:

	virtual void call() = 0;	
};


template <typename R, typename ...ARGS>
class MyFunction : public IFunction
{
public:
	R retVal;
	std::tuple<ARGS...> args;

	std::function<R(ARGS...)> f;
	
	MyFunction(){}

	MyFunction(std::function<R(ARGS...)> f) : f(f){}

	virtual void call()
	{
		this->retVal = this->callFunc(typename gen_seq2<sizeof...(ARGS)>::type());
	};

	template<int ...S>
	R callFunc(seq<S...>)
	{
		return f(std::get<S>(args) ...);
	}
};






#endif
