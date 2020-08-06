/*
	This file is part of solidity.

	solidity is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	solidity is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with solidity.  If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once


#include <libyul/AsmDataForward.h>
#include <libyul/optimiser/ASTWalker.h>
#include <libyul/YulString.h>
#include <libyul/optimiser/OptimiserStep.h>

#include <map>
#include <set>
#include <string>

namespace yul
{

struct Dialect;

/**
 * Pass to "simplify" all identifier names.
 *
 * The purpose of this is to make generated code more readable, but also
 * to remove AST identifiers that could lead to a different sorting order
 * and thus influence e.g. the order of function inlining.
 *
 * Prerequisites: Disambiguator, FunctionHoister, FunctionGrouper
 */
class NameSimplifier: public ASTModifier
{
public:
	static constexpr char const* name{"NameSimplifier"};
	static void run(OptimiserStepContext& _context, Block& _ast)
	{
		NameSimplifier{_ast, _context.dialect, _context.reservedIdentifiers}(_ast);
	}

	using ASTModifier::operator();
	void operator()(VariableDeclaration& _varDecl) override;
	void operator()(Identifier& _identifier) override;
	void operator()(FunctionDefinition& _funDef) override;

private:
	NameSimplifier(
		Block const& _ast,
		Dialect const& _dialect,
		std::set<YulString> _fixed = {}
	);

	/// Tries to rename a list of variables.
	void renameVariables(std::vector<TypedName>& _variables);

	void findSimplification(YulString _name);

	/// Tests whether a given name was already used within this pass
	/// or is on the blacklist.
	bool isUsedName(YulString const& _name) const;

	Dialect const& m_dialect;
	std::set<YulString> m_usedNames;
	std::map<YulString, YulString> m_translations;
};

}
