#pragma once
struct ItemMapping
{
	enum class MatchType
	{
		FullyQualified,				// System.Console::WriteLine(string)
		FunctionAndSignature,		// WriteLine(string)
		FunctionAndClass,			// System.Console::WriteLine
		FunctionOnly,				// WriteLine
		FullyQualifiedDerivesFrom,	// System.Data.IDbCommand::.ctor(System.Data.IDbConnection)
		DerivesFrom,				// System.Data.IDbCommand::Execute
		AllInModule,				// EVERYTHING in the specified Module
		AllInClass,					// EVERYTHING in the specified Class
		AllInAssembly				// EVERYTHING in the specified Assembly
	};

	enum class StringMatchMethod
	{
		BEGIN,
		CONTAINS,
		END
	};

	std::wstring FunctionName;
	std::wstring ClassName;
	std::wstring AssemblyName;
	std::wstring ModuleName;
	std::wstring DerivesFrom;
	std::wstring Signature;

	std::wstring HashString;

	StringMatchMethod Compare;
	MatchType Match;

	bool operator == (const std::wstring &strCompare);

	bool IgnoreGetters;
	bool IgnoreSetters;
};