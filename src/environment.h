#pragma once
#include "../helpers/hash_map.h"

namespace env {

	enum class Var_proc_type {
		MUT,
		LET,
		PROC,
		METHOD
	};

	class Environment {
	public:
		Environment() : prev(nullptr) {
		}
		explicit Environment(Environment* prev) : prev(prev) {};
		Environment(Environment&) = delete;
		Environment(Environment&&) = delete;
		Environment& operator=(Environment&) = delete;

		void store_entry( string tag, any object)  {
			decl.put(tag, object);
		}
		any get_entry(const string tag) {
			return decl.get_value(tag);
		}

		bool contains_identifier(const string ident) {
			return decl.contain_key(ident);
		}

		void store_identifier_label(string tag, Var_proc_type type) {
			identifier_verification.put(tag, type);
		}
		HashMap<string, Var_proc_type> get_identifier_verification() {
			return identifier_verification;
		}
		
	private:
		hash::HashMap<string, Var_proc_type> identifier_verification;
		Environment* prev;
		hash::HashMap<string, any> decl;
		
	};

};//end namespace env;