/**
* Name:    Francesco
* Surname: Longo
* ID:      223428
* Lab:     8
* Ex:      2
*
* Familiarize with the UNICODE encoding system, binary files, and
*       serialized read/write operation.
*       Preliminary step for exercise 03.
*
* An ASCII file has lines with the following format:
* - An identifier, i.e., an integer value varying from 1 to the number
*   of rows in the file (e.g., 1, 2, etc.)
* - A register number, i.e., a long integer of 6 digits (e.g., 164678)
* - A surname, i.e., a string of maximum 30 characters (e.g., Rossi)
* - A name, i.e., a string of maximum 30 characters (e.g., Mario)
* - An examination mark, i.e., an integer value.
* 
* The following is a correct example of such a file:
* 
* 1 100000 Romano Antonio 25
* 2 150000 Fabrizi Aldo 22
* 3 200000 Verdi Giacomo 15
* 4 250000 Rossi Luigi 30
* 
* Write a C program in the MS Visual Studio environment which is able
* to:
* - Read such a file and create a new file with the exact same content
*  but stored in *binary* format and with *fixed-length* records
*   (integer + long integer + 30 characters + 30 characters + integer).
* - To verify the previous step (i.e., to be sure the binary file has
*  been stored in a proper way) re-read the binary file (the one
*  just written) and write its content on standard output.
* Input and output file names are passed on the command line.
*
*
* Suggestions
* -----------
*
* - Read the input file on a field by field basis, using the _ftscanf
*   construct (see the tchar.h file)
* - Store fields within a proper C data structure ("struct student" with
*   fields for the id, register number, etc.)
* - Store the structure onto the output file as a unique object
*   using the WriteFile function.
* - Read it back using ReadFile ...
**/

// Remove security warning for function
#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#ifndef UNICODE
#define UNICODE
#define _UNICODE
#endif // !UNICODE

#include <Windows.h>
#include <tchar.h>
#include <stdio.h>

#define STR_MAX_L 30+1	// extra 1 for \0
#define SECONDS 10

int Return(int value);
LPWSTR getErrorMessageAsString(DWORD errorCode);

typedef struct _student {
	INT id;
	DWORD regNum;
	TCHAR surname[STR_MAX_L];
	TCHAR name[STR_MAX_L];
	INT mark;
} STUDENT;

INT _tmain(INT argc, LPTSTR argv[]) {
	// Variables
	FILE *fInP;
	HANDLE hOut;
	DWORD nOut;
	STUDENT s;

	// Check for arguments passed from command line (file_in and file_out)
	if (argc != 3) {
		_ftprintf(stderr, _T("Usage: %s <file_in> <file_out>\n"), argv[0]);
		return Return(1);
	}

	// Print parameters retrieved for DEBUG
	printf("I have %d parameters:\n", argc);
	for (int i = 0; i < argc; i++) {
		_ftprintf(stdout, _T(" - param %d is %s\n"), i, argv[i]);
	}

	// Open file for read and error check -> fInP is the pointer to open file
	if ( (fInP = _tfopen(argv[1], _T("rt"))) == NULL) {
		_ftprintf(stderr, _T("Cannot open input file %s: \n\t--> %s\n"), argv[1], getErrorMessageAsString(GetLastError()));
		return Return(2);
	}

	// Create output file and get the handle of the open FILE
	hOut = CreateFile(argv[2], 
		GENERIC_WRITE, 
		0, 
		NULL, 
		CREATE_ALWAYS, 
		FILE_ATTRIBUTE_NORMAL, 
		NULL);

	// check if the HANDLE is correct
	if (hOut == INVALID_HANDLE_VALUE) {
		_ftprintf(stderr, _T("Cannot open output file %s: \n\t--> %s\n"), argv[2], getErrorMessageAsString(GetLastError()));
		fclose(fInP);
		return Return(3);
	}

	// parsing dei parametri in input (5 parametri per volta: INT - LONG INT - CHAR[] - CHAR[] - INT)
	while (_ftscanf(fInP, _T("%d %ld %s %s %d"), &s.id, &s.regNum, s.surname, s.name, &s.mark) == 5) {
		// WriteFile writes in binary, write all the read structure on the output file
		WriteFile(hOut, &s, sizeof(s), &nOut, NULL);
		// Check if what I wrote is the same size of the entire struct
		if (nOut != sizeof(s)) {
			_ftprintf(stderr, _T("Cannot write correctly the output: \n\t--> %s\n"), getErrorMessageAsString(GetLastError()));
			fclose(fInP);
			CloseHandle(hOut);
			return Return(4);
		}
	}

	// End of the copy (in binary)
	_tprintf(_T("Correctly written the output file\n"));

	// Close file
	fclose(fInP);
	CloseHandle(hOut);

	// Second step
	_tprintf(_T("Now reading it back:\n"));

	HANDLE hIn2;
	DWORD nIn2;

	// Open file created before with CreateFile
	hIn2 = CreateFile(argv[2],
		GENERIC_READ,
		FILE_SHARE_READ,
		0,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);

	// Error check for the open
	if (hIn2 == INVALID_HANDLE_VALUE) {
		_ftprintf(stderr, _T("Cannot open input file %s: \n\t--> %s\n"), argv[2], getErrorMessageAsString(GetLastError()));
		return Return(5);
	}

	// Read the file from binary and print in ASCII to check if it was copyed correctly
	while (ReadFile(hIn2, &s, sizeof(s), &nIn2, NULL) && nIn2 > 0) {
		// If what I read is different from the size of a struct: error
		if (nIn2 != sizeof(s)) {
			_ftprintf(stderr, _T("Error reading, file shorter than expected.\n"));
		}

		// Print what I read
		_ftprintf(stdout, _T("id: %d\treg_number: %ld\tsurname: %10s\tname: %10s\tmark: %d\n"), s.id, s.regNum, s.surname, s.name, s.mark);
	}

	// Close HANDLE
	CloseHandle(hIn2);

	// Return with a sleep
	return Return(0);
}

int Return(int value) {
	Sleep(SECONDS * 1000);
	return value;
}

LPWSTR getErrorMessageAsString(DWORD errorCode) {
	LPWSTR errString = NULL;

	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		0,
		errorCode,
		0,
		(LPWSTR)&errString,
		0,
		0);

	return errString;
}
