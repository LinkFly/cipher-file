#include "parse-args.h"
#include "Cipher.h"

#include <iostream>

using namespace std;

int main(int argc, char** argv)
{
    ParsedArgs args = parseArgs(argc, argv);
    Cipher cipher(args.output, args.key);
	if(args.cipher == ECipher::cipher)
	    cipher.cipherFile();
	else if (args.cipher == ECipher::uncipher)
	    cipher.uncipherFile();
}
