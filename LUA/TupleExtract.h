#ifndef TUPLE_EXTRACT_H
#define TUPLE_EXTRACT_H


//http://stackoverflow.com/questions/19942877/c-variadic-templates-and-evaluation-order
//http://pastebin.com/17UAQ0jk

//===============================================================
//===============================================================
//===============================================================


template <unsigned int n>
struct tuple_extractor 
{
	template <
		typename ClassType, 
		typename MethodType,		
		typename ...ArgsIn, 
		typename ...ArgsPart
	>
	static void extractTuple(
		ClassType* obj,
		MethodType func,
		const std::tuple<ArgsIn...>& inParams,
		ArgsPart... partParams) 
	{
		tuple_extractor<n - 1>::extractTuple(obj, 
			func, 
			inParams, 
			std::get<n - 1>(inParams), 
			partParams...);
	}
};

//===============================================================

template <>
struct tuple_extractor<0> 
{
	template <
		typename ClassType, 
		typename MethodType,		
		typename ...ArgsIn, 
		typename ...ArgsPart
	>
	static void extractTuple(
		ClassType* obj,
		MethodType func,
		const std::tuple<ArgsIn...>& inParams,
		ArgsPart... partParams) 
	{
		(obj->*func)(partParams...);	
	}
};



//===============================================================
//===============================================================
//===============================================================
//===============================================================
//===============================================================
//===============================================================


template <unsigned int n>
struct tuple_extractor2
{
	template <
		typename ClassType,
		typename MethodType,
		typename RetVal,
		typename ...ArgsIn,
		typename ...ArgsPart
	>
		static RetVal extractTuple(
			ClassType* obj,
			MethodType func,
			const std::tuple<ArgsIn...>& inParams,
			ArgsPart... partParams)
	{
		return tuple_extractor2<n - 1>::extractTuple<ClassType, 
			MethodType, RetVal, ArgsIn..., ArgsPart...>
			(obj,
			func,
			inParams,
			std::get<n - 1>(inParams),
			partParams...);
	}
};

//===============================================================
//===============================================================
//===============================================================

template <>
struct tuple_extractor2<0>
{
	template <
		typename ClassType,
		typename MethodType,
		typename RetVal,
		typename ...ArgsIn,
		typename ...ArgsPart
	>
		static RetVal extractTuple(
			ClassType* obj,
			MethodType func,
			const std::tuple<ArgsIn...>& inParams,
			ArgsPart... partParams)
	{
		return (obj->*func)(partParams...);
	}
};




#endif // !TUPLE_EXTRACT_H

