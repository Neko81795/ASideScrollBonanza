#include "ComponentHandle.h"

namespace ASSB
{
	template<typename T>
	inline ComponentHandle<T>::ComponentHandle(Globals::ObjectID objectId, std::unordered_map<unsigned long long, std::unique_ptr<T>>& map) : Map(map), Id(objectId)
	{

	}

	template<typename T>
	inline ComponentHandle<T>::operator bool()
	{
		return Map.find(Id) != Map.end();
	}

	template<typename T>
	inline const T * ComponentHandle<T>::operator->() const
	{
		return Map.at(Id);
	}

	template<typename T>
	T * ComponentHandle<T>::operator->()
	{
		return &*Map.at(Id);
	}

	template<typename T>
	const T& ComponentHandle<T>::operator*() const
	{
		return *Map.at(Id);
	}

	template<typename T>
	T& ComponentHandle<T>::operator*()
	{
		return *Map.at(Id);
	}

	template<typename T>
	ComponentHandle<T> & ComponentHandle<T>::operator=(const ComponentHandle & other)
	{
		Id = other.Id;
		return *this;
	}
}
