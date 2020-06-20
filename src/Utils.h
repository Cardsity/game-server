#pragma once
#include "safe_ptr.h"
#include <algorithm>
#include <optional>
#include <random>
#include <thread>
#include <vector>
#include <time.h>
#include <map>

#define foreach(var, list) for (auto var = list->begin(); var != list->end(); ++var)

inline std::random_device rd;
inline std::mt19937 mt(rd());

template <typename Content, typename T, typename List>
bool sfMContains(List l, T what)
{
	auto pIt = std::find_if(l->begin(), l->end(), [&](auto& other)
	{
		return other.second == what;
	});
	return pIt != l->end();
}
template <typename Content, typename T, typename List>
bool sfLContains(List l, T what)
{
	auto pIt = std::find_if(l->begin(), l->end(), [&](Content& other)
	{
		return other == what;
	});
	return pIt != l->end();
}
template <typename Content, typename T, typename List>
bool LContains(List l, T what)
{
	auto pIt = std::find_if(l.begin(), l.end(), [&](Content& other)
	{
		return other == what;
	});
	return pIt != l.end();
}

template<typename Content, typename List, typename AList, typename T>
std::optional<typename List::iterator> sfgetItemByValue(AList& l, T& what)
{
	auto pIt = std::find_if(l->begin(), l->end(), [&](Content& other)
	{
		return other == what;
	});
	if (pIt != l->end())
		return pIt;
	return std::nullopt;
}
template<typename Content, typename T, typename List>
std::optional<typename List::iterator> getItemByValue(List& l, T& what)
{
	auto pIt = std::find_if(l.begin(), l.end(), [&](Content& other)
	{
		return other == what;
	});
	if (pIt != l.end())
		return pIt;
	return std::nullopt;
}

template <typename List>
List sfShuffle(sf::safe_ptr<List> list)
{
	List copy(list->begin(), list->end());
	std::shuffle(std::begin(copy), std::end(copy), mt);
	return copy;
}

template<typename T, typename W>
bool hasTimePassed(T start, W waitTime)
{
	if (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start).count() > waitTime)
		return true;
	return false;
}

template <typename T>
std::optional<T> getRandomFromList(std::vector<T> vec)
{
	std::uniform_int_distribution<int> ran(0, vec.size() - 1);
	if (vec.size() > 0)
		return vec.at(ran(mt));
	return std::nullopt;
}
template <typename T, typename Vec>
std::optional<T> sfgetRandomFromList(Vec vec)
{
	std::uniform_int_distribution<int> ran(0, vec->size() - 1);
	if (vec->size() > 0)
		return vec->at(ran(mt));
	return std::nullopt;
}

template <typename M, typename V>
std::vector<V> MapToVec(const  M & m) {
	std::vector<V> v;
	for (auto it = m.begin(); it != m.end(); ++it) {
		v.push_back(it->second);
	}
	return v;
}
template <typename V, typename M>
std::vector<V> SfMapToVec(const  M & m) {
	std::vector<V> v;
	for (auto it = m->begin(); it != m->end(); ++it) {
		v.push_back(it->second);
	}
	return v;
}

template <typename Key, typename T >
std::vector<T> slice(const std::map<Key, T>& map, int start = 0, int end = -1)
{
	auto v = MapToVec(map);
	int oldlen = v.size();
	int newlen;

	if (end == -1 || end >= oldlen) {
		newlen = oldlen - start;
	}
	else {
		newlen = end - start;
	}

	std::vector<T> nv(newlen);

	for (int i = 0; i < newlen; i++) {
		nv[i] = v[start + i];
	}
	return nv;
}
template <typename F, typename T, typename Map>
std::vector<T> sfslice(const Map& map, int start = 0, int end = -1)
{
	auto v = SfMapToVec<F>(map);
	int oldlen = v.size();
	int newlen;

	if (end == -1 || end >= oldlen) {
		newlen = oldlen - start;
	}
	else {
		newlen = end - start;
	}

	std::vector<T> nv(newlen);

	for (int i = 0; i < newlen; i++) {
		nv[i] = v[start + i];
	}
	return nv;
}
template <typename T>
class HasIsValid
{
private:
	typedef char YesType[1];
	typedef char NoType[2];

	template <typename C> static YesType& test(decltype(&C::IsValid));
	template <typename C> static NoType& test(...);


public:
	enum { value = sizeof(test<T>(0)) == sizeof(YesType) };
};
#ifndef _WIN32
inline uint64_t GetTickCountMs()
{
	struct timespec ts;

	clock_gettime(CLOCK_MONOTONIC, &ts);

	return (uint64_t)(ts.tv_nsec / 1000000) + ((uint64_t)ts.tv_sec * 1000ull);
}
#endif