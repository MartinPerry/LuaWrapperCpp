#include <iostream>
#include <utility>
#include <tuple>


template <class MT>
struct count_method_args;

template <class T, class Res, class... Args>
struct count_method_args<Res (T::*)(Args...)> {
   static constexpr std::size_t value = sizeof...(Args);
};

template <class MT>
struct method_args_as_tuple;

template <class T, class Res, class... Args>
struct method_args_as_tuple<Res (T::*)(Args...)> {
   using type = std::tuple<Args&&...>;
};

template <class MT, MT, class I = std::make_index_sequence<count_method_args<MT>::value>>
struct func_impl;

template <class T, class Res, class... Args, Res (T::*M)(Args...), std::size_t... Is>
struct func_impl<Res (T::*)(Args...), M, std::index_sequence<Is...>> {
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
   void print(int &&a, double &&b) {
      std::cout << a << " " << b << std::endl;
   }
};

int main() {
   S::func<decltype(&foo::print), &foo::print>(std::forward_as_tuple(1, 1.5));
}