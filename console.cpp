#include "console.h"

#include <iostream>

#ifdef __WIN32

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#else
#include <unistd.h>
#endif


/**********************************************************************************************************************
 * Function: allocConsole
 *
 * Abstract: printing debugging information, status information is common and helpful...except if you are brain-dead 
 * idiots from Microsoft who disable this stuff.  So, lets fix this.  (taken from Adding Console I/O to a Win32 GUI 
 * App, Windows Developer Journal, Dec 1997; and updated from 
 * https://smacdo.com/programming/redirecting-standard-output-to-console-in-windows/)
 * 
 * Input   :
 *
 * Returns :
 *
 * Written : Apr 2025 (gkhuber)
 *********************************************************************************************************************/
void allocConsole()
#ifdef __WIN32                 // code for Windows...
{
    // Create the console window and set the window title.
    if (AllocConsole() == 0)
    {
        // Handle error here. Use ::GetLastError() to get the error.
    }

    // Redirect CRT standard input, output and error handles to the console window.
    FILE* pNewStdout = nullptr;
    FILE* pNewStderr = nullptr;
    FILE* pNewStdin = nullptr;

    ::freopen_s(&pNewStdout, "CONOUT$", "w", stdout);
    ::freopen_s(&pNewStderr, "CONOUT$", "w", stderr);
    ::freopen_s(&pNewStdin, "CONIN$", "r", stdin);

    // adjust the console mode to support colored output
    HANDLE  hStdOut= GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD   mode = 0;
    if (INVALID_HANDLE_VALUE != hStdOut)
    {
      GetConsoleMode(hStdOut, &mode);
      mode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
      SetConsoleMode(hStdOut, mode);
    }
    

    // Clear the error state for all of the C++ standard streams. Attempting to accessing the streams before they refer
    // to a valid target causes the stream to enter an error state. Clearing the error state will fix this problem,
    // which seems to occur in newer version of Visual Studio even when the console has not been read from or written
    // to yet.

    std::cout.clear();
    std::cerr.clear();
    std::cin.clear();

    std::wcout.clear();
    std::wcerr.clear();
    std::wcin.clear();
}
#else                              // code for Linux...wait there is none, Linux is not brain-dead
{

}
#endif

/**********************************************************************************************************************
 * Function:
 *
 * Abstract:
 *
 * Input   :
 *
 * Returns :
 *
 * Written : Apr 2025 (gkhuber)
 *********************************************************************************************************************/
void deallocConsole()
#ifdef __WIN32                    // code for Windows, clean up our mess
{
    FreeConsole();
}
#else                             // code for Linux...nothing to do
{

}
#endif

