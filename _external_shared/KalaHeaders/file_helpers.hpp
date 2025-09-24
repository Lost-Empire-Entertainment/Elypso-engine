//------------------------------------------------------------------------------
// file_helpers.hpp
//
// Copyright (C) 2025 Lost Empire Entertainment
//
// This is free source code, and you are welcome to redistribute it under certain conditions.
// Read LICENSE.md for more information.
//
// Provides:
//   - Various filesystem and fstream helpers to improve workflow with file operations
//------------------------------------------------------------------------------

#pragma once

#include <string>
#include <vector>
#include <sstream>
#include <fstream>
#include <filesystem>

namespace KalaHeaders
{
	using std::exception;
	using std::string;
	using std::vector;
	using std::ostringstream;
	using std::ifstream;
	using std::ofstream;
	using std::ios;
	using std::filesystem::exists;
	using std::filesystem::path;
	using std::filesystem::is_regular_file;
	using std::filesystem::is_directory;
	using std::filesystem::copy_file;
	using std::filesystem::copy_options;
	using std::filesystem::copy;
	using std::filesystem::rename;
	using std::filesystem::remove;
	using std::filesystem::remove_all;
	using std::filesystem::file_size;
	using std::filesystem::recursive_directory_iterator;
	using std::filesystem::directory_iterator;

	//Get the filename of the target (with extension)
	inline string GetPathName(
		const path& target,
		string& outName)
	{
		ostringstream oss{};

		if (!exists(target))
		{
			oss << "Failed to get target '"
				<< target << "' name because it does not exist!";

			return oss.str();
		}

		outName = target.filename().string();

		return{};
	}
	//Get the stem (filename without extension) of the target
	inline string GetPathStem(
		const path& target,
		string& outStem)
	{
		ostringstream oss{};

		if (!exists(target))
		{
			oss << "Failed to get target '"
				<< target << "' stem because it does not exist!";

			return oss.str();
		}

		outStem = target.stem().string();

		return{};
	}

	//Set the extension of the target
	inline string SetPathExtension(
		const path& target,
		const string& newExtension,
		string& outResult)
	{
		ostringstream oss{};

		if (!exists(target))
		{
			oss << "Failed to set target '"
				<< target << "' extension because it does not exist!";

			return oss.str();
		}
		if (!is_regular_file(target))
		{
			oss << "Failed to set extension for target '"
				<< target << "' because it is not a regular file!";

			return oss.str();
		}

		try
		{
			path newTarget = target;
			newTarget.replace_extension(newExtension);

			string result = RenamePath(
				target,
				newTarget.filename().string());

			if (!result.empty())
			{
				oss << "Failed to set target extension to '"
					<< newExtension << "' because RenameTarget failed! Reason: " << result;

				return oss.str();
			}

			outResult = newTarget.string();
		}
		catch (exception& e)
		{
			oss << "Failed to set extension for target '"
				<< target << "' to '" + newExtension + "'! Reason: " << e.what();

			return oss.str();
		}

		return{};
	}
	//Get the extension of the target
	inline string GetPathExtension(
		const path& target,
		string& outExtension)
	{
		ostringstream oss{};

		if (!exists(target))
		{
			oss << "Failed to get target '"
				<< target << "' extension because it does not exist!";

			return oss.str();
		}
		if (!is_regular_file(target))
		{
			oss << "Failed to get extension for target '"
				<< target << "' because it is not a regular file!";

			return oss.str();
		}

		outExtension = target.extension().string();

		return{};
	}

	//Get the parent directory of the target
	inline string GetPathParent(
		const path& target,
		string& outParent)
	{
		ostringstream oss{};

		if (!exists(target))
		{
			oss << "Failed to get target '"
				<< target << "' parent path because it does not exist!";

			return oss.str();
		}
		if (!target.has_parent_path())
		{
			oss << "Failed to get parent path for target '"
				<< target << "' because it does not have a parent!";

			return oss.str();
		}

		outParent = target.parent_path().string();

		return{};
	}

	//Copy file or folder from origin to target, with optional overwrite flag
	inline string CopyPath(
		const path& origin,
		const path& target,
		bool overwrite = false)
	{
		ostringstream oss{};

		if (!exists(origin))
		{
			oss << "Failed to copy origin to target because origin '"
				<< origin << "' does not exist!";

			return oss.str();
		}
		if (exists(target)
			&& overwrite)
		{
			string result = DeletePath(target);
			if (!result.empty())
			{
				oss << "Failed to copy origin '"
					<< origin << "' to target '"
					<< target << "' because overwrite was enabled and target couldn't be deleted! Reason: " << result;

				return oss.str();
			}
		}

		try
		{
			if (is_regular_file(origin))
			{
				copy_file(
					origin,
					target,
					overwrite
						? copy_options::overwrite_existing
						: copy_options::skip_existing);
			}
			else if (is_directory(origin))
			{
				copy(
					origin,
					target,
					overwrite
						? copy_options::overwrite_existing
						| copy_options::recursive
						: copy_options::skip_existing
						| copy_options::recursive);
			}
		}
		catch (exception& e)
		{
			oss << "Failed to copy '" 
				<< origin << "' to target '"
				<< target << "'! Reason: " << e.what();

			return oss.str();
		}

		return{};
	}

	//Move file or folder from origin to target, target is always overwritten if it already exists
	inline string MovePath(
		const path& origin,
		const path& target)
	{
		ostringstream oss{};

		if (!exists(origin))
		{
			oss << "Failed to move origin to target because origin '"
				<< origin << "' does not exist!";

			return oss.str();
		}
		if (exists(target))
		{
			string result = DeletePath(target);
			if (!result.empty())
			{
				oss << "Failed to move origin '"
					<< origin << "' to target '"
					<< target << "' because it existed and it couldn't be deleted! Reason: " << result;

				return oss.str();
			}
		}

		try
		{
			rename(origin, target);
		}
		catch (exception& e)
		{
			oss << "Failed to move '"
				<< origin << "' to target '"
				<< target << "'! Reason: " << e.what();

			return oss.str();
		}

		return{};
	}

	//Delete file or folder in target path (recursive for directories)
	inline string DeletePath(const path& target)
	{
		ostringstream oss{};

		if (!exists(target))
		{
			oss << "Failed to delete target '"
				<< target << "' because it does not exist!";

			return oss.str();
		}

		try
		{
			if (is_regular_file(target)) remove(target);
			else if (is_directory(target)) remove_all(target);
		}
		catch (exception& e)
		{
			oss << "Failed to delete '"
				<< target << "'! Reason: " << e.what();

			return oss.str();
		}

		return{};
	}

	//Rename file or folder in its current directory
	inline string RenamePath(
		const path& target,
		const string& newName)
	{
		ostringstream oss{};

		if (!exists(target))
		{
			oss << "Failed to rename target '" << target << "' to '"
				<< newName << "' because it does not exist!";

			return oss.str();
		}

		try
		{
			path newTarget = target.parent_path() / newName;
			rename(target, newTarget);
		}
		catch (exception& e)
		{
			oss << "Failed to rename '"
				<< target << "' to '"
				<< newName << "'! Reason: " << e.what();

			return oss.str();
		}

		return{};
	}

	//Get the size of the target in bytes
	inline string GetFileSize(
		const path& target, 
		uintmax_t& outSize)
	{
		ostringstream oss{};

		if (!exists(target))
		{
			oss << "Failed to get target '" << target << "' file size because it does not exist!";

			return oss.str();
		}
		if (!is_regular_file(target))
		{
			oss << "Failed to get target '" << target << "' file size because it is not a regular file!";

			return oss.str();
		}

		try
		{
			outSize = file_size(target);
		}
		catch (exception& e)
		{
			oss << "Failed to get target '" << target << "' file size! Reason: " << e.what();

			return oss.str();
		}

		return{};
	}

	//Write all text from a string to a text file, with optional append flag.
	//A new file is created at target path if it doesn't already exist
	inline string WriteTextToFile(
		const path& target,
		const string& inText,
		bool append = false)
	{
		ostringstream oss{};

		if (exists(target)
			&& !is_regular_file(target))
		{
			oss << "Failed to write text to target '" << target << "' because it is not a regular file!";

			return oss.str();
		}
		if (inText.empty())
		{
			oss << "Failed to write text to target '" << target << "' because inText string is empty!";

			return oss.str();
		}

		try
		{
			ofstream out;

			if (append)
			{
				out.open(
					target,
					ios::out
					| ios::app);
			}
			else
			{
				out.open(
					target,
					ios::out
					| ios::trunc);
			}

			if (!out.is_open())
			{
				oss << "Failed to write text to target '" << target << "' because it couldn't be opened!";

				return oss.str();
			}

			out << inText;

			out.close();
		}
		catch (exception& e)
		{
			oss << "Failed to write text to target '" << target << "'! Reason: " << e.what();

			return oss.str();
		}

		return{};
	}
	//Read all text from a file into a string
	inline string ReadTextFromFile(
		const path& target, 
		string& outText)
	{
		ostringstream oss{};

		if (!exists(target))
		{
			oss << "Failed to read text from target '" << target << "' because it does not exist!";

			return oss.str();
		}
		if (!is_regular_file(target))
		{
			oss << "Failed to read text from target '" << target << "' because it is not a regular file!";

			return oss.str();
		}

		try
		{
			ifstream in(target);

			if (!in.is_open())
			{
				oss << "Failed to read text from target '" << target << "' because it couldn't be opened!";

				return oss.str();
			}

			ostringstream buffer{};

			buffer << in.rdbuf();
			outText = buffer.str();

			in.close();
		}
		catch (exception& e)
		{
			oss << "Failed to read text from target '" << target << "'! Reason: " << e.what();

			return oss.str();
		}

		return{};
	}

	//Write all lines from a vector to a text file, with optional append flag.
	//A new file is created at target path if it doesn't already exist
	inline string WriteLinesToFile(
		const path& target,
		const vector<string>& inLines,
		bool append = false)
	{
		ostringstream oss{};

		if (exists(target)
			&& !is_regular_file(target))
		{
			oss << "Failed to write lines to target '" << target << "' because it is not a regular file!";

			return oss.str();
		}
		if (inLines.empty())
		{
			oss << "Failed to write lines to target '" << target << "' because inLines vector is empty!";

			return oss.str();
		}

		try
		{
			ofstream out;

			if (append)
			{
				out.open(
					target,
					ios::out
					| ios::app);
			}
			else
			{
				out.open(
					target,
					ios::out
					| ios::trunc);
			}

			if (!out.is_open())
			{
				oss << "Failed to write lines to target '" << target << "' because it couldn't be opened!";

				return oss.str();
			}

			for (size_t i = 0; i < inLines.size(); ++i)
			{
				out << inLines[i];
				if (i + 1 < inLines.size()) out << '\n';
			}

			out.close();
		}
		catch (exception& e)
		{
			oss << "Failed to write lines to target '" << target << "'! Reason: " << e.what();

			return oss.str();
		}

		return{};
	}
	//Read all lines from a file into a vector of strings
	inline string ReadLinesFromFile(
		const path& target,
		vector<string>& outLines)
	{
		ostringstream oss{};

		if (!exists(target))
		{
			oss << "Failed to read lines from target '" << target << "' because it does not exist!";

			return oss.str();
		}
		if (!is_regular_file(target))
		{
			oss << "Failed to read lines from target '" << target << "' because it is not a regular file!";

			return oss.str();
		}

		try
		{
			ifstream in(target);

			if (!in.is_open())
			{
				oss << "Failed to read lines from target '" << target << "' because it couldn't be opened!";

				return oss.str();
			}

			string line{};
			while (getline(in, line))
			{
				outLines.push_back(line);
			}

			in.close();
		}
		catch (exception& e)
		{
			oss << "Failed to read lines from target '" << target << "'! Reason: " << e.what();

			return oss.str();
		}

		return{};
	}

	//List all the contents of a folder, with optional recursive flag
	inline string ListDirectoryContents(
		const path& target,
		vector<path>& outEntries,
		bool recursive = false)
	{
		ostringstream oss{};

		if (!exists(target))
		{
			oss << "Failed to list paths from target '" << target << "' because it does not exist!";

			return oss.str();
		}
		if (!is_directory(target))
		{
			oss << "Failed to list paths from target '" << target << "' because it is not a directory!";

			return oss.str();
		}

		try
		{
			if (recursive)
			{
				for (auto& entry : recursive_directory_iterator(target))
				{
					outEntries.push_back(entry.path());
				}
			}
			else
			{
				for (auto& entry : directory_iterator(target))
				{
					outEntries.push_back(entry.path());
				}
			}
		}
		catch (exception& e)
		{
			oss << "Failed to list paths from target '" << target << "'! Reason: " << e.what();

			return oss.str();
		}

		return{};
	}
}