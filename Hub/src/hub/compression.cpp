//<Elypso hub>
//    Copyright(C) < 2024 > < Greenlaser >
//
//    This program is free software : you can redistribute it and /or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License in LICENCE.md
//    and a copy of the EULA in EULA.md along with this program. 
//    If not, see < https://github.com/Lost-Empire-Entertainment/Elypso-engine 

//hub
#include "compression.hpp"

#include <cstdlib>
#include <iostream>

using std::cout;

bool Compression::CompressFolder(const string& inputPath, const string& outputPath)
{
	string quotedInputPath = "\"" + inputPath + "\"";
	string quotedOutputPath = "\"" + outputPath + "\"";
	string powershell1 = "powershell Compress-Archive -LiteralPath ";
	string powershell2 = " -DestinationPath ";
	string command = powershell1 + quotedInputPath + powershell2 + quotedOutputPath;

	cout << "hub compress:\n" << command << "\n\n";

	int result = system(command.c_str());
	return result == 0;
}

bool Compression::DecompressFile(const string& inputPath, const string& outputPath)
{
	string quotedInputPath = "\"" + inputPath + "\"";
	string quotedOutputPath = "\"" + outputPath + "\"";
	string powershell1 = "powershell Expand-Archive -Path ";
	string powershell2 = " -DestinationPath ";
	string command = powershell1 + quotedInputPath + powershell2 + quotedOutputPath;

	cout << "hub decompress:\n" << command << "\n\n";

	int result = system(command.c_str());
	return result == 0;
}