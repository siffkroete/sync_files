#pragma once

/**
Created by pei 13.07.2016
**/

template <class T>
class MySingleton
{
public:
	inline static T* getSingleton()
	{
		if (s_pSingleton == nullptr)
		{
			s_pSingleton = new T();
		}
		return s_pSingleton;
	}

	inline static void del()
	{
		if (s_pSingleton != nullptr)
		{
			delete s_pSingleton;
			s_pSingleton = nullptr;
		}
	}

	virtual ~MySingleton(void)
	{
	}

protected:
	MySingleton(void)
	{
	}

	static T* s_pSingleton;
};

template <class T>
T* MySingleton<T>::s_pSingleton = nullptr;

