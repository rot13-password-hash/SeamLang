#pragma once

#include <memory>
#include <string>
#include <variant>
#include <vector>
#include <unordered_set>
#include <stdexcept>

#include "node.hpp"

namespace seam::ir::ast
{
	struct class_descriptor
	{
		
	};

	struct type
	{
		enum class built_in_type
		{
			auto_,
			void_,
			bool_,
			string,
			i8,
			i16,
			i32,
			i64,
			u8,
			u16,
			u32,
			u64,
			f32,
			f64
		};

		std::variant<built_in_type, class_descriptor> value;

		bool is(built_in_type t)
		{
			if (const auto built_in = std::get_if<built_in_type>(&value))
			{
				return t == *built_in;
			}
			else
			{
				throw std::runtime_error("class types are not supported");
			}
		}

		explicit type(built_in_type t) :
			value(t)
		{}
	};
}
