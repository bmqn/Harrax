#pragma once

#include <memory>

template<typename T>
class Result
{
public:
	template<typename T, typename... Args>
	friend constexpr Result<T> Success(Args... args);
	
	template<typename T>
	friend constexpr Result<T> Error();

	operator bool() const { return m_Value != nullptr; }
	const T& operator()() const { return *m_Value; }
	T& operator()() { return *m_Value; }

private:
	Result(T *value)
		: m_Value(value)
	{
	}

private:
	std::unique_ptr<T> m_Value;
};

template<typename T, typename... Args>
constexpr Result<T> Success(Args... args)
{
	return Result(new T(args...));
}

template<typename T>
constexpr Result<T> Error()
{
	return Result(nullptr);
}