#pragma once

#include "../../utils/position.h"

namespace seam::ir::ast
{
	struct visitor;
	struct node
	{
		utils::position_range range; // the range of text the node ranges over. 

		/**
		 * Virtual visit method for visitor pattern.
		 *
		 * @param vst visitor
		 * @note not implemented for base node, will throw exception.
		 */
		virtual void visit(visitor* vst) = 0;

		virtual ~node() = default;
	protected:
		explicit node(const utils::position_range range)
			: range(range) {}
	};
}
