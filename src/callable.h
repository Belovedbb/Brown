#pragma once
#include "environment.h"
#include "../helpers/array_list.h"

using list::ArrayList;
using env::Environment;

namespace calls_interface {

	class Procedure_callable {
	public:
		virtual any call(Environment&, ArrayList<any>) = 0;
		virtual int arity() const = 0;
	};

	//mut
	//let
};