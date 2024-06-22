#pragma once
#include "global_types.h"

#define EmitAccumulativeOperator(Name, Operation) \
template<typename value_type> \
value_type &Name(value_type &left, value_type right) \
{ \
	return Operation(left, right); \
}
#define PrimitiveMove(arg1, arg2) (arg1 = arg2)
EmitAccumulativeOperator(move, PrimitiveMove);
#undef PrimitiveMove
#define PrimitiveAdd(arg1, arg2) (arg1 += arg2)
EmitAccumulativeOperator(add, PrimitiveAdd);
#undef PrimitiveAdd
#define PrimitiveSub(arg1, arg2) (arg1 -= arg2)
EmitAccumulativeOperator(sub, PrimitiveSub);
#undef PrimitiveSub
#define PrimitiveMul(arg1, arg2) (arg1 *= arg2)
EmitAccumulativeOperator(mul, PrimitiveMul);
#undef PrimitiveMul
#define PrimitiveDiv(arg1, arg2) (arg1 /= arg2)
EmitAccumulativeOperator(div, PrimitiveDiv);
#undef PrimitiveDiv
#define PrimitiveModDiv(arg1, arg2) (arg1 %= arg2)
EmitAccumulativeOperator(moddiv, PrimitiveModDiv);
#undef PrimitiveModDiv
#define PrimitiveAnd(arg1, arg2) (arg1 &= arg2)
EmitAccumulativeOperator(andeq, PrimitiveAnd);
#undef PrimitiveAnd
#define PrimitiveOr(arg1, arg2) (arg1 |= arg2)
EmitAccumulativeOperator(oreq, PrimitiveOr);
#undef PrimitiveOr
#define PrimitiveXor(arg1, arg2) (arg1 ^= arg2)
EmitAccumulativeOperator(xoreq, PrimitiveXor);
#undef PrimitiveXor
#define PrimitiveLsh(arg1, arg2) (arg1 <<= arg2)
EmitAccumulativeOperator(lsh, PrimitiveLsh);
#undef PrimitiveLsh
#define PrimitiveRsh(arg1, arg2) (arg1 >>= arg2)
EmitAccumulativeOperator(rsh, PrimitiveRsh);
#undef PrimitiveRsh
#define PrimitiveLogicalAnd(arg1, arg2) (arg1 = arg1 && arg2)
EmitAccumulativeOperator(logical_and, PrimitiveLogicalAnd);
#undef PrimitiveLogicalAnd
#define PrimitiveLogicalOr(arg1, arg2) (arg1 = arg1 || arg2)
EmitAccumulativeOperator(logical_or, PrimitiveLogicalOr);
#undef PrimitiveLogicalOr
#undef EmitAssignOperator

#define EmitUnaryOperator(Name, Operation) \
template<typename value_type> value_type &Name(value_type &value) \
{ \
	return Operation(value); \
}
#define PrimitiveInc(arg) (arg++)
EmitUnaryOperator(inc, PrimitiveInc);
#undef PrimitiveInc
#define PrimitivePreInc(arg) (++arg)
EmitUnaryOperator(preinc, PrimitivePreInc);
#undef PrimitivePreInc
#define PrimitiveDec(arg) (arg--)
EmitUnaryOperator(dec, PrimitiveDec);
#undef PrimitiveDec
#define PrimitivePreDec(arg) (--arg)
EmitUnaryOperator(predec, PrimitivePreDec);
#undef PrimitivePreDec
#define PrimitiveNegate(arg) (arg = -arg)
EmitUnaryOperator(negate, PrimitiveNegate);
#undef PrimitiveNegate
#define PrimitiveInvert(arg) (arg = ~arg)
EmitUnaryOperator(bit_invert, PrimitiveInvert);
#undef PrimitiveInvert
#define PrimitiveLogicalInvert(arg) (arg = !arg)
EmitUnaryOperator(logical_invert, PrimitiveLogicalInvert);
#undef PrimitiveLogicalInvert
#undef EmitUnaryAssignOperator

#define EmitUnaryValueOperator(Name, Operation) \
template<typename value_type> \
constexpr value_type Name(value_type value) \
{ \
	return Operation(value); \
}
#define PrimitiveReflect(arg) (arg)
EmitUnaryValueOperator(reflect, PrimitiveReflect);
#undef PrimitiveReflect
#define PrimitiveNegative(arg) (-arg)
EmitUnaryValueOperator(negative, PrimitiveNegative);
#undef PrimitiveNegative
#define PrimitiveInversion(arg) (~arg)
EmitUnaryValueOperator(bit_inversion, PrimitiveInversion);
#undef PrimitiveInvertion
#define PrimitiveNot(arg) (!arg)
EmitUnaryValueOperator(logical_inversion, PrimitiveNot);
#undef PrimitiveNot
#undef EmitUnaryValueOperator

#define EmitValueOperator(Name, Operation) \
template<typename value_type, typename ...Args> \
constexpr value_type Name(value_type arg, Args... args) \
{ \
	if constexpr(sizeof...(Args) == 1) \
		return Operation(arg, reflect(args...)); \
	else return Operation(arg, Name<Args...>(args...)); \
}
#define PrimitiveMin(arg1, arg2) ((arg1) < (arg2) ? (arg1) : (arg2))
EmitValueOperator(min, PrimitiveMin);
#undef PrimitiveMin
#define PrimitiveMax(arg1, arg2) ((arg1) < (arg2) ? (arg2) : (arg1))
EmitValueOperator(max, PrimitiveMax);
#undef PrimitiveMax
#define PrimitiveSum(arg1, arg2) (arg1 + arg2)
EmitValueOperator(sum, PrimitiveSum);
#undef PrimitiveSum
#define PrimitiveDiff(arg1, arg2) (arg1 - arg2)
EmitValueOperator(diff, PrimitiveDiff);
#undef PrimitiveDiff
#define PrimitiveProduct(arg1, arg2) (arg1 * arg2)
EmitValueOperator(product, PrimitiveProduct);
#undef PrimitiveProduct
#define PrimitiveQuotient(arg1, arg2) (arg1 / arg2)
EmitValueOperator(quotient, PrimitiveQuotient);
#undef PrimitiveQuotient
#define PrimitiveModQuotient(arg1, arg2) (arg1 % arg2)
EmitValueOperator(modquotient, PrimitiveModQuotient);
#undef PrimitiveModQuotient
#define PrimitiveAndProduct(arg1, arg2) (arg1 & arg2)
EmitValueOperator(and_product, PrimitiveAndProduct);
#undef PrimitiveAndProduct
#define PrimitiveOrProduct(arg1, arg2) (arg1 | arg2)
EmitValueOperator(or_product, PrimitiveOrProduct);
#undef PrimitiveOrProduct
#define PrimitiveXorProduct(arg1, arg2) (arg1 ^ arg2)
EmitValueOperator(xor_product, PrimitiveXorProduct);
#undef PrimitiveXorProduct
#define PrimitiveLshProduct(arg1, arg2) (arg1 << arg2)
EmitValueOperator(lsh_product, PrimitiveLshProduct);
#undef PrimitiveLshProduct
#define PrimitiveRshProduct(arg1, arg2) (arg1 >> arg2)
EmitValueOperator(rsh_product, PrimitiveRshProduct);
#undef PrimitiveRshProduct
#define PrimitiveLogicalAndProduct(arg1, arg2) (arg1 && arg2)
EmitValueOperator(logical_and_product, PrimitiveLogicalAndProduct);
#undef PrimitiveLogicalAndProduct
#define PrimitiveLogicalOrProduct(arg1, arg2) (arg1 || arg2)
EmitValueOperator(logical_or_product, PrimitiveLogicalOrProduct);
#undef PrimitiveLogicalOrProduct
#undef EmitValueOperator

#define EmitComparisonOperator(Name, Operation) \
template<typename value_type> \
constexpr bool Name(value_type arg1, value_type arg2) \
{ \
	return Operation(arg1, arg2); \
}
#define PrimitiveEqual(arg1, arg2) (arg1 == arg2)
EmitComparisonOperator(equal, PrimitiveEqual);
#undef PrimitiveEqual
#define PrimitiveNotEqual(arg1, arg2) (arg1 != arg2)
EmitComparisonOperator(not_equal, PrimitiveNotEqual);
#undef PrimitiveNotEqual
#define PrimitiveLess(arg1, arg2) (arg1 < arg2)
EmitComparisonOperator(less, PrimitiveLess);
#undef PrimitiveLess
#define PrimitiveGreater(arg1, arg2) (arg1 > arg2)
EmitComparisonOperator(greater, PrimitiveGreater);
#undef PrimitiveGreater
#define PrimitiveLessEqual(arg1, arg2) (arg1 <= arg2)
EmitComparisonOperator(less_equal, PrimitiveLessEqual);
#undef PrimitiveLess
#define PrimitiveGreaterEqual(arg1, arg2) (arg1 >= arg2)
EmitComparisonOperator(greater_equal, PrimitiveGreaterEqual);
#undef PrimitiveGreaterEqual
#undef EmitComparisonOperator

template<typename return_type, typename... args_type>
return_type empty_function_instance(args_type... args)
{
	return return_type();
}

template<typename function_type> struct empty_function_generator {};

template<typename return_type, typename... args_type>
struct empty_function_generator<return_type(*)(args_type...)>
{
	return_type (*instance())(args_type...)
	{
		return empty_function_instance<return_type, args_type...>;
	}
};

template<typename function_type> function_type empty_function()
{
	return empty_function_generator<function_type>().instance();
}

template<typename value_type>
void construct(value_type *source)
{
	new(source) value_type();
}

template<typename value_type>
void construct_range(value_type *begin, value_type *end)
{
	while(begin != end)
	{
		construct(begin);
		begin++;
	}
}

template<typename value_type>
void destroy(value_type *source)
{
	source->~value_type();
}

template<typename value_type>
void destroy_range(value_type *begin, value_type *end)
{
	while(begin != end)
	{
		destroy(begin);
		begin++;
	}
}

template<typename value_type>
void copy(const value_type &source, value_type *target)
{
	utility<value_type>().copy(source, target);
}

template<typename value_type>
void move_addr(value_type **value_addr, int64 offset)
{
	*value_addr = static_cast<value_type *>(static_cast<void *>(
		*static_cast<byte **>(static_cast<void *>(value_addr)) + offset));
}

template<uint32 size>
void static_copy(const void *source, void *target)
{
	struct wrapper
	{
		byte data[size];
	};
	*(static_cast<wrapper *>(target)) = *(static_cast<wrapper *>(const_cast<void *>(source)));
}

template<typename value_type>
void swap(value_type *addr1, value_type *addr2)
{
	byte temp[sizeof(value_type)];
	*(static_cast<value_type *>(static_cast<void *>(temp))) = *addr1;
	*addr1 = *addr2;
	*addr2 = *(static_cast<value_type *>(static_cast<void *>(temp)));
}

inline void copy_memory(const void *source, void *target, uint64 size)
{
	while(size--)
	{
		static_copy<1>(source, target);
		move_addr(&source, 1);
		move_addr(&target, 1);
	}
}

inline void move_memory(void *source, void *target, uint64 size)
{
	if(target < source)
	{
		while(size--)
		{
			static_copy<1>(source, target);
			move_addr(&source, 1);
			move_addr(&target, 1);
		}
	}
	else
	{
		move_addr(&source, size - 1);
		move_addr(&target, size - 1);
		while(size--)
		{
			static_copy<1>(source, target);
			move_addr(&source, -1);
			move_addr(&target, -1);
		}
	}
}

enum struct compare_result : uint8
{
	equal,
	less,
	greater
};

template<typename stride_type = byte>
compare_result compare_memory(void *addr1, void *addr2, uint64 size)
{
	while(size != 0)
	{
		if(*((stride_type *)(addr1)) != *((stride_type *)(addr2)))
		{
			if(*((stride_type *)(addr1)) < *((stride_type *)(addr2)))
				return compare_result::less;
			else return compare_result::greater;
		}
		move_addr(&addr1, sizeof(stride_type));
		move_addr(&addr2, sizeof(stride_type));
		size -= sizeof(stride_type);
	}
	return compare_result::equal;
}

template<typename value_type, uint64 size>
constexpr uint64 array_size(value_type (&)[size])
{
	return size;
}

template<uint32 iterations, typename operator_type, typename ...source>
void coherent_run(operator_type function, source... source_addr)
{
	function(*(source_addr)...);
	if constexpr(iterations != 1)
		coherent_run<iterations - 1>(function, (source_addr + 1)...);
}

template<
	uint32 iterations,
	typename operator_type,
	typename target,
	typename ...source>
void coherent_mapping(
	operator_type function,
	target target_addr,
	source... source_addr)
{
	*target_addr = function(*source_addr...);
	if constexpr(iterations != 1)
		coherent_mapping<iterations - 1>(
			function,
			target_addr + 1,
			(source_addr + 1)...);
}

template<
	uint32 iterations,
	typename value_type,
	typename mapping_operator,
	typename accumulative_operator,
	typename ...source>
value_type accumulate(
	mapping_operator mapping_function,
	accumulative_operator accumulative_function,
	value_type initial_value,
	source... source_addr)
{
	if constexpr(iterations == 1)
		return accumulative_function(
			initial_value,
			mapping_function(*(source_addr)...));
	else return accumulate<iterations - 1>(
		mapping_function,
		accumulative_function,
		accumulative_function(
			initial_value,
			mapping_function(*(source_addr)...)),
		(source_addr + 1)...);
}

template<uint32 iterations, typename operator_type, typename ...source>
bool conjunction(operator_type function, source... source_addr)
{
	if constexpr(iterations == 1)
		return function(*(source_addr)...);
	else return function(*(source_addr)...) ?
		conjunction<iterations - 1>(function, (source_addr + 1)...) : false;
}

template<uint32 iterations, typename operator_type, typename ...source>
bool disjunction(operator_type function, source... source_addr)
{
	if constexpr(iterations == 1)
		return function(*(source_addr)...);
	else return function(*(source_addr)...) ?
		true : disjunction<iterations - 1>(function, (source_addr + 1)...);
}
