/*
 * Copyright (C) 2010 Jiri Simacek
 *
 * This file is part of forester.
 *
 * forester is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * forester is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with forester.  If not, see <http://www.gnu.org/licenses/>.
 */

// DO NOT MOVE LINES IN THIS PART  --- BEGIN

// Forester headers
#include "config.h"
#include "streams.hh"

/**
 * @brief  Reports an error
 *
 * @param[in]  errMsg  The error message
 */
void reportErrorNoLocation(const char* errMsg)
{
	FA_ERROR(errMsg);
}

// DO NOT MOVE LINES IN THIS PART  --- END


// Standard library headers
#include <sstream>
#include <fstream>
#include <vector>
#include <list>
#include <set>
#include <algorithm>

// Code Listener headers
#include <cl/cl_msg.hh>
#include <cl/cldebug.hh>
#include <cl/clutil.hh>
#include <cl/code_listener.h>
#include <cl/storage.hh>
#include "../cl/ssd.h"

// Forester headers
#include "backward_run.hh"
#include "executionmanager.hh"
#include "fixpoint.hh"
#include "fixpointinstruction.hh"
#include "forestautext.hh"
#include "memplot.hh"
#include "programconfig.hh"
#include "programerror.hh"
#include "restart_request.hh"
#include "symctx.hh"
#include "symexec.hh"
#include "svtrace_lite.hh"

using namespace ssd;

#if 0
void dumpOperandTypes(std::ostream& os, const cl_operand* op) {
	os << "operand:" << std::endl;
	cltToStream(os, op->type, false);
	os << "accessors:" << std::endl;
	const cl_accessor* acc = op->accessor;
	while (acc) {
		cltToStream(os, acc->type, false);
		acc = acc->next;
	}
}
#endif

// anonymous namespace
namespace
{
/**
 * @brief  Prints the trace to output stream
 *
 * @param[in,out]  os     The output stream
 * @param[in]      trace  The trace to be printed
 *
 * @returns  Modified stream
 */
std::ostream& printTrace(
	std::ostream&             os,
	const SymState::Trace&    trace)
{
	const CodeStorage::Insn* lastInsn = nullptr;

	for (auto it = trace.crbegin(); it != trace.crend(); ++it)
	{	// traverse in the reverse order
		const SymState& state = **it;

		assert(state.GetInstr());
		const AbstractInstruction& instr = *state.GetInstr();

		const CodeStorage::Insn* origInsn = instr.insn();
		if ((nullptr != origInsn) && (lastInsn != origInsn))
		{
			std::ostringstream oss;
			oss << *origInsn;

			std::string filename = MemPlotter::plotHeap(state, "trace", &origInsn->loc);
			lastInsn = origInsn;
			os << std::setw(50) << std::left
				<< Compiler::Assembly::insnToString(*origInsn) << " " << origInsn->loc.line << " // "
				<< origInsn->loc.file << ":" << std::setw(4) << std::left
				<< origInsn->loc.line << "|  " << filename << "\n";
		}
	}

	return os;
}


/**
 * @brief  Prints the microcode trace to output stream
 *
 * @param[in,out]  os     The output stream
 * @param[in]      trace  The trace to be printed
 *
 * @returns  Modified stream
 */
std::ostream& printUcodeTrace(
	std::ostream&             os,
	const SymState::Trace&    trace)
{
	const CodeStorage::Insn* lastInsn = nullptr;

	for (auto it = trace.crbegin(); it != trace.crend(); ++it)
	{	// traverse in the reverse order
		const SymState& state = **it;

		assert(state.GetInstr());
		const AbstractInstruction& instr = *state.GetInstr();
		const CodeStorage::Insn* clInsn = instr.insn();

		os << &state;

		os << std::setw(18);
		if (instr.isTarget())
		{
			std::ostringstream addrStream;
			addrStream << &instr;

			if ((nullptr != clInsn) && (clInsn != lastInsn)
				&& (clInsn->bb->front() == clInsn))
			{
				addrStream << " (" << clInsn->bb->name() << ")";
			}

			addrStream << ":";

			os << std::left << addrStream.str();
		}
		else
		{
			os << "";
		}

		std::ostringstream osInstr;
		osInstr << instr;

		os << std::setw(40) << std::left << osInstr.str();

		if ((nullptr != clInsn) && (lastInsn != clInsn))
		{
			lastInsn = clInsn;
			os << "; " << clInsn->loc.line << ": " << *clInsn;
		}

		os << "\n";
		//MemPlotter::plotHeap(state);
	}

	return os;
}
} // namespace


class SymExec::Engine
{
private:  // data members

	TreeAut ta_;
	TreeAut fixpoint_;
	BoxMan boxMan_;

	Compiler compiler_;
	Compiler::Assembly assembly_;

	ExecutionManager execMan_;

	const ProgramConfig& conf_;

	volatile bool dbgFlag_;
	volatile bool userRequestFlag_;

	bool newPredicates_;
	std::vector<std::shared_ptr<const TreeAut>> predicates_;

protected:

	/**
	 * @brief  Prints boxes
	 *
	 * Method that prints all boxes from the box manager.
	 */
	void printBoxes() const
	{
		std::vector<const Box*> boxes;

		boxMan_.boxDatabase().asVector(boxes);

		std::map<std::string, const Box*> orderedBoxes;

		// reorder according to the name
		for (auto& box : boxes)
		{
			std::stringstream ss;

			ss << *static_cast<const AbstractBox*>(box);

			orderedBoxes.insert(std::make_pair(ss.str(), box));
		}

		for (auto& nameBoxPair : orderedBoxes)
		{
			FA_DEBUG_AT(1, nameBoxPair.first << ':' << std::endl << *nameBoxPair.second);
		}
	}

	/**
	 * @brief  Clears all fixpoints
	 */
	void clearFixpoints()
	{
		// clear all fixpoints
		for (auto instr : assembly_.code_)
		{
			if (instr->getType() == fi_type_e::fiFix)
			{
				// clear the fixpoint
				static_cast<FixpointInstruction*>(instr)->clear();
			}
		}
	}


	/**
	 * @brief  The main execution loop
	 *
	 * This method is the main execution loop for the symbolic execution. It
	 * assumes that the microcode is already compiled, etc.
	 */
	bool mainLoop()
	{
		if (FA_USE_PREDICATE_ABSTRACTION)
		{
			FA_DEBUG_AT(1, "Running the analysis with the folowing predicates:");
			for (AbstractInstruction* instr : this->GetAssembly().code_)
			{
				if (fi_type_e::fiFix == instr->getType())
				{
					FI_abs* absInstr = dynamic_cast<FI_abs*>(instr);
					if (nullptr != absInstr)
					{
						// TODO add a new predicate to other abstract instruction
						if (newPredicates_)
						{
							absInstr->addPredicate(predicates_);
						}

						FA_DEBUG_AT(1, "Number of predicates " << absInstr->getPredicates().size() << " of " << *absInstr->insn());
						for (const std::shared_ptr<const TreeAut>& pred : absInstr->getPredicates())
						{
							std::ostringstream os;
							os << "\n---------------------------------------------------\n"
								<< *absInstr << absInstr->insn();

							const CodeStorage::Insn* clInsn = absInstr->insn();
							assert(nullptr != clInsn);
							assert(nullptr != clInsn->bb);
							if (clInsn->bb->front() == clInsn)
							{
								os << " (" << clInsn->bb->name() << ")";
							}

							os << ": " << *absInstr->insn() << "\n" << *pred;

							FA_DEBUG_AT(1, os.str());
						}
					}
				}
			}
			newPredicates_ = false;
			FA_DEBUG_AT(1, "\n---------------------END---------------------------");
		}

		FA_DEBUG_AT(2, "creating empty heap ...");

		// create an empty heap
		std::shared_ptr<FAE> fae = std::shared_ptr<FAE>(
			new FAE(ta_, boxMan_));

		FA_DEBUG_AT(2, "scheduling initial state ...");

		// schedule the initial state for processing
		execMan_.init(
			DataArray(assembly_.regFileSize_, Data::createUndef()),
			fae,
			assembly_.code_.front()
		);

		SymState* state = nullptr;

		try
		{	// expecting problems...
			while (nullptr != (state = execMan_.dequeueDFS()))
			{	// process all states in the DFS order
				assert(nullptr != state);

				const CodeStorage::Insn* insn = state->GetInstr()->insn();
				if (nullptr != insn)
				{	// in case current instruction IS an instruction
					FA_DEBUG_AT(2, SSD_INLINE_COLOR(C_LIGHT_RED, insn->loc << *insn));
					FA_DEBUG_AT(2, *state);
				}
				else
				{
					FA_DEBUG_AT(3, *state);
				}

				if (testAndClearUserRequestFlag())
				{
					FA_NOTE("Executed " << std::setw(7) << execMan_.statesEvaluated()
						<< " states and " << std::setw(7) << execMan_.pathsEvaluated()
						<< " paths so far.");
				}

				// run the state
				execMan_.execute(*state);
			}

			return true;
		}
		catch (ProgramError& e)
		{
			assert(nullptr != e.state());

			const CodeStorage::Insn* insn = e.state()->GetInstr()->insn();
			if (nullptr != insn)
			{
				FA_DEBUG_AT(2, std::endl << *(e.state()->GetFAE()));
			}

			if (conf_.printTrace)
			{
				FA_LOG_MSG(e.location(), "Printing trace");
				std::ostringstream oss;
				printTrace(oss, e.state()->getTrace());
				if (conf_.traceFile.length() == 0)
				{
					Streams::trace(oss.str().c_str());
				}
				else
				{
					Streams::traceFile(oss.str().c_str(), conf_.traceFile.c_str());
				}
			}

			if (conf_.printSVTrace)
			{
				FA_LOG_MSG(e.location(), "Printing SV-Comp trace");
				
				// preprocess trace
				std::vector<const CodeStorage::Insn*> trace;
				for (const auto& state : e.state()->getTrace())
				{
					const CodeStorage::Insn *s = state->GetInstr()->insn();
					if (s != NULL)
					{
						trace.insert(trace.begin()+0,s);
					}
				}

				// prepare output
				std::streambuf* buf;
				std::ofstream of;
				if (conf_.traceFile.length() > 0)
				{
					of.open(conf_.traceFile, std::ofstream::out);
					buf = of.rdbuf();
				}
				else
				{
					buf = std::cerr.rdbuf();
				}
				std::ostream out(buf);

				// get file name from a instruction. it is not neccessary to read it
				// from the first instruction but it is needed to read the filename
				//const char* filename = (trace.size() > 0) ? trace[0]->loc.file : NULL;
				
				SVTraceLite svPrinter;
				svPrinter.printTrace(trace, out);
				
				if (conf_.traceFile.length() > 0)
				{
					of.close();
				}
			}

			if (conf_.printUcodeTrace)
			{
				FA_LOG_MSG(e.location(), "Printing microcode trace");

				std::ostringstream oss;
				printUcodeTrace(oss, e.state()->getTrace());
				Streams::traceUcode(oss.str().c_str());
			}

			if (FA_BACKWARD_RUN && !FA_USE_PREDICATE_ABSTRACTION)
			{
				FA_LOG("Executing backward run...");

				// check whether the counterexample is spurious and in case it is collect
				// some perhaps helpful information (failpoint and predicate)
				BackwardRun bwdRun(execMan_);
				SymState::Trace trace = e.state()->getTrace();
				SymState* failPoint = nullptr;
				std::vector<std::shared_ptr<const TreeAut>> predicate;

				bool isSpurious = bwdRun.isSpuriousCE(trace, failPoint, predicate);
				if (isSpurious)
				{
					FA_NOTE("Is spurious");
				}
				else
				{
					FA_NOTE("Is real");
					if (nullptr != insn)
						FA_NOTE_MSG(&insn->loc, SSD_INLINE_COLOR(C_LIGHT_RED, *insn));
					if (nullptr != e.location())
						FA_ERROR_MSG(e.location(), e.what());
					else
						reportErrorNoLocation(e.what());
				}
				throw;
			}

			if (FA_USE_PREDICATE_ABSTRACTION)
			{	// in case we are using predicate abstraction
				FA_LOG("Executing backward run...");

				// check whether the counterexample is spurious and in case it is collect
				// some perhaps helpful information (failpoint and predicate)
				BackwardRun bwdRun(execMan_);
				SymState::Trace trace = e.state()->getTrace();
				SymState* failPoint = nullptr;
				predicates_.clear();

				bool isSpurious = bwdRun.isSpuriousCE(trace, failPoint, predicates_);
				if (isSpurious)
				{
					assert(!predicates_.empty());
					assert(nullptr != failPoint);
					assert(nullptr != failPoint->GetInstr());

					FA_DEBUG_AT(1,"The counterexample IS (PROBABLY) spurious");

					FA_DEBUG_AT(1,"Failing instruction: " << *failPoint->GetInstr());
					for (const auto& p : predicates_)
					{
						FA_DEBUG_AT(1,"Learnt predicate: " << *p);
					}

					// now, we add 'predicate' to the set of predicates that are used for
					// abstraction at failPoint (which should BTW be abstraction)

					FI_abs* absInstr = dynamic_cast<FI_abs*>(failPoint->GetInstr());
					if (nullptr == absInstr)
					{
						assert(false);
					}

					// set the new predicate for abstraction
					//absInstr->addPredicate(predicate);

					newPredicates_ = true;

					clearFixpoints();

					return false;
				}
				else
				{	// if the counterexample is not spurious
					FA_NOTE("The counterexample IS real");
					if (nullptr != insn)
						FA_NOTE_MSG(&insn->loc, SSD_INLINE_COLOR(C_LIGHT_RED, *insn));
					if (nullptr != e.location())
						FA_ERROR_MSG(e.location(), e.what());
					else
						reportErrorNoLocation(e.what());

					throw;
				}
			}
			else
			{	// in case we are using finite height abstraction
				if (nullptr != insn)
					FA_NOTE_MSG(&insn->loc, SSD_INLINE_COLOR(C_LIGHT_RED, *insn));
				if (nullptr != e.location())
					FA_ERROR_MSG(e.location(), e.what());
				else
					reportErrorNoLocation(e.what());
				throw;
			}
		}
		catch (RestartRequest& e)
		{	// in case a restart is requested, clear all fixpoint computation points
			clearFixpoints();

			FA_DEBUG_AT(2, e.what());

			return false;
		}
	}

public:   // methods

	/**
	 * @brief  The default constructor
	 *
	 * The default constructor.
	 */
	explicit Engine(const ProgramConfig& conf) :
		ta_{},
		fixpoint_{},
		boxMan_{},
		compiler_(fixpoint_, ta_, boxMan_),
		assembly_{},
		execMan_{},
		conf_(conf),
		dbgFlag_{false},
		userRequestFlag_{false},
		newPredicates_{false},
		predicates_()
	{ }

	/**
	 * @brief  Loads types from a storage
	 *
	 * This method loads data types and function stackframes from the provided
	 * storage.
	 *
	 * @param[in]  stor  The code storage containing types
	 */
	void loadTypes(const CodeStorage::Storage& stor)
	{
		FA_DEBUG_AT(3, "loading types ...");

		// clear the box manager
		boxMan_.clear();

		// ************ infer data types' layouts ************
		for (const cl_type* type : stor.types)
		{	// for each data type in the storage
			std::vector<size_t> v;
			std::string name;
			if (type->name)
			{	// in case the structure has a name
				name = std::string(type->name);
			}
			else
			{	// in case the structure is nameless
				std::ostringstream ss;
				ss << type->uid;
				name = ss.str();
			}
			FA_DEBUG_AT(3, name);

			switch (type->code)
			{
				case cl_type_e::CL_TYPE_STRUCT: // for a structure
					NodeBuilder::buildNode(v, type);
					break;

				default: // for other types
					v.push_back(0);
					break;
			}

			boxMan_.createTypeInfo(name, v);
		}

		// ************ infer functions' stackframes ************
		for (const CodeStorage::Fnc* fnc : stor.fncs)
		{	// for each function in the storage, create a data structure representing
			// its stackframe
			std::vector<size_t> v;

			const SymCtx ctx(*fnc);
			for (const SelData& sel : ctx.GetStackFrameLayout())
			{	// create the stackframe
				v.push_back(sel.offset);
			}

			std::ostringstream ss;
			ss << "__@" << nameOf(*fnc) << ':' << uidOf(*fnc);

			FA_DEBUG_AT(3, ss.str());

			boxMan_.createTypeInfo(ss.str(), v);
		}

		// ************ compile layout of the block of global vars ************
		std::vector<const cl_type*> components;
		for (const CodeStorage::Var& var : stor.vars)
		{
			if (CodeStorage::EVar::VAR_GL == var.code)
			{
				components.push_back(var.type);
			}
		}

		std::vector<size_t> v;
		if (!components.empty())
		{ // in case the are some global variables
			NodeBuilder::buildNodes(v, components, 0);
		}
		else
		{	// in case there are no global variables, make one fake
			v.push_back(0);
		}

		boxMan_.createTypeInfo(GLOBAL_VARS_BLOCK_STR, v);
		FA_DEBUG_AT(1, "created box for global variables: "
			<< *boxMan_.getTypeInfo(GLOBAL_VARS_BLOCK_STR));
	}

#if 0
	void loadBoxes(const std::unordered_map<std::string, std::string>& db) {

		FA_DEBUG_AT(2, "loading boxes ...");

		for (auto p : db) {

			this->boxes.push_back((const Box*)boxMan_.loadBox(p.first, db));

			FA_DEBUG(p.first << ':' << std::endl << *(const FA*)this->boxes.back());

		}

		boxMan_.buildBoxHierarchy(this->hierarchy, this->basicBoxes);

	}
#endif

	void compile(const CodeStorage::Storage& stor, const CodeStorage::Fnc& entry)
	{
		compiler_.compile(assembly_, stor, entry);
	}

	const Compiler::Assembly& GetAssembly() const
	{
		return assembly_;
	}

	void run()
	{
		// Assertions
		assert(assembly_.code_.size());

		try
		{	// expect problems...
			while (!this->mainLoop())
			{	// while the analysis hasn't terminated
				FA_NOTE("Restarting the analysis...");
			}

			FA_NOTE("The program is SAFE.");

			// print out boxes
			this->printBoxes();

			for (auto instr : assembly_.code_)
			{	// print out all fixpoints
				if (instr->getType() != fi_type_e::fiFix)
				{
					continue;
				}

				if (instr->insn())
				{
					FA_DEBUG_AT(1, "fixpoint at " << instr->insn()->loc << std::endl
						<< (static_cast<FixpointInstruction*>(instr))->getFixPoint());
				} else
				{
					FA_DEBUG_AT(1, "fixpoint at unknown location" << std::endl
						<< (static_cast<FixpointInstruction*>(instr))->getFixPoint());
				}
			}

			// print out stats
			FA_DEBUG_AT(1, "forester has generated " << execMan_.statesEvaluated()
				<< " symbolic configuration(s) in " << execMan_.pathsEvaluated()
				<< " path(s) using " << boxMan_.boxDatabase().size() << " box(es)");
		}
		catch (const ProgramError& e)
		{ }
		catch (std::exception& e)
		{
			FA_DEBUG(e.what());

			this->printBoxes();

			throw;
		}
	}

	void run(const Compiler::Assembly& assembly)
	{
		assembly_ = assembly;

		try
		{
			this->run();
			assembly_.code_.clear();
		}
		catch (...)
		{
			assembly_.code_.clear();

			throw;
		}
	}

	void setDbgFlag()
	{
		dbgFlag_ = true;
	}

	void setUserRequestFlag()
	{
		userRequestFlag_ = true;
	}

	bool testAndClearUserRequestFlag()
	{
		bool oldValue = userRequestFlag_;
		userRequestFlag_ = false;
		return oldValue;
	}
};

SymExec::SymExec(const ProgramConfig& conf) :
	engine{new Engine(conf)}
{ }

SymExec::~SymExec()
{
	// Assertions
	assert(engine != nullptr);

	delete this->engine;
}

void SymExec::loadTypes(const CodeStorage::Storage& stor)
{
	// Assertions
	assert(engine != nullptr);

	this->engine->loadTypes(stor);
}

#if 0
void SymExec::loadBoxes(const std::unordered_map<std::string, std::string>& db) {
	this->engine->loadBoxes(db);
}
#endif

const Compiler::Assembly& SymExec::GetAssembly() const
{
	// Assertions
	assert(nullptr != engine);

	return this->engine->GetAssembly();
}

void SymExec::compile(const CodeStorage::Storage& stor,
	const CodeStorage::Fnc& main)
{
	// Assertions
	assert(engine != nullptr);

	this->engine->compile(stor, main);
}

void SymExec::run()
{
	// Assertions
	assert(engine != nullptr);

	this->engine->run();
}

void SymExec::run(const Compiler::Assembly& assembly)
{
	// Assertions
	assert(engine != nullptr);

	this->engine->run(assembly);
}

void SymExec::setDbgFlag()
{
	// Assertions
	assert(engine != nullptr);

	this->engine->setDbgFlag();
}

void SymExec::setUserRequestFlag()
{
	// Assertions
	assert(engine != nullptr);

	this->engine->setUserRequestFlag();
}
