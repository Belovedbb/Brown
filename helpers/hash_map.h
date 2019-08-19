#pragma once
#include "../helpers/linked_list.h"
namespace hash {

	using list::LinkedList;

	template <typename K, typename V>
	class Entry {
	public:
		Entry(){}
		Entry(K key, V value) {
			this->key = key;
			this->value = value;
		}
		
		K get_entry_key() const {
			return key;
		}
		
		V get_entry_value() const {
			return value;
		}

	private:
		K key;
		V value;
	};

	template <typename K, typename V>
	class HashMap : private LinkedList<Entry<K,V>>{
	public:
		HashMap();
		HashMap& operator=(const HashMap&) = delete;
		HashMap(const HashMap&);
		HashMap(const HashMap&&) = delete;
		HashMap(int initial_cap, float load_factor);
		void put(K key, V value);
		V get_value(K key);
		//return the key to the first value
		K get_key(V value);
		inline LinkedList<Entry<K, V>> get_entry(int index);
		bool contain_key(K key);
		//search at O(n) time
		bool contain_value(V value);
		inline size_t size() const;
		inline bool is_empty() const;
		~HashMap();
	private:
		float load_factor = 0.0f;
		int ini_cap = 0;
		int len = 0;
		unsigned int generate_hash_code(K key);
		unsigned int compress_hash(const unsigned int& hash_code);
		void rehash();
		LinkedList<Entry<K, V>>* table;
	};

	template<typename K, typename V>
	HashMap<K, V>::HashMap() : HashMap(4, 0.75)
	{
	}

	template<typename K, typename V>
	HashMap<K, V>::HashMap(int initial_cap, float load_factor)
	{
			if (initial_cap > (1 << 30) )
				 initial_cap = (1 << 30) ;
			if (load_factor >= 1.0f)
				load_factor = 0.9f;
			this->ini_cap = initial_cap;
			this->load_factor = load_factor;
			table = new LinkedList<Entry<K, V>>[this->ini_cap];
	}

	template<typename K, typename V>
	HashMap<K, V>::HashMap(const HashMap<K, V>& other)
	{
		this->ini_cap = other.ini_cap;
		const auto old_tab = other.table;
		load_factor = other.load_factor;
		len = other.len;
		auto this_table = new LinkedList<Entry<K, V>>[ini_cap];
		table = this_table;
		for (int i = 0; i < ini_cap; i++) {
			for (int j = 0; j < old_tab[i].size(); j++) {
				this_table[i].add(old_tab[i].get(j));
			}
		}
	}

	template<typename K, typename V>
	void HashMap<K, V>::put(K key, V value)
	{
		if (len >= ini_cap * load_factor)
			rehash();

		long hash_key = compress_hash(generate_hash_code(key));

		if(nullptr == &table[hash_key]){
			table[hash_key] = new LinkedList<Entry<K, V>>;
		}
		else {
			for (int i = 0; i < table[hash_key].size(); i++) {
				if (table[hash_key].get(i).get_entry_key() == key) {
					table[hash_key].remove(i);
					Entry<K, V> bucket(key, value);
					table[hash_key].add(bucket);
					return;
				}

			}
		}

		Entry<K, V> bucket(key, value);
		table[hash_key].add(bucket);
		++len;
	}

	template<typename K, typename V>
	V HashMap<K, V>::get_value(K key)
	{
		int hash_key = compress_hash(generate_hash_code(key));
			LinkedList<Entry<K, V>> bucket = table[hash_key];
			for (int i = 0; i < bucket.size(); i++) {
				if ( bucket.get(i).get_entry_key() == key)
					return bucket.get(i).get_entry_value();
			}
	}
	template<typename K, typename V>
	LinkedList<Entry<K, V>> HashMap<K,V>::get_entry(int index) {
			return table[index];
	}

	template<typename K, typename V>
	K HashMap<K, V>::get_key(V value)
	{
		for (int i = 0; i < ini_cap; i++) {
			if (&table[i] != nullptr) {
				for (int j = 0; j < table[i].size(); j++) {
					if (table[i].get(j).get_entry_value() == value)
						return table[i].get(j).get_entry_key();
				}
			}
		}

	}

	template<typename K, typename V>
	bool HashMap<K, V>::contain_key(K key)
	{
		long hash_key = compress_hash(generate_hash_code(key));
		if (&table[hash_key] != nullptr) {
			for (int i = 0; i < table[hash_key].size(); i++) {
				if (table[hash_key].get(i).get_entry_key() == key)
					return true;
			}
		}
		return false;
	}

	
	template<typename K, typename V>
	bool HashMap<K, V>::contain_value(V value)
	{
		for (int i = 0; i < ini_cap; i++) {
			if (&table[i] != nullptr) {
				for (int j = 0; j < table[i].size(); j++) {
					if (table[i].get(j).get_entry_value() == value)
						return true;
				}
			}
		}
		return false;
	}


	template<typename K, typename V>
	size_t HashMap<K, V>::size() const
	{
		return len;
	}

	

	template<typename K, typename V>
	bool HashMap<K, V>::is_empty() const
	{
		return len == 0;
	}

	template<typename K, typename V>
	HashMap<K, V>::~HashMap()
	{
		for (int i = 0; i < ini_cap; i++) {
			LinkedList<Entry<K,V>>* bucket = &table[i];
			for (int j = 0; j < bucket->size(); j++)
				bucket->remove(j);
		}
	}

	template<typename K, typename V>
	unsigned int HashMap<K, V>::generate_hash_code(K key)
	{
		return typeid(key).hash_code();
	}

	template<typename K, typename V>
	unsigned int HashMap<K, V>::compress_hash(const unsigned int& hash_code)
	{
		return hash_code & (ini_cap - 1);
	}

	template<typename K, typename V>
	void HashMap<K, V>::rehash()
	{
		auto old_cap = ini_cap;
		ini_cap <<= 1;
		const auto old_tab = table;
		table = new LinkedList<Entry<K, V>>[ini_cap];
		for (int i = 0; i < old_cap; i++) {
			for (int j = 0; j < old_tab[i].size(); j++) {
				int rehashed_key = compress_hash(generate_hash_code(old_tab[i].get(j).get_entry_key()));
				table[rehashed_key].add( old_tab[i].get(j) );
			}
		}

		delete[] old_tab;

	}

};//end namespace hash;