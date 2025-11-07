#pragma once
#ifndef _CSELECTIONLIST_H
#define _CSELECTIONLIST_H

#include "common.h"
#include "Reuben\Math\Random.h"
#include "Reuben\System\Reuben.h"

class CSelectionList
{
public:
	CSelectionList(UInt32Vector& vectorOfValue, UInt32Vector& weighVector);
	~CSelectionList();

	UInt32 Get();
	VOID Reset();

private:
	UInt32Vector GetValueVector()	{return m_value;};
	UInt32Vector GetWeighVector()	{return m_weigh;};
	UInt32 GetTempTotal();
	VOID ResetTempVariables();

private:
	UInt32Vector	m_value;
	UInt32Vector	m_weigh;
	UInt32Vector	m_tempValue;
	UInt32Vector	m_tempWeigh;
	Reuben::Math::Random m_rand;
};

#endif
