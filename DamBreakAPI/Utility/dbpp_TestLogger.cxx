//	 Description: functions to output an error log
#include <string.h>
// Elligno include
#include "dbpp_TestLogger.h"

namespace dbpp 
{

	// _________________________________________________________
	//
	//   Initiates log. Don't call, it is in the constructor
	// NOTE: it would be nice to remove or replace function "printf"
	//
	bool Logger::Init( /*char * filename*/)
	{
		// Clear the log file contents
// 		if( ( logfile = fopen( filename, "wb")) == NULL)
// 			return false;

		char NomFichierDEBUG[256];

		// initializing file creation 
		::strcpy_s( NomFichierDEBUG, 256, "DBppGuiLogger.txt");

		// Open for write 
		errno_t err = fopen_s( &logfile, NomFichierDEBUG, "wb" );
		if( err == 0 ) // success
		{
			printf( "The logger file 'DBppGuiLogger.txt' was opened\n");
			m_opened=true;
			// Close the file, return success
			fclose( logfile);
			return m_opened;
		}
		else
		{
			printf( "The logger file 'DBppGuiLogger.txt' was not opened\n");
			m_opened = false;
			return m_opened;
		}
	}

	// _________________________________________________________
	//
	//  Shuts down log, in the destructor.
	bool Logger::Shutdown()
	{
		if( logfile)
			fclose( logfile);

		return true;
	}

	// _________________________________________________________
	//
	// output newline
	void Logger::OutputNewline()
	{
		// Open the file for append
// 		if( ( logfile = fopen( "MessageLogger.txt", "a+")) == NULL)
// 			return;
		
		char NomFichierDEBUG[256];
		// initializing file creation 
		::strcpy_s( NomFichierDEBUG, 256, "MessageLogger.txt");

		// Open for write 
		errno_t err = fopen_s( &logfile, NomFichierDEBUG, "a+" );
		if( err == 0 ) // success
		{
//			printf( "The logger file 'MessageLogger.txt' was opened\n");

			// Write the newline
			putc( '\n', logfile);

			// Close the file
			fclose( logfile);
		}
		else
		{
//			printf( "The logger file 'MessageLogger.txt' was not opened\n");
			return;
		}
	}

	// _________________________________________________________
	//
	void Logger::OutputError( char * text,...)
	{
		va_list arg_list;

		// Initialize variable argument list
		va_start( arg_list, text);

		// Open the file for append
// 		if((logfile=fopen("MessageLogger.txt", "a+"))==NULL)
// 			return;
		char NomFichierDEBUG[256];
		// initializing file creation 
		::strcpy_s( NomFichierDEBUG, 256, "MessageLogger.txt");

		// Open for write 
		errno_t err = fopen_s( &logfile, NomFichierDEBUG, "a+" );
		if( err == 0 ) // success
		{
			// Write the text
			fprintf(logfile, "<!> ");
			vfprintf(logfile, text, arg_list);
			putc('\n', logfile);

			// Also write to the console window
			printf("<!> ");
			vprintf(text, arg_list);
			printf("\n");

			// Close the file
			fclose( logfile);
			va_end( arg_list);
		}
		else
		{
			return;
		}
	}

	// _________________________________________________________
	//
	void Logger::OutputSuccess( char * text,...)
	{
		va_list arg_list;

		// Initialize variable argument list
		va_start(arg_list, text);

		// Open the file for append
// 		if((logfile=fopen("MessageLogger.txt", "a+"))==NULL)
// 			return;
		char NomFichierDEBUG[256];
		// initializing file creation 
		::strcpy_s( NomFichierDEBUG, 256, "MessageLogger.txt");

		// Open for write 
		errno_t err = fopen_s( &logfile, NomFichierDEBUG, "a+" );
		if( err == 0 ) // success
		{
			// Write the text
			fprintf(logfile, "<-> ");
			vfprintf(logfile, text, arg_list);
			putc('\n', logfile);

			// Close the file
			fclose(logfile);
			va_end(arg_list);
		}
		else
		{
			return;
		}
	}
} // End of namespace
 