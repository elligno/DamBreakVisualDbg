
#pragma once

// C++ includes
#include <iostream>
#include <string>
#include <cstring>

using namespace std;

namespace dbpp
{
	// description : this class contains static member functions for reading 
	//               arguments from the command line. 
	//               based on code by Roger Hansen <rogerha@ifi.uio.no>
	class CmdLineArgs
	{
		static int argc;
		static const char** argv;
		static char invalid[20];
	public:
		static void init(int argc_, const char** argv_)
		{
			// strcpy(invalid, "____invalid____");
			strcpy_s( invalid, 20, "____invalid____"); // added number of elements (J.B.)

			argc = argc_;
			argv = argv_;
		}

		// Read argument without value (switch)
		static bool hasSwitch(const char* arg)
		{
			const char* thisArg;   
			int tmp_argc = argc;
			const char** tmp_argv = argv;

			while ((thisArg = *++tmp_argv) && --tmp_argc) 
			{   /* Check all arguments */
				if (!strcmp(thisArg, arg) && tmp_argc > 0) 
					return  true; /* return if match and ++tmp_argv exist */
			}

			return false;
		}

		static double read(const char* arg, const double& def)
		{
			const char* pc = read(arg, invalid);

			if (strcmp(pc, invalid) == 0)
				return def;
			else
				return atof(pc);
		}

		static int read(const char* arg, const int& def)
		{
			const char* pc = read(arg, invalid);

			if (strcmp(pc, invalid) == 0)
				return def;
			else
				return atoi(pc);
		}

		static string read(const char* arg, const string& def)
		{
			const char* pc = read(arg, invalid);

			if (strcmp(pc, invalid) == 0)
				return def;
			else
				return pc;
		}

		// Read argument with value
		static const char* read(const char* arg, char* def)
		{
			const char* thisArg;   
			int tmp_argc = argc;
			const char** tmp_argv = argv;

			while ((thisArg = *++tmp_argv) && --tmp_argc) {  /* Check all arguments */
				if (!strcmp(thisArg, arg) && tmp_argc > 0) 
					return  *++tmp_argv; /* return if match and ++tmp_argv exist */
			}
			return def; /* No match. Return default value */
		}
	};
} // End of namespace