#pragma once

#include <filesystem>
#include <llvm/Object/Error.h>
#include <llvm/Support/Error.h>
#include <llvm/Support/WithColor.h>

#include "position.hpp"

namespace seam::utils
{
	struct error_info final : llvm::ErrorInfo<error_info>
	{
		static char ID;
		
		std::filesystem::path file;
		position pos;
		std::string msg;

		explicit error_info(std::filesystem::path file, position pos, std::string msg) :
			file(std::move(file)), pos(pos), msg(std::move(msg)) {}

		void log(llvm::raw_ostream& os) const override
		{
			os << file.string() << ':' << pos.column << ':' << pos.line << ": ";
			llvm::WithColor::error(os) << msg;
		}

		[[nodiscard]] std::error_code convertToErrorCode() const override
		{
			return make_error_code(llvm::object::object_error::parse_failed);
		}
	};
}
