#pragma once

#include <cstdint>
#include <cstring>
#include <type_traits>

class DynamicPool
{
public:
	DynamicPool()
	: m_Data(nullptr)
	, m_Size(0)
	{
	}

	~DynamicPool()
	{
		delete[] m_Data;
	}

	DynamicPool(const DynamicPool &other) = delete;
	DynamicPool(const DynamicPool &&other) = delete;
	DynamicPool& operator=(DynamicPool &other) = delete;
	DynamicPool& operator=(DynamicPool &&other) = delete;

	template<typename T>
	T &Get(size_t index)
	{
		static_assert(
			std::is_trivially_copyable_v<T>,
			"Only supports trivially copyable types !"
		);

		if (m_Size <= index)
		{
			size_t newSize = m_Size;

			while (index >= newSize)
			{
				newSize = newSize == 0 ? 1 : newSize * 2;
			}

			size_t oldSize = m_Size * sizeof(T);
			uint8_t *oldData = new uint8_t[oldSize]; 
			memcpy(oldData, m_Data, oldSize);

			delete[] m_Data;

			m_Size = newSize;
			m_Data = new uint8_t[newSize * sizeof(T)];
			memcpy(m_Data, oldData, oldSize);

			delete[] oldData;
		}

		return *reinterpret_cast<T*>(m_Data + (index * sizeof(T)));
	}

private:
	uint8_t *m_Data;
	size_t m_Size;
};