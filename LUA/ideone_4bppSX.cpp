#define TUPLE_FORWARDx
#ifdef TUPLE_FORWARD
#include <iostream>
#include <utility>
#include <tuple>


template <class MT>
struct count_method_args;

template <class T, class Res, class... Args>
struct count_method_args<Res (T::*)(Args...)> 
{
   static constexpr std::size_t value = sizeof...(Args);
};

template <class MT>
struct method_args_as_tuple;

template <class T, class Res, class... Args>
struct method_args_as_tuple<Res (T::*)(Args...)> 
{
	using type = std::tuple<Args&&...>;
	//typedef type std::tuple<Args&&...> ArgsTuple;
};

template <class MT, MT, class I = std::make_index_sequence<count_method_args<MT>::value>>
struct func_impl;

template <class T, class Res, class... Args, Res (T::*M)(Args...), std::size_t... Is>
struct func_impl<Res (T::*)(Args...), M, std::index_sequence<Is...>> 
{
   void operator()(std::tuple<Args&&...> &&tup) 
   {
       T t;
       (t.*M)(std::forward<Args>((std::get<Is>(tup)))...);
   }
};


struct S 
{

   template <class MethodType, MethodType MethodName>
   static void func(typename method_args_as_tuple<MethodType>::type &&tup) 
   {
       func_impl<MethodType, MethodName>{}(std::forward<typename method_args_as_tuple<MethodType>::type>(tup));
   }
  
};

struct foo {
   void print(int &a, double &b) {	   
      std::cout << a << " " << b << std::endl;
	  a++;
   }

   void print2(int a, double b) {
	   std::cout << a << " " << b << std::endl;
	   a++;
   }

   void print3(const std::string & s) {
	   std::cout << s.c_str() << std::endl;	   
   }

   void print4(std::string s) {
	   std::cout << s.c_str() << std::endl;
   }
};

int Get10() {
	int var = 5 * 2;
	return var;
}




std::tuple<int&> TupleTest()
{
	int * acc = new int;
	*acc = 55;
	int &k = *acc;
	std::tuple<int&> ttt = std::forward_as_tuple(k);
	std::tuple<> args = std::make_tuple<>();

	return std::tuple_cat(std::move(ttt), std::move(args));		
}

int main() {
	int a = 5;
	std::string xx = "Ahoj";
	std::string xx2 = "Ahoj2";

	//primitivni typy (promenna, hodnota) = make_tuple
	//reference = forward_as_tuple

	
	auto tpl = TupleTest();
	//acc = 10;
	auto ii = std::get<0>(tpl);


	std::tuple<int&, double&&> tuple = std::forward_as_tuple(a, 1.5);

	std::tuple<int&> t1 = std::forward_as_tuple(a); //=> std::make_tuple(std::ref(a))
	std::tuple<double&&> t2 = std::forward_as_tuple(1.5);

	auto tupleCat = std::tuple_cat(std::move(t1), std::move(t2));
		

	//auto t12 = std::make_tuple(std::ref(a));
	
	//auto rr = std::forward_as_tuple(4, 1.5);

   S::func<decltype(&foo::print), &foo::print>(tupleCat);

   auto t12 = std::forward_as_tuple(Get10());
   auto t22 = std::forward_as_tuple(1.5);
   auto tuple2 = std::tuple_cat(std::move(t12), std::move(t22));

   S::func<decltype(&foo::print2), &foo::print2>(std::move(tuple2));
   S::func<decltype(&foo::print3), &foo::print3>(std::forward_as_tuple(xx));
   S::func<decltype(&foo::print4), &foo::print4>(std::make_tuple(xx2));
   std::cout << "Original a after change: " << a << std::endl;

   /*
   int a = 5;
	std::string xx = "Ahoj";
	std::string xx2 = "Ahoj2";

	std::tuple<int&, double&&> tuple = std::forward_as_tuple(a, 1.5);

	auto t1 = std::forward_as_tuple(a);
	auto t2 = std::forward_as_tuple(1.5);
	auto tupleCat = std::tuple_cat(std::move(t1), std::move(t2));


	auto t12 = std::forward_as_tuple(a);
	auto t22 = std::forward_as_tuple(1.5);
	auto tuple2 = std::tuple_cat(std::move(t12), std::move(t22));
	

   S::func<decltype(&foo::print), &foo::print>(tupleCat);
   S::func<decltype(&foo::print2), &foo::print2>(std::make_tuple(a, 1.5));
   S::func<decltype(&foo::print3), &foo::print3>(std::forward_as_tuple(xx));
   S::func<decltype(&foo::print4), &foo::print4>(std::make_tuple(xx2));
   std::cout << "Original a after change: " << a << std::endl;
   
   */

}

#endif