// Fill out your copyright notice in the Description page of Project Settings.


#include "SG_TupleUtilityLibrary.h"
TTuple<float, float, float> USG_TupleUtilityLibrary::AddTuple(TTuple<float, float, float>& A, TTuple<float, float, float>& B)
{
	float x = A.Get<0>() + B.Get<0>();
	float y = A.Get<1>() + B.Get<1>();
	float z = A.Get<2>() + B.Get<2>();
	return MakeTuple(x, y, z);
}
