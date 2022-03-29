/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   shared_ptr.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: smun <smun@student.42seoul.kr>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/28 20:11:55 by smun              #+#    #+#             */
/*   Updated: 2022/03/29 13:12:13 by smun             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SHARED_PTR_HPP
#define SHARED_PTR_HPP

#include <cstddef>
#include "log.hpp"

template<typename T>
class SharedPtr
{
private:
	T*		_ptr;
	int*	_ref;

public:
	SharedPtr()
		: _ptr(NULL)
		, _ref(NULL)
	{}

	~SharedPtr()
	{
		if (_ref == NULL)
			return;
		if (--(*_ref) == 0)
		{
			Log::Vp("SharedPtr::~SharedPtr", "공유 포인터가 삭제되었습니다. 내부 포인터의 삭제를 시작합니다. (남은 참조 카운트:0)");
			delete _ref;
			delete _ptr;
		}
		else
			Log::Vp("SharedPtr::~SharedPtr", "공유 포인터가 삭제되었습니다. 참조 카운트를 1 낮춥니다. (남은 참조 카운트:%d)", *_ref);
	}

	SharedPtr(T* ptr)
		: _ptr(ptr)
		, _ref(new int(1))
	{
		Log::Vp("SharedPtr::SharedPtr", "공유 포인터가 생성되었습니다. (남은 참조 카운트:1)");
	}

	SharedPtr(const SharedPtr& s)
		: _ptr(s._ptr)
		, _ref(s._ref)
	{
		if (s._ref == NULL)
			return;
		++(*_ref);
		Log::Vp("SharedPtr::SharedPtr", "공유 포인터가 복사되었습니다. (남은 참조 카운트:%d)", *_ref);
	}

	SharedPtr& operator= (const SharedPtr& s)
	{
		if (this == &s)
			return *this;
		this->~SharedPtr();
		_ptr = s._ptr;
		_ref = s._ref;
		++(*_ref);
		Log::Vp("SharedPtr::operator=", "공유 포인터가 대입되었습니다. (남은 참조 카운트:%d)", *_ref);
		return *this;
	}

	T*	Load()
	{
		return _ptr;
	}
};

#endif
