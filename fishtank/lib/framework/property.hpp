#ifndef __INCLUDED_LIB_FRAMEWORK_PROPERTY_HPP__
#define __INCLUDED_LIB_FRAMEWORK_PROPERTY_HPP__

template <typename T, typename Class, const T& (Class::*setter)(T& curVal, const T& newVal) = 0>
class Property : public T
{
	public:
		Property(Class* owner) :
			owner(owner)
		{
		}

		Property(Class* owner, const T& val) :
			T(val),
			owner(owner)
		{
			if (setter)
				(owner->*setter)(*this, val);
		}

		const T& operator=(const T& val)
		{
			if (setter)
				return (owner->*setter)(*this, val);
			else
				return static_cast<T&>(*this) = val;
		}

		const T& operator+= (const T& val) { return *this = *this  + val; }
		const T& operator-= (const T& val) { return *this = *this  - val; }
		const T& operator*= (const T& val) { return *this = *this  * val; }
		const T& operator/= (const T& val) { return *this = *this  / val; }
		const T& operator%= (const T& val) { return *this = *this  % val; }
		const T& operator&= (const T& val) { return *this = *this  & val; }
		const T& operator|= (const T& val) { return *this = *this  | val; }
		const T& operator^= (const T& val) { return *this = *this  ^ val; }
		const T& operator<<=(const T& val) { return *this = *this << val; }
		const T& operator>>=(const T& val) { return *this = *this >> val; }
		const T& operator++ () { return *this = ++*this; }
		const T& operator-- () { return *this = --*this; }
		T operator++(int) { const T old(*this); *this = ++*this; return old; }
		T operator--(int) { const T old(*this); *this = --*this; return old; }

	private:
		Class* owner;
};

template <typename T, typename Class, const T& (Class::*setter)(T& curVal, const T& newVal) = 0>
class ScalarProperty
{
	public:
		ScalarProperty(Class* owner) :
			owner(owner)
		{
		}

		ScalarProperty(Class* owner, const T& val) :
			store(val),
			owner(owner)
		{
			if (setter)
				(owner->*setter)(store, val);
		}

		const T& operator=(const T& val)
		{
			if (setter)
				return (owner->*setter)(store, val);
			else
				return static_cast<T&>(store) = val;
		}

		operator const T&() const
		{
			return store;
		}

		const T& operator+= (const T& val) { return *this = *this  + val; }
		const T& operator-= (const T& val) { return *this = *this  - val; }
		const T& operator*= (const T& val) { return *this = *this  * val; }
		const T& operator/= (const T& val) { return *this = *this  / val; }
		const T& operator%= (const T& val) { return *this = *this  % val; }
		const T& operator&= (const T& val) { return *this = *this  & val; }
		const T& operator|= (const T& val) { return *this = *this  | val; }
		const T& operator^= (const T& val) { return *this = *this  ^ val; }
		const T& operator<<=(const T& val) { return *this = *this << val; }
		const T& operator>>=(const T& val) { return *this = *this >> val; }
		const T& operator++ () { return *this = ++*this; }
		const T& operator-- () { return *this = --*this; }
		T operator++(int) { const T old(*this); *this = ++*this; return old; }
		T operator--(int) { const T old(*this); *this = --*this; return old; }

	private:
		T store;
		Class* owner;
};

#endif // __INCLUDED_LIB_FRAMEWORK_PROPERTY_HPP__
