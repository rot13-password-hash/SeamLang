#pragma once

#include "../../utils/position.hpp"

namespace seam::ir::ast
{
	struct base_visitor;
	struct node
	{
		utils::position_range range; // the range of text the node ranges over. 

		/**
		 * Virtual visit method for base_visitor pattern.
		 *
		 * @param vst base_visitor
		 * @note not implemented for base node, will throw exception.
		 */
		virtual void visit(base_visitor* vst) = 0;

		virtual ~node() = default;
	protected:
		explicit node(const utils::position_range range)
			: range(range) {}
	};
}
