#ifdef SAMPLES_RETURN_TYPE

#include <cstdio>
#include <cstdlib>
#include "./Strings/MyString.h"


// in the normal case, just the identity
//http://stackoverflow.com/questions/15911890/overriding-return-type-in-function-template-specialization/15912228#15912228


//----------------------------------------------------------------------

struct Bar
{
template<class T>
struct item_return { typedef T type; };


template<class T>
typename item_return<T>::type foo();
};



template<>
struct Bar::item_return<const MyStringAnsi &> { typedef MyStringAnsi type; };
//------------------------------------------------------------------------



template<class T>
typename Bar::item_return<T>::type Bar::foo() { return T(); }

template<>
typename Bar::item_return<int>::type Bar::foo<int>() { return 5; }

template<>
typename Bar::item_return<MyStringAnsi>::type Bar::foo<MyStringAnsi>() { return "42xxx"; }

template<>
MyStringAnsi Bar::foo<const MyStringAnsi &>() { return "xxx42xxx"; }

// might want to stick to the following
// so you only need to update the return-type in one place
// namely, the `item_return` specialization
//template<>
//item_return<float>::type foo<float>(){ ... }
// note: No `typename` needed, because `float` is not a dependent type

int main() 
{
Bar b;

printf("%s %i", b.foo<const MyStringAnsi &>().c_str(), b.foo<int>());

}

#endif


#ifdef SAMPLE_TYPE_TRAITS

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include "./Strings/MyString.h"

//http://www.johannmuszynski.com/blog/post/modern_c_templates_are_your_friend/

template< typename F >
struct function_traits;

template< typename R, typename... Args >
struct function_traits< R(Args...) > 
{
	using return_type = R;

	constexpr static const std::size_t arity = sizeof...(Args);

	template< std::size_t N >
	struct argument 
	{
		static_assert(N < arity, "FunctionTraits error: invalid argument count parameter");
		using type = std::tuple_element_t< N, std::tuple< Args... > >;
	};

	template< std::size_t N >
	using argument_type = typename argument<N>::type;
};

double convertToDegrees(double rad) {
	return rad * (180.0 / 3.14159265359);
}

class TestClass
{
	public:
		double convertToDegrees(double rad) {
			return rad * (180.0 / 3.14159265359);
		}
};

int main(int argc, char *argv[])
{
	//using traits = function_traits< decltype(&TestClass::convertToDegrees) >; //not working
	using traits = function_traits< decltype(convertToDegrees) >;
	std::cout << std::is_same<double, traits::argument_type<0>>::value << std::endl;
	std::cout << std::is_same<double, traits::return_type>::value << std::endl;
}

#endif

#define DEDUCT_ARGSx
#ifdef DEDUCT_ARGS


#include <iostream>
#include <utility>
#include <tuple>

//=======================================================================================


template <class MT>
struct count_method_args;

template <class ClassType, class Res, class... Args>
struct count_method_args<Res(ClassType::*)(Args...)>
{
	static constexpr std::size_t value = sizeof...(Args);
};


//=======================================================================================

template <class MT, MT, class I = std::make_index_sequence<count_method_args<MT>::value>>
struct func_impl;

template <class ClassType, class Res, class... Args, Res(ClassType::*MethodName)(Args...), std::size_t... Is>
struct func_impl<Res(ClassType::*)(Args...), MethodName, std::index_sequence<Is...>>
{
	void operator()(ClassType * obj, std::tuple<Args&&...> &&tup) 
	{		
		typedef std::tuple_element<0, std::tuple<Args...>>::type ArgType;

		//(obj->*MethodName)(std::forward<Args>((std::get<Is>(tup)))...);
		(obj->*MethodName)(std::forward<Args>(
			std::get<Is>(tup)
		)...);
	}
};


//=======================================================================================


template <class MT>
struct method_info;


template <class T, class Res, class... Args>
struct method_info<Res(T::*)(Args...)>
{
	typedef std::tuple<Args...> ArgsTuple;
	typedef T ClassType;
	typedef Res RetVal;	
};


//=======================================================================================


struct S 
{

	template <class MethodType, MethodType MethodName,
		typename MethodInfo = method_info<MethodType>,
		typename RetVal = method_info<MethodType>::RetVal,		
		typename std::enable_if <std::is_same<void, RetVal>::value>::type* = nullptr		
	> 		
	static void func() 
	{
		//std::string ss = "xx1";
		//typedef method_info<MethodType> MethodInfo;
		typename MethodInfo::ArgsTuple tup = std::make_tuple(1, 1.5, 2);

		
		using rrr = typename std::tuple_element<0, MethodInfo::ArgsTuple>::type;


		std::cout << "XXX" << std::is_same<void, MethodInfo::RetVal>::value << std::endl;
		std::cout << "-->" << std::is_same<int, rrr>::value << std::endl;
		std::cout << "RetVal: " << 1 << std::endl;

		MethodInfo::ClassType f;

		std::cout << "RetVal" << std::is_same<int, MethodInfo::RetVal>::value << std::endl;

		func_impl<MethodType, MethodName>{}(
			&f,
			std::forward<MethodInfo::ArgsTuple>(tup)
		);
	}


	template <class MethodType, MethodType MethodName,
		typename MethodInfo = method_info<MethodType>,
		typename RetVal = method_info<MethodType>::RetVal,
		typename std::enable_if <!std::is_same<void, RetVal>::value>::type* = nullptr
	>
		static void func()
	{
		//std::string ss = "xx1";
		//typedef method_info<MethodType> MethodInfo;
		typename MethodInfo::ArgsTuple tup = std::make_tuple(1, 1.5, 2);


		std::cout << "YYYY" << std::is_same<void, MethodInfo::RetVal>::value << std::endl;


		MethodInfo::ClassType f;

		std::cout << "RetVal" << std::is_same<int, MethodInfo::RetVal>::value << std::endl;

		func_impl<MethodType, MethodName>{}(
			&f,
			std::forward<MethodInfo::ArgsTuple>(tup)
			);
	}

	

};

//=======================================================================================

struct foo 
{
	void print()
	{
	}

	int print(int a, double b) 
	{
		std::cout << a << " " << b << std::endl;
		return 1;
	}

	
	void print(int a, double b, int c) { std::cout << a << " " << b << " " << c << std::endl; }

	int print3(int a, double b, int c) { std::cout << "PRINT3: " << a << " " << b << " " << c << std::endl; return 0; }

	void print2(int a, double b, int c) { std::cout << "PRINT2: " << a << " * " << b << "  * " << c << std::endl; }
	
};

//=======================================================================================

template<typename T, typename ... Args>
using get_print_type = decltype(std::declval<T>().print(std::declval<Args>() ...)) (T::*)(Args ...);


int main() 
{	
	//S::func<decltype(&foo::print), &foo::print>();
	//S::func<get_print_type<foo, int, double, int>, &foo::print >();
	//S::func<(static_cast<void(foo::*)(int, double, int)>(&foo::print)), &foo:print>();
	//S::func<void(foo::*)(int, double, int), &foo:print>();

	S::func<decltype(&foo::print2), &foo::print2>();
	S::func<decltype(&foo::print3), &foo::print3>();
	//S::func<get_print_type<foo, int, double, int>, &foo::print >();
	//S::func<get_print_type<foo>, &foo::print >();
}


//=======================================================================================

/*

#include <iostream>
#include <utility>
#include <tuple>
#include <type_traits>
#include <utility>
#include <functional>

template <class MT>
struct count_method_args;

template <class T, class Res, class... Args>
struct count_method_args<Res(T::*)(Args...)> {
	static constexpr std::size_t value = sizeof...(Args);
};

template <class MT>
struct method_args_as_tuple;

template <class T, class Res, class... Args>
struct method_args_as_tuple<Res(T::*)(Args...)> {
	using type = std::tuple<Args&&...>;
};

template <class MT, MT, class I = std::make_index_sequence<count_method_args<MT>::value>>
struct func_impl;

template <class T, class Res, class... Args, Res(T::*M)(Args...), std::size_t... Is>
struct func_impl<Res(T::*)(Args...), M, std::index_sequence<Is...>> {
	void operator()(std::tuple<Args&&...> &&tup) {
		T t;
		(t.*M)(std::forward<Args>((std::get<Is>(tup)))...);
	}
};


struct S {

	template <class MethodType, MethodType MethodName>
	static void func(typename method_args_as_tuple<MethodType>::type &&tup) {
		func_impl<MethodType, MethodName>{}(std::forward<typename method_args_as_tuple<MethodType>::type>(tup));
	}

};

struct foo {
	void print(int a, double b) {
		std::cout << a << " " << b << std::endl;
	}

	int print()
	{
		return 5;
	}
	
};

int test(double x, double y);
double test(int x, int y);
char test(char x, int y);
void test();

template<typename... Ts>
using TestType = decltype(test(std::declval<Ts>()...))(Ts...);

template<typename... Ts>
using TestType2 = decltype(&foo::print(std::declval<Ts>()...))(Ts...);


struct ComplicatedType1 {};
struct ComplicatedType2 {};

struct A {
	ComplicatedType1 f();
	ComplicatedType2 f(int);
};

//template<typename T, typename... Ts>
//using TestType3 = decltype(std::declval<foo>().print(int, double));

template <class MethodType, typename... Args>
struct V
{
	using type = decltype(std::declval<MethodType>().f(Args...));
};

struct Sx {
	double x(char, int) { return 0; };
	float x(int) { return 1.0; }
	float y(int) { return 1.0; }
};

template<class T>
typename std::result_of<T(int)>::type f(T& t)
{
	std::cout << "overload of f for callable T\n";
	return t(0);
}

template<class T, class U>
int f(U u)
{
	std::cout << "overload of f for non-callable T\n";
	return u;
}

typedef  double (Sx::*(SxMemPtr1))(char, int);  // Please do this!
typedef  float (Sx::*(SxMemPtr2))(int);  // Please do this!


struct foo2 {
	int    memfun1(int a) const { return a; }
	double memfun2(double b) const { return b; }
};


int main() {


	using Type = decltype(std::declval<Sx>().x());

	SxMemPtr1 p1 = &Sx::x;
	SxMemPtr2 p2 = &Sx::x;
	
	SxMemPtr1 fun = &Sx::x; //no cast required now!
	//std::function<double (char, int)> func = fun;// static_cast<SxMemPtr1>(&Sx::x);//cast!
	

	

	//typedef std::result_of<decltype(std::declval<foo2>().memfun1(1))>::type i;
	//std::cout << i << std::endl;
	//decltype(std::declval<foo2>().memfun2(10.0)) d = 12.9;
	//std::cout << d << std::endl;


	//typedef std::result_of<SxMemPtr1> D; // d has type double
	//static_assert(std::is_same<std::result_of<SxMemPtr1>::type, double>::value, "neshoda");

	//S::func<TestType, &foo::print>(std::forward_as_tuple(1, 1.5));
	//std::result_of < foo::print(int, double) >::type ;

	//decltype(&A::f)(A, int)
	
	//typedef std::result_of<decltype(&Sx::x)(int)>::type D; // d has type double
	//static_assert(std::is_same<D, double>::value, "neshoda");
	
	
	//std::cout << std::is_same<D, double>::value << std::endl;
	

	//static_assert(std::is_same<TestType3, void>::value, "Oops");


	//static_assert(std::is_same<TestType2<int, double>, void (foo::*)(int, double)>::value, "1");
	//static_assert(std::is_same<TestType2<>, int()>::value, "2");

	static_assert(std::is_same<TestType<double, double>, int(double, double)>::value, "1");
	static_assert(std::is_same<TestType<int, int>, double(int, int)>::value, "2");
	static_assert(std::is_same<TestType<char, int>, char(char, int)>::value, "3");
	static_assert(std::is_same<TestType<>, void()>::value, "4");
	

}
*/
#endif

#ifdef SOME_ELSE

template <typename... R>
static inline std::tuple<typename std::remove_reference<R>::type...>
remove_ref_from_tuple_members(std::tuple<R...> const& t)
{
	//http://stackoverflow.com/questions/12742877/remove-reference-from-stdtuple-members
	return std::tuple<typename std::remove_reference<R>::type...>{ t };
}

#endif