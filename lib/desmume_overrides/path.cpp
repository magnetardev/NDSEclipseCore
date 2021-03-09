/*
	Copyright (C) 2009-2016 DeSmuME team

	This file is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 2 of the License, or
	(at your option) any later version.

	This file is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with the this software.  If not, see <http://www.gnu.org/licenses/>.
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include "types.h"
#include "path.h"
#include "utils/xstring.h"

//-----------------------------------
//This is taken from mono Path.cs
static const char InvalidPathChars[] = {
	'\x22', '\x3C', '\x3E', '\x7C', '\x00', '\x01', '\x02', '\x03', '\x04', '\x05', '\x06', '\x07',
	'\x08', '\x09', '\x0A', '\x0B', '\x0C', '\x0D', '\x0E', '\x0F', '\x10', '\x11', '\x12', 
	'\x13', '\x14', '\x15', '\x16', '\x17', '\x18', '\x19', '\x1A', '\x1B', '\x1C', '\x1D', 
	'\x1E', '\x1F'
};

//but it is sort of windows-specific. Does it work in linux? Maybe we'll have to make it smarter
static const char VolumeSeparatorChar = ':';
static bool dirEqualsVolume = (DIRECTORY_DELIMITER_CHAR == VolumeSeparatorChar);
bool Path::IsPathRooted(const std::string &path) {
	if (path.empty() || path.find_first_of(InvalidPathChars) != std::string::npos) 
    return false;
	
	std::string delimiters = ALL_DIRECTORY_DELIMITER_STRING;
	return (delimiters.find(path[0]) != std::string::npos ||
			(!dirEqualsVolume && path.size() > 1 && path[1] == VolumeSeparatorChar));
}

std::string Path::GetFileDirectoryPath(std::string filePath) {
	if (filePath.empty()) return "";
	size_t i = filePath.find_last_of(ALL_DIRECTORY_DELIMITER_STRING);
	if (i == std::string::npos) return filePath;
	return filePath.substr(0, i);
}

std::string Path::GetFileNameFromPath(std::string filePath) {
	if (filePath.empty()) return "";
	size_t i = filePath.find_last_of(ALL_DIRECTORY_DELIMITER_STRING);
	if (i == std::string::npos) return filePath;
	return filePath.substr(i + 1);
}

std::string Path::GetFileNameWithoutExt(std::string fileName) {
	if (fileName.empty()) return "";
	size_t i = fileName.find_last_of(FILE_EXT_DELIMITER_CHAR);
	if (i == std::string::npos)	return fileName;
	return fileName.substr(0, i);
}

std::string Path::ScrubInvalid(std::string str) {
	for (std::string::iterator it(str.begin()); it != str.end(); ++it) {
		bool ok = true;
		for (int i=0;i<ARRAY_SIZE(InvalidPathChars);i++) {
			if (InvalidPathChars[i] == *it) {
				ok = false;
				break;
			}
		}
		if (!ok) *it = '*';
	}

	return str;
}

std::string Path::GetFileNameFromPathWithoutExt(std::string filePath) {
	if (filePath.empty()) return "";
	std::string fileName = GetFileNameFromPath(filePath);
	return GetFileNameWithoutExt(fileName);
}

std::string Path::GetFileExt(std::string fileName) {
	if (fileName.empty()) return "";
	size_t i = fileName.find_last_of(FILE_EXT_DELIMITER_CHAR);
	if (i == std::string::npos) return fileName;
	return fileName.substr(i + 1);
}

void PathInfo::init(const char *filename)  {
	path = std::string(filename);

	//extract the internal part of the logical rom name
	std::vector<std::string> parts = tokenize_str(filename,"|");
	SetRomName(parts[parts.size()-1].c_str());
	LoadModulePath();
	ReadPathSettings();		
}

void PathInfo::LoadModulePath() {
  mkdir("/desmume", S_IRWXU);
	strncpy(pathToModule, "/desmume", MAX_PATH);
}

void PathInfo::GetDefaultPath(char *pathToDefault, const char *key, int maxCount) {
	strncpy(pathToDefault, pathToModule, maxCount);
}

void PathInfo::ReadKey(char *pathToRead, const char *key) {
	//since the variables are all intialized in this file they all use MAX_PATH
	GetDefaultPath(pathToRead, key, MAX_PATH);
}


void PathInfo::ReadPathSettings() {
	if ((strcmp(pathToModule, "") == 0) || !pathToModule) LoadModulePath();

	ReadKey(pathToRoms, ROMKEY);
	ReadKey(pathToBattery, BATTERYKEY);
	ReadKey(pathToSramImportExport, SRAMIMPORTKEY);
	ReadKey(pathToStates, STATEKEY);
	ReadKey(pathToStateSlots, STATESLOTKEY);
	ReadKey(pathToScreenshots, SCREENSHOTKEY);
	ReadKey(pathToAviFiles, AVIKEY);
	ReadKey(pathToCheats, CHEATKEY);
	ReadKey(pathToSounds, SOUNDKEY);
	ReadKey(pathToFirmware, FIRMWAREKEY);
	ReadKey(pathToLua, LUAKEY);
	ReadKey(pathToSlot1D, SLOT1DKEY);
}

void PathInfo::SwitchPath(Action action, KnownPath path, char *buffer) {
	char *pathToCopy = 0;
	switch (path) {
	case ROMS:
		pathToCopy = pathToRoms;
		break;
	case BATTERY:
		pathToCopy = pathToBattery;
		break;
	case SRAM_IMPORT_EXPORT:
		pathToCopy = pathToSramImportExport;
		break;
	case STATES:
		pathToCopy = pathToStates;
		break;
	case STATE_SLOTS:
		pathToCopy = pathToStateSlots;
		break;
	case SCREENSHOTS:
		pathToCopy = pathToScreenshots;
		break;
	case AVI_FILES:
		pathToCopy = pathToAviFiles;
		break;
	case CHEATS:
		pathToCopy = pathToCheats;
		break;
	case SOUNDS:
		pathToCopy = pathToSounds;
		break;
	case FIRMWARE:
		pathToCopy = pathToFirmware;
		break;
	case MODULE:
		pathToCopy = pathToModule;
		break;
	case SLOT1D:
		pathToCopy = pathToSlot1D;
		break;
	}

	if (action == GET) {
		std::string thePath = pathToCopy;
		std::string relativePath = (std::string)"." + DIRECTORY_DELIMITER_CHAR;

		int len = (int)thePath.size() - 1;

		if (len == -1) thePath = relativePath;
		else if (thePath[len] != DIRECTORY_DELIMITER_CHAR) thePath += DIRECTORY_DELIMITER_CHAR;

		if (!Path::IsPathRooted(thePath)) thePath = (std::string)pathToModule + thePath;

		strncpy(buffer, thePath.c_str(), MAX_PATH);
	} else if (action == SET) {
		int len = strlen(buffer) - 1;
		if (std::string(ALL_DIRECTORY_DELIMITER_STRING).find(buffer[len]) != std::string::npos)
			buffer[len] = '\0';

		strncpy(pathToCopy, buffer, MAX_PATH);
	}
}

std::string PathInfo::getpath(KnownPath path) {
	char temp[MAX_PATH];
	SwitchPath(GET, path, temp);
	return temp;
}

void PathInfo::getpath(KnownPath path, char *buffer) {
	SwitchPath(GET, path, buffer);
}

void PathInfo::setpath(KnownPath path, std::string value) {
	SwitchPath(SET, path, (char*)value.c_str());
}

void PathInfo::setpath(KnownPath path, char *buffer) {
	SwitchPath(SET, path, buffer);
}

void PathInfo::getfilename(char *buffer, int maxCount) {
	strcpy(buffer, noextension().c_str());
}

void PathInfo::getpathnoext(KnownPath path, char *buffer) {
	getpath(path, buffer);
	strcat(buffer, GetRomNameWithoutExtension().c_str());
}

std::string PathInfo::extension() {
	return Path::GetFileExt(path);
}

std::string PathInfo::noextension() {
	return Path::GetFileDirectoryPath(path) + DIRECTORY_DELIMITER_CHAR + Path::GetFileNameWithoutExt(RomName);
}

void PathInfo::formatname(char *output) {
	// Except 't' for tick and 'r' for random.
	const char* strftimeArgs = "AbBcCdDeFgGhHIjmMnpRStTuUVwWxXyYzZ%";

	std::string file;
	time_t now = time(NULL);
	tm *time_struct = localtime(&now);

	for (char* p = screenshotFormat, *end = p + sizeof(screenshotFormat); p < end; p++) {
		if (*p != '%') file.append(1, *p);
		else {
			p++;
			if (*p == 'f') file.append(GetRomNameWithoutExtension());
			else if (*p == 'r') file.append(stditoa(rand()));
			else if (*p == 't') file.append(stditoa(clock() >> 5));
			else if (strchr(strftimeArgs, *p)) {
				char tmp[MAX_PATH];
				char format[] = { '%', *p, '\0' };
				strftime(tmp, MAX_PATH, format, time_struct);
				file.append(tmp);
			}
		}
	}

	strncpy(output, file.c_str(), MAX_PATH);
}

PathInfo::ImageFormat PathInfo::imageformat() {
	return currentimageformat;
}

void PathInfo::SetRomName(const char *filename) {
	std::string romPath = filename;
	RomName = Path::GetFileNameFromPath(romPath);
	RomName = Path::ScrubInvalid(RomName);
	RomDirectory = Path::GetFileDirectoryPath(romPath);
}

const char *PathInfo::GetRomName() {
	return RomName.c_str();
}

std::string PathInfo::GetRomNameWithoutExtension() {
	if (RomName.c_str() == NULL) return "";
	return Path::GetFileNameWithoutExt(RomName);
}

bool PathInfo::isdsgba(std::string fileName) {
	size_t i = fileName.find_last_of(FILE_EXT_DELIMITER_CHAR);
	if (i != std::string::npos) fileName = fileName.substr(i - 2);
	return fileName == "ds.gba";
}
