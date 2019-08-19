#pragma once
#include "headers.h"

namespace list {
	//******************************Declaration******************************************************************//
	template <typename T>
	class ArrayList
	{
	public:
		explicit ArrayList(std::initializer_list<T> init_list);
		ArrayList();
		ArrayList& operator=(ArrayList& list);
		ArrayList(const ArrayList& list);
		ArrayList(ArrayList&& list) = delete;
		~ArrayList();
		void add(const T&);
		T get(const size_t index);
		bool is_empty();
		void clear();
		T remove(size_t index);
		inline const size_t size() const;
	private:
		size_t capacity;
		size_t length;
		T* internal_array;
		void grow_buf();
		void copy_elements(T* old, T* future);
	};
	//******************************End Declaration******************************************************************//

	//******************************Definition******************************************************************//
	enum { INCREMENT = 5 };

	template<typename T>
	ArrayList<T>::ArrayList(std::initializer_list<T> init_list)
	{

	}
	template <typename T>
	ArrayList<T>::ArrayList()
	{
		length = 0;
		capacity = INCREMENT;
		internal_array = new T[capacity];
	}

	template<typename T>
	ArrayList<T>::ArrayList(const ArrayList& list)
	{
		this->capacity = list.capacity;
		this->length = list.length;
		T* alloc = new T[list.capacity];
		copy_elements(list.internal_array, alloc);
		internal_array = alloc;
	}
	template < typename T>
	ArrayList<T>& ArrayList<T>::operator=(ArrayList<T>& list) {
		this->capacity = list.capacity;
		this->length = list.length;
		T* alloc = new T[list.capacity];
		copy_elements(list.internal_array, alloc);
		internal_array = alloc;
		return *this;
	}


	template <typename T>
	ArrayList<T>::~ArrayList()
	{
		if (internal_array != nullptr )
			delete[] internal_array;
		internal_array = nullptr;
	}

	template<typename T>
	void ArrayList<T>::add(const T& element)
	{
		if (length >= capacity)
			grow_buf();
		internal_array[length++] = element;
	}

	template<typename T>
	T ArrayList<T>::get(size_t index)
	{
		if (length == 0 || index > length)
			exit(-1);
		return internal_array[index];
	}

	template<typename T>
	bool ArrayList<T>::is_empty()
	{
		return size() == 0;
	}

	template<typename T>
	void ArrayList<T>::clear()
	{
		delete[] internal_array;
		internal_array = nullptr;
		length = 0;
		capacity = INCREMENT;
	}

	template<typename T>
	T ArrayList<T>::remove(const size_t index)
	{
		if (index > length)
			exit(-1);
		T temp = internal_array[index];
		T * alloc = new T[capacity];
		size_t j = 0;
		for (size_t i = 0; i < length; i++, j++) {
			if (i == index) {
				j--;
				continue;
			}
			alloc[j] = internal_array[i];
		}
		delete[] internal_array;
		length--;
		internal_array = alloc;
		return temp;
	}

	template<typename T>
	const size_t ArrayList<T>::size() const
	{
		return length;
	}

	template<typename T>
	void ArrayList<T>::grow_buf()
	{
		capacity += INCREMENT * 2 + 1;
		T* alloc = new T[capacity];
		copy_elements(internal_array, alloc);
		delete[] internal_array;
		internal_array = alloc;
	}

	template<typename T>
	void ArrayList<T>::copy_elements(T * old, T * future)
	{
		for (size_t i = 0; i < length; i++)
			future[i] = old[i];
	}
	//******************************End Definition******************************************************************//
};// end namespace list
