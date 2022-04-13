// Copyright 2021 Justin Kiesskalt, All Rights Reserved.

#pragma once
#include "Misc/OutputDevice.h"

template<typename T>
class FOSLOutputDevice : public FOutputDevice
{
public:
	FOSLOutputDevice()
	{
		GLog->AddOutputDevice(this);
	}

	~FOSLOutputDevice()
	{
		if (GLog != nullptr)
		{
			GLog->RemoveOutputDevice(this);
		}
	}

	virtual void Serialize(const TCHAR* V, ELogVerbosity::Type Verbosity, const FName& Category) override
	{
		if (ContextObject)
		{
			ContextObject->OnLog(V, Verbosity, Category);
		}
	}

	void SetContextObject(T* InListener)
	{
		ContextObject = InListener;
	}

private:
	T* ContextObject;
};
