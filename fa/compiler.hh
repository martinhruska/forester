/*
 * Copyright (C) 2011 Jiri Simacek
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


/**
 * @file compiler.hh
 *
 * Header file for the compiler of microinstructions.
 */


#ifndef COMPILER_H
#define COMPILER_H

// Standard library headers
#include <iomanip>
#include <iostream>
#include <sstream>
#include <ostream>
#include <unordered_map>
#include <vector>

// Code Listener headers
#include <cl/storage.hh>
#include <cl/cldebug.hh>
#include <cl/clutil.hh>

// Forester headers
#include "abstractinstruction.hh"
#include "treeaut_label.hh"


namespace CodeStorage {
	struct Fnc;
	struct Storage;
	struct Insn;
}

std::ostream& operator<<(std::ostream& os, const cl_loc& loc);


/**
 * @brief  The compiler class
 *
 * Class that represents the compiler from CodeListener's code storage to
 * Forester's microinstructions.
 */
class Compiler
{
public:

	/**
	 * @brief  The structure that contains the assembly code
	 *
	 * This structure contains the compiled assembly code (in microinstructions)
	 * of the program.
	 *
	 * @todo: use something more robust than a struct
	 */
	struct Assembly
	{
		/// type for the code
		typedef std::vector<AbstractInstruction*> CodeList;

		/// linear code
		CodeList code_;

		/// index with pointers to functions' entry points in the code
		std::unordered_map<const struct CodeStorage::Fnc*, AbstractInstruction*>
			functionIndex_;

		/// size of the register file
		size_t regFileSize_;


		/**
		 * @brief  Default constructor
		 */
		Assembly() :
			code_{},
			functionIndex_{},
			regFileSize_{}
		{ }


		/**
		 * @brief  The destructor
		 */
		~Assembly() { clear(); }


		/**
		 * @brief  Clears the assembly
		 */
		void clear()
		{
			for (AbstractInstruction* instr : code_)
			{
				delete instr;
			}

			code_.clear();
			functionIndex_.clear();
			regFileSize_ = 0;
		}


		/**
		 * @brief  Gets the entry point of a function
		 *
		 * Returns a pointer to the instruction that is the entry point of the given
		 * function @p f.
		 *
		 * @param[in]  f  The function @p f
		 *
		 * @returns  The instruction that is the entry point of @p f
		 */
		AbstractInstruction* getEntry(
			const CodeStorage::Fnc*          f) const
		{
			auto iter = functionIndex_.find(f);
			// Assertions
			assert(iter != functionIndex_.end());
			return iter->second;
		}


		/**
		 * @brief  Prints the microcode of the assembly
		 *
		 * Prints the microcode of the assembly.
		 *
		 * @param[in,out]  os    The output stream
		 * @param[in]      code  The code to be printed
		 *
		 * @returns  Modified output stream
		 */
		static std::ostream& printUcode(
			std::ostream&         os,
			const CodeList&       code);


		/**
		 * @brief  Transforms an instruction into a string
		 *
		 * This method takes a CodeListener instruction and transforms it into a @e
		 * nice string.
		 *
		 * @param[in]  clInsn  The CodeListener instruction to be transformed
		 *
		 * @returns  String with the instruction
		 */
		static std::string insnToString(
			const CodeStorage::Insn&          clInsn);


		/**
		 * @brief  Prints the original code of the program
		 *
		 * Prints the original code of the program as stored in the assembly.
		 *
		 * @param[in,out]  os     The output stream
		 * @param[in]      code   The code to be printed
		 *
		 * @returns  Modified output stream
		 */
		static std::ostream& printOrigCode(
			std::ostream&         os,
			const CodeList&       ucode);


		/**
		 * @brief  The output stream operator
		 *
		 * The std::ostream << operator for conversion to a string.
		 *
		 * @param[in,out]  os     The output stream
		 * @param[in]      as     The value to be appended to the stream
		 *
		 * @returns  The modified output stream
		 */
		friend std::ostream& operator<<(
			std::ostream&              os,
			const Assembly&            as)
		{
			return printUcode(os, as.code_);
		}
	};

private:  // methods

	Compiler(const Compiler&);

public:
	Compiler& operator=(const Compiler&);

	/**
	 * @brief  The constructor
	 *
	 * Constructs the compiler object with given backends for fixpoints, tree
	 * automata, and given box manager.
	 *
	 * @param[in]  fixpoint         The backend for fixpoints
	 * @param[in]  ta               The backend for tree automata
	 * @param[in]  boxMan           The box manager
	 */
	Compiler(
		TreeAut&         fixpoint,
		TreeAut&         ta,
		class BoxMan&             boxMan);

	/**
	 * @brief  The destructor
	 */
	~Compiler();

	/**
	 * @brief  Compiles code from the code storage
	 *
	 * @param[out]  assembly  The output assembly code
	 * @param[in]   stor      The code storage to be compiled
	 * @param[in]   entry     The entry point of the program
	 */
	void compile(
		Assembly&                     assembly,
		const CodeStorage::Storage&   stor,
		const CodeStorage::Fnc&       entry);


private:

	/**
	 * @brief  Class with the core implementation of the compiler (hidden)
	 */
	class Core;

	/// The core of the compiler
	Core *core_;
};

#endif
