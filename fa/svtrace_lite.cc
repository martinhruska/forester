#include "svtrace_lite.hh"

// code listener headers
#include <cl/storage.hh>

// std headers
#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <exception>
#include <assert.h>
#include <bits/unordered_map.h>

namespace
{

	// XML initialization
	const std::string START       = "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\n\
		<graphml xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns=\"http://graphml.graphdrawing.org/xmlns\">\n\
		<key attr.name=\"nodeType\" attr.type=\"string\" for=\"node\" id=\"nodetype\">\n\
		<default>path</default>\n\
		</key>\n\
		<key attr.name=\"originFileName\" attr.type=\"string\" for=\"edge\" id=\"originfile\">\n\
		<default>filedefault</default>\n\
		</key>\n\
		<key attr.name=\"isFrontierNode\" attr.type=\"boolean\" for=\"node\" id=\"frontier\">\n\
		<default>false</default>\n\
		</key>\n\
		<key attr.name=\"isViolationNode\" attr.type=\"boolean\" for=\"node\" id=\"violation\">\n\
		<default>false</default>\n\
		</key>\n\
		<key attr.name=\"isEntryNode\" attr.type=\"boolean\" for=\"node\" id=\"entry\">\n\
		<default>false</default>\n\
		</key>\n\
		<key attr.name=\"isSinkNode\" attr.type=\"boolean\" for=\"node\" id=\"sink\">\n\
		<default>false</default>\n\
		</key>\n\
		<key attr.name=\"invariant\" attr.type=\"string\" for=\"node\" id=\"invariant\"/>\n\
		<key attr.name=\"automaton\" attr.type=\"string\" for=\"node\" id=\"automaton\"/>\n\
		<key attr.name=\"invariant.scope\" attr.type=\"string\" for=\"node\" id=\"invariant.scope\"/>\n\
		<key attr.name=\"namedValue\" attr.type=\"string\" for=\"node\" id=\"named\"/>\n\
		<key attr.name=\"violatedProperty\" attr.type=\"string\" for=\"node\" id=\"violatedProperty\"/>\n\
		<key attr.name=\"sourcecodeLanguage\" attr.type=\"string\" for=\"graph\" id=\"sourcecodelang\"/>\n\
		<key attr.name=\"programFile\" attr.type=\"string\" for=\"graph\" id=\"programfile\"/>\n\
		<key attr.name=\"specification\" attr.type=\"string\" for=\"graph\" id=\"specification\"/>\n\
		<key attr.name=\"memoryModel\" attr.type=\"string\" for=\"graph\" id=\"memorymodel\"/>\n\
		<key attr.name=\"architecture\" attr.type=\"string\" for=\"graph\" id=\"architecture\"/>\n\
		<key attr.name=\"producer\" attr.type=\"string\" for=\"graph\" id=\"producer\"/>\n\
		<key attr.name=\"sourcecode\" attr.type=\"string\" for=\"edge\" id=\"sourcecode\"/>\n\
		<key attr.name=\"startline\" attr.type=\"int\" for=\"edge\" id=\"startline\"/>\n\
		<key attr.name=\"startoffset\" attr.type=\"int\" for=\"edge\" id=\"startoffset\"/>\n\
		<key attr.name=\"lineColSet\" attr.type=\"string\" for=\"edge\" id=\"lineCols\"/>\n\
		<key attr.name=\"control\" attr.type=\"string\" for=\"edge\" id=\"control\"/>\n\
		<key attr.name=\"assumption\" attr.type=\"string\" for=\"edge\" id=\"assumption\"/>\n\
		<key attr.name=\"assumption.scope\" attr.type=\"string\" for=\"edge\" id=\"assumption.scope\"/>\n\
		<key attr.name=\"enterFunction\" attr.type=\"string\" for=\"edge\" id=\"enterFunction\"/>\n\
		<key attr.name=\"returnFromFunction\" attr.type=\"string\" for=\"edge\" id=\"returnFrom\"/>\n\
		<key attr.name=\"predecessor\" attr.type=\"string\" for=\"edge\" id=\"predecessor\"/>\n\
		<key attr.name=\"successor\" attr.type=\"string\" for=\"edge\" id=\"successor\"/>\n\
		<key attr.name=\"graphtype\" attr.type=\"string\" for=\"graph\" id=\"type\"/>\n\
		<key attr.name=\"programhash\" attr.type=\"string\" for=\"graph\" id=\"programhash\"/>\n\
		<key attr.name=\"witness-type\" attr.type=\"string\" for=\"graph\" id=\"witness-type\"/>\n\
		<graph edgedefault=\"directed\">\n\
			<data key=\"sourcecodelang\">C</data>\n\
			<data key=\"memorymodel\">precise</data>\n\
			<data key=\"architecture\">32b</data>\n\
			<data key=\"specification\"><__SPECIFICATION__></data>\n\
			<data key=\"programhash\"><__PROGRAMHASH__></data>\n\
			<data key=\"producer\">Forester</data>\n";


	const std::string END         = "</graph>\n</graphml>";
	const std::string DATA_START  = "<data key=";
	const std::string DATA_END    = "</data>\n";
	const std::string NODE_START  = "<node id=";
	const std::string NODE_END    = "</node>";
	const std::string NODE_ENTRY  = "<data key=\"entry\">true</data>";
	const std::string NODE_NAME   = "A";
	const std::string NODE_AUTOMATON  = "<data key=\"automaton\">";
	const std::string VIOLATION   = "<data key=\"violation\">true";
	const std::string EDGE_START  = "<edge source=";
	const std::string EDGE_END    = "</edge>";
	const std::string EDGE_TARGET = " target=";
    const std::string WITNESS_TYPE = "<data key=\"witness-type\">";
	const std::string PROGRAMFILE  = "<data key=\"programfile\">";
	const std::string CORRECTNESS_WITNESS = "correctness_witness";
	const std::string VIOLATION_WITNESS  = "violation_witness";



	bool instrsEq(
			const CodeStorage::Insn*                       instr1,
			const CodeStorage::Insn*                       instr2)
	{
		return instr1->loc.line == instr2->loc.line &&
			instr1->loc.column >= instr2->loc.column && // TODO: remove this if needed
			instr1->code == instr2->code;
	}


	void printStart(
			std::ostream&        out,
            const std::string&   filename,
			const bool           violationWitness=true)
	{
		out << START;
		out << "\t\t\t" << WITNESS_TYPE <<
			(violationWitness ? VIOLATION_WITNESS : CORRECTNESS_WITNESS) << DATA_END;
		out  << "\t\t\t" << PROGRAMFILE << filename << DATA_END << "\n";
	}


	void printEnd(
			std::ostream&                                  out,
			int                                            endNode,
            const bool                                     isViolation=true)
	{
		out << "\t" << NODE_START << "\"" << NODE_NAME << endNode << "\">\n";
        if (isViolation)
		    out << "\t" << VIOLATION << DATA_END;
		out << "\t" << NODE_END << "\n";
		out << "\t" << END;
	}

	void printEdge(
			std::ostream&                                  out,
			const int                                      lineNumber,
			const int                                      nodeNumber)
	{
		const std::string indent("\t\t");
		out << "\t" << EDGE_START << "\"" << NODE_NAME << nodeNumber << "\"" <<
			EDGE_TARGET << "\""<<  NODE_NAME << nodeNumber+1 << "\">\n";

		out << indent << DATA_START << "\"startline\">"<< lineNumber << DATA_END;
		out << "\t" << EDGE_END << "\n";

	}

	void printNode(
			std::ostream&                                  out,
			const int                                      nodeNumber)
	{
		if (nodeNumber == 1)
		{
			out << "\t" << NODE_START << "\"" << NODE_NAME << nodeNumber << "\">\n\t"
				<< NODE_ENTRY << "\n\t" << NODE_END <<"\n";
		}
		else
		{
			out << "\t" << NODE_START << "\"" << NODE_NAME << nodeNumber << "\"/>\n";
		}
	}

	void printNode(
			std::ostream&                                  out,
			const int                                      nodeNumber,
			const TreeAut&                                 invariant)
	{
		if (nodeNumber == 1)
		{
			out << "\t" << NODE_START << "\"" << NODE_NAME << nodeNumber << "\">\n\t"
				<< NODE_ENTRY << "\n\t"
				<< NODE_AUTOMATON << invariant << DATA_END << "\n\t"
				<< NODE_END <<"\n";
		}
		else
		{
			out << "\t" << NODE_START << "\"" << NODE_NAME << nodeNumber << "\">\n\t"
				<< NODE_AUTOMATON << invariant << DATA_END << "\n\t"
				<< NODE_END <<"\n";
		}
	}
}


void SVTraceLite::printTrace(
			const std::vector<const CodeStorage::Insn*>&   instrs,
			std::ostream&                                  out,
			const bool                                     violationWitness)
{
	const std::string filename = instrs.size() ? instrs.front()->loc.file : "";
	printStart(out, filename, violationWitness);

	for (size_t i = 0; i < instrs.size(); ++i)
	{
		const auto instr = instrs.at(i);
		const int lineNumber = instr->loc.line;
		
		if (i+1 < instrs.size() && instrsEq(instr, instrs.at(i+1)))
		{
			continue;
		}

		printNode(out, nodeNumber_);
		printEdge(out, lineNumber, nodeNumber_);
	
		++nodeNumber_;
	}
	
	printEnd(out, nodeNumber_);
}

void SVTraceLite::printCorrectnessTrace(
		const std::string&   filename,
		std::ostream&        out,
		const std::map<int, const TreeAut*>& lineToFixpoint)
{
	printStart(out, filename, false);

	for (const auto& i : lineToFixpoint)
	{
		printNode(out, nodeNumber_, *i.second);
		printEdge(out, i.first, nodeNumber_);
		++nodeNumber_;
	}

	printEnd(out, nodeNumber_, false);
}
