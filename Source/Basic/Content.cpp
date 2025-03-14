/* Copyright (c) 2023 Jin Li, dragon-fly@qq.com

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE. */

#include "Const/Header.h"

#include "Basic/Content.h"

#include "Basic/Application.h"
#include "Basic/VGRender.h"
#include "Common/Async.h"
#include <fstream>
using std::ofstream;
#include <mutex>

#if BX_PLATFORM_LINUX
#include <limits.h>
#include <unistd.h>

#include "ghc/fs_impl.hpp"

#include "ghc/fs_fwd.hpp"
namespace fs = ghc::filesystem;
#else
#include <filesystem>
namespace fs = std::filesystem;
#endif // BX_PLATFORM_LINUX

#if BX_PLATFORM_ANDROID
#include "ZipUtils.h"
#endif

#include "miniz.h"

#include "SDL.h"

static void releaseFileData(void* _ptr, void* _userData) {
	DORA_UNUSED_PARAM(_userData);
	if (_ptr) {
		uint8_t* data = r_cast<uint8_t*>(_ptr);
		delete[] data;
	}
}

NS_DOROTHY_BEGIN

#if BX_PLATFORM_ANDROID

bool Content::isAndroidAsset(String fullPath) const {
	return fullPath.left(_assetPath.length()) == _assetPath;
}

String Content::getAndroidAssetName(String fullPath) const {
	const auto& apkPath = SharedApplication.getAPKPath();
	fullPath.skip(apkPath.length() + 1);
	return fullPath;
}

#endif // BX_PLATFORM_ANDROID

Content::~Content() { }

Async* Content::getThread() const {
	return _thread;
}

std::pair<OwnArray<uint8_t>, size_t> Content::load(String filename) {
	_thread->pause();
	int64_t size = 0;
	uint8_t* data = Content::loadUnsafe(filename, size);
	_thread->resume();
	return {OwnArray<uint8_t>(data), s_cast<size_t>(size)};
}

const bgfx::Memory* Content::loadBX(String filename) {
	_thread->pause();
	int64_t size = 0;
	uint8_t* data = Content::loadUnsafe(filename, size);
	_thread->resume();
	return bgfx::makeRef(data, (uint32_t)size, releaseFileData);
}

bool Content::copy(String src, String dst) {
	_thread->pause();
	bool result = Content::copyUnsafe(src, dst);
	_thread->resume();
	return result;
}

bool Content::move(String src, String dst) {
	std::error_code err;
	fs::rename(src.toString(), dst.toString(), err);
	WarnIf(err, "failed to move file from \"{}\" to \"{}\" due to \"{}\".", src, dst, err.message());
	return !err;
}

bool Content::save(String filename, String content) {
	ofstream stream(Content::getFullPath(filename), std::ios::trunc | std::ios::binary);
	if (!stream) return false;
	if (stream.write(content.rawData(), content.size())) {
		return true;
	}
	return false;
}

bool Content::save(String filename, uint8_t* content, int64_t size) {
	ofstream stream(Content::getFullPath(filename), std::ios::trunc | std::ios::binary);
	if (!stream) return false;
	if (stream.write(r_cast<char*>(content), s_cast<std::streamsize>(size))) {
		return true;
	}
	return false;
}

bool Content::remove(String filename) {
	std::string fullPath = Content::getFullPath(filename);
	if (!Content::exist(fullPath)) return false;
	std::error_code err;
	fs::remove_all(fullPath, err);
	WarnIf(err, "failed to remove files from \"{}\" due to \"{}\".", filename, err.message());
	return !err;
}

bool Content::createFolder(String folder) {
	fs::path path = folder.toString();
	return fs::create_directories(path);
}

std::list<std::string> Content::getDirs(String path) {
	return Content::getDirEntries(path, true);
}

std::list<std::string> Content::getFiles(String path) {
	return Content::getDirEntries(path, false);
}

std::list<std::string> Content::getAllFiles(String path) {
	std::string searchName = path.empty() ? _assetPath : path.toString();
	std::string fullPath = Content::getFullPath(searchName);
#if BX_PLATFORM_ANDROID
	if (isAndroidAsset(fullPath)) {
		return _apkFile->getAllFiles(getAndroidAssetName(fullPath));
	}
#endif // BX_PLATFORM_ANDROID
	std::list<std::string> files;
	if (Content::isFileExist(fullPath)) {
		fs::path parentPath = fullPath;
		for (const auto& item : fs::recursive_directory_iterator(parentPath)) {
			if (!item.is_directory()) {
				files.push_back(item.path().lexically_relative(parentPath).string());
			}
		}
	} else {
		Error("Content failed to get entry of \"{}\"", fullPath);
	}
	return files;
}

bool Content::visitDir(String path, const std::function<bool(String, String)>& func) {
	std::function<bool(String)> visit;
	visit = [&visit, &func, this](String path) {
		auto files = getFiles(path);
		for (const auto& file : files) {
			if (func(file, path)) return true;
		}
		auto dirs = getDirs(path);
		auto parent = fs::path(path.begin(), path.end());
		for (const auto& dir : dirs) {
			if (visit((parent / dir).string())) return true;
		}
		return false;
	};
	return visit(path);
}

const std::string& Content::getAssetPath() const {
	return _assetPath;
}

const std::string& Content::getWritablePath() const {
	return _writablePath;
}

static std::tuple<std::string, std::string> splitDirectoryAndFilename(const std::string& filePath) {
	std::string file = filePath;
	std::string path;
	size_t pos = filePath.find_last_of("/\\");
	if (pos != std::string::npos) {
		path = filePath.substr(0, pos + 1);
		file = filePath.substr(pos + 1);
	}
	return std::make_tuple(path, file);
}

std::string Content::getFullPath(String filename) {
	AssertIf(filename.empty(), "invalid filename for full path.");

	Slice targetFile = filename;
	targetFile.trimSpace();

	if (Content::isAbsolutePath(targetFile)) {
		return targetFile;
	}

	while (targetFile.size() > 1 && (targetFile.back() == '\\' || targetFile.back() == '/')) {
		targetFile.skipRight(1);
	}

	static std::mutex pathMutex;
	{
		std::lock_guard<std::mutex> lock(pathMutex);

		auto it = _fullPathCache.find(targetFile);
		if (it != _fullPathCache.end()) {
			return it->second;
		}

		std::string path, file, fullPath;
		auto fname = fs::path(targetFile.begin(), targetFile.end()).lexically_normal();
		for (const auto& searchPath : _searchPaths) {
			std::tie(path, file) = splitDirectoryAndFilename((fs::path(searchPath) / fname).string());
			fullPath = Content::getFullPathForDirectoryAndFilename(path, file);
			if (!fullPath.empty()) {
				_fullPathCache[fname.string()] = fullPath;
				return fullPath;
			}
		}

		std::tie(path, file) = splitDirectoryAndFilename(targetFile);
		fullPath = Content::getFullPathForDirectoryAndFilename(path, file);
		if (!fullPath.empty()) {
			_fullPathCache[targetFile] = fullPath;
			return fullPath;
		}
	}

	return targetFile;
}

std::list<std::string> Content::getFullPathsToTry(String filename) {
	AssertIf(filename.empty(), "invalid filename for full path.");

	Slice targetFile = filename;
	targetFile.trimSpace();

	while (targetFile.size() > 1 && (targetFile.back() == '\\' || targetFile.back() == '/')) {
		targetFile.skipRight(1);
	}

	if (Content::isAbsolutePath(targetFile)) {
		return {targetFile};
	}

	std::list<std::string> paths;
	std::string path, file, fullPath;
	auto fname = fs::path(targetFile.begin(), targetFile.end()).lexically_normal();
	for (const auto& searchPath : _searchPaths) {
		paths.push_back((fs::path(searchPath) / fname).string());
	}
	return paths;
}

void Content::insertSearchPath(int index, String path) {
	std::string searchPath = Content::getFullPath(path);
	_searchPaths.insert(_searchPaths.begin() + index, searchPath);
	_fullPathCache.clear();
}

void Content::addSearchPath(String path) {
	std::string searchPath = Content::getFullPath(path);
	_searchPaths.push_back(searchPath);
}

void Content::removeSearchPath(String path) {
	std::string realPath = Content::getFullPath(path);
	for (auto it = _searchPaths.begin(); it != _searchPaths.end(); ++it) {
		if (*it == realPath) {
			_searchPaths.erase(it);
			_fullPathCache.clear();
			break;
		}
	}
}

void Content::setSearchPaths(const std::vector<std::string>& searchPaths) {
	_searchPaths.clear();
	_fullPathCache.clear();
	for (const std::string& searchPath : searchPaths) {
		Content::addSearchPath(searchPath);
	}
}

const std::vector<std::string>& Content::getSearchPaths() const {
	return _searchPaths;
}

bool Content::copyUnsafe(String src, String dst) {
	std::string srcPath = Content::getFullPath(src);
	// Info("copy file from {}", srcPath);
	// Info("copy file to {}", dst);
	if (Content::isPathFolder(srcPath)) {
		std::string dstPath = dst;
		auto folders = Content::getDirEntries(src, true);
		for (const std::string& folder : folders) {
			std::string dstFolder = (fs::path(dstPath) / folder).string();
			if (!Content::isFileExist(dstFolder)) {
				if (!Content::createFolder(dstFolder)) {
					Error("failed to create folder \"{}\"", dstFolder);
					return false;
				}
			}
			std::string srcFolder = (fs::path(srcPath) / folder).string();
			if (!Content::copyUnsafe(srcFolder, dstFolder)) {
				return false;
			}
		}
		auto files = Content::getDirEntries(src, false);
		for (const std::string& file : files) {
			// Info("now copy file {}",file);
			ofstream stream(fs::path(dstPath) / file, std::ios::out | std::ios::trunc | std::ios::binary);
			if (!stream) return false;
			bool result = Content::loadByChunks((fs::path(srcPath) / file).string(), [&](uint8_t* buffer, int size) {
				if (!stream.write(r_cast<char*>(buffer), size)) {
					Error("failed to copy to file \"{}\"", (fs::path(dstPath) / file).string());
					return true;
				}
				return false;
			});
			if (!result) {
				return false;
			}
		}
	} else {
		ofstream stream(dst, std::ios::out | std::ios::trunc | std::ios::binary);
		if (!stream) {
			Error("failed to open file: \"{}\"", dst);
			return false;
		}
		bool result = Content::loadByChunks(src, [&](uint8_t* buffer, int size) {
			if (!stream.write(r_cast<char*>(buffer), size)) {
				Error("failed to copy to file \"{}\"", dst);
				return true;
			}
			return false;
		});
		if (!result) {
			return false;
		}
	}
	return true;
}

void Content::loadAsyncUnsafe(String filename, const std::function<void(uint8_t*, int64_t)>& callback) {
	std::string fileStr = filename;
	_thread->run(
		[fileStr, this]() {
			int64_t size = 0;
			uint8_t* buffer = this->loadUnsafe(fileStr, size);
			return Values::alloc(buffer, size);
		},
		[callback](Own<Values> result) {
			uint8_t* buffer;
			int64_t size;
			result->get(buffer, size);
			callback(buffer, size);
		});
}

void Content::loadAsync(String filename, const std::function<void(String)>& callback) {
	Content::loadAsyncUnsafe(filename, [callback](uint8_t* buffer, int64_t size) {
		auto data = MakeOwnArray(buffer);
		callback(Slice(r_cast<char*>(data.get()), s_cast<size_t>(size)));
	});
}

void Content::loadAsyncData(String filename, const std::function<void(OwnArray<uint8_t>&&, size_t)>& callback) {
	Content::loadAsyncUnsafe(filename, [callback](uint8_t* buffer, int64_t size) {
		callback(MakeOwnArray(buffer), s_cast<size_t>(size));
	});
}

void Content::loadAsyncBX(String filename, const std::function<void(const bgfx::Memory*)>& callback) {
	Content::loadAsyncUnsafe(filename, [callback](uint8_t* buffer, int64_t size) {
		callback(bgfx::makeRef(buffer, s_cast<uint32_t>(size), releaseFileData));
	});
}

void Content::copyAsync(String src, String dst, const std::function<void(bool)>& callback) {
	std::string srcFile(src), dstFile(dst);
	_thread->run(
		[srcFile, dstFile, this]() {
			bool success = Content::copyUnsafe(srcFile, dstFile);
			return Values::alloc(success);
		},
		[callback](Own<Values> result) {
			bool success = false;
			result->get(success);
			callback(success);
		});
}

void Content::saveAsync(String filename, String content, const std::function<void(bool)>& callback) {
	std::string file(filename);
	auto data = std::make_shared<std::string>(content);
	_thread->run(
		[file, data, this]() {
			bool success = Content::save(file, *data);
			return Values::alloc(success);
		},
		[callback](Own<Values> result) {
			bool success = false;
			result->get(success);
			callback(success);
		});
}

void Content::saveAsync(String filename, OwnArray<uint8_t> content, size_t size, const std::function<void(bool)>& callback) {
	std::string file(filename);
	auto data = std::make_shared<OwnArray<uint8_t>>(std::move(content));
	_thread->run(
		[file, data, size, this]() {
			bool success = Content::save(file, Slice(r_cast<char*>((*data).get()), size));
			return Values::alloc(success);
		},
		[callback](Own<Values> result) {
			bool success = false;
			result->get(success);
			callback(success);
		});
}

void Content::zipAsync(String zipFile, String folderPath, const std::function<bool(String)>& filter, const std::function<void(bool)>& callback) {
	std::error_code err;
	if (!fs::exists(folderPath.toString(), err)) {
		Error("\"{}\" must be a local disk folder to zip", folderPath);
		callback(false);
		return;
	}
	if (!Content::isFolder(folderPath)) {
		callback(false);
		return;
	}
	auto files = Content::getAllFiles(folderPath);
	std::list<std::pair<std::string, std::string>> filePairs;
	for (auto& file : files) {
		if (!filter(file)) continue;
		for (auto& ch : file) {
			if (ch == '\\') ch = '/';
		}
		auto fullPath = Content::getFullPath(Path::concat({folderPath, file}));
		filePairs.push_back({fullPath, file});
	}
	SharedAsyncThread.run([files = std::move(filePairs), zipFile = zipFile.toString()]() {
		mz_zip_archive archive;
		mz_zip_zero_struct(&archive);
		if (mz_zip_writer_init_file(&archive, zipFile.c_str(), 0)) {
			for (const auto& file : files) {
				if (!mz_zip_writer_add_file(&archive, file.second.c_str(), file.first.c_str(), nullptr, 0, MZ_DEFAULT_COMPRESSION)) {
					Error("failed to write file \"{}\" to zip, due to: {}", file.first, mz_zip_get_error_string(mz_zip_get_last_error(&archive)));
					mz_zip_writer_end(&archive);
					return Values::alloc(false);
				}
			}
			mz_zip_writer_finalize_archive(&archive);
			mz_zip_writer_end(&archive);
			return Values::alloc(true);
		} else {
			Error("failed to init zip file \"{}\", due to: {}", zipFile, mz_zip_get_error_string(mz_zip_get_last_error(&archive)));
			mz_zip_writer_end(&archive);
			return Values::alloc(false);
		}
	},
		[callback](Own<Values> values) {
			bool success = false;
			values->get(success);
			callback(success);
		});
}

bool Content::exist(String filename) {
	return Content::isFileExist(Content::getFullPath(filename));
}

bool Content::isFolder(String path) {
	return Content::isPathFolder(Content::getFullPath(path));
}

std::list<std::string> Content::getDirEntries(String path, bool isFolder) {
	std::string searchName = path.empty() ? _assetPath : path.toString();
	std::string fullPath = Content::getFullPath(searchName);
#if BX_PLATFORM_ANDROID
	if (isAndroidAsset(fullPath)) {
		return _apkFile->getDirEntries(getAndroidAssetName(fullPath), isFolder);
	}
#endif // BX_PLATFORM_ANDROID
	std::list<std::string> files;
	if (Content::isFileExist(fullPath)) {
		fs::path parentPath = fullPath;
		for (const auto& item : fs::directory_iterator(parentPath)) {
			if (isFolder == item.is_directory()) {
				files.push_back(item.path().lexically_relative(parentPath).string());
			}
		}
	} else {
		Error("Content failed to get entry of \"{}\"", fullPath);
	}
	return files;
}

uint8_t* Content::loadInMainUnsafe(String filename, int64_t& size) {
	_thread->pause();
	uint8_t* data = Content::loadUnsafe(filename, size);
	_thread->resume();
	return data;
}

void Content::clearPathCache() {
	_fullPathCache.clear();
}

#if BX_PLATFORM_ANDROID
Content::Content()
	: _thread(SharedAsyncThread.newThread()) {
	_apkFilter = "assets/"s;
	_assetPath = SharedApplication.getAPKPath() + '/' + _apkFilter;
	_apkFile = New<ZipFile>(SharedApplication.getAPKPath(), _apkFilter);

	char* prefPath = SDL_GetPrefPath(DORA_DEFAULT_ORG_NAME, DORA_DEFAULT_APP_NAME);
	_writablePath = prefPath;
	SDL_free(prefPath);
}

uint8_t* Content::loadUnsafe(String filename, int64_t& size) {
	uint8_t* data = nullptr;
	if (filename.empty()) {
		return data;
	}
	std::string fullPath = Content::getFullPath(filename);
	if (isAndroidAsset(fullPath)) {
		data = _apkFile->getFileDataUnsafe(getAndroidAssetName(fullPath), r_cast<size_t*>(&size));
	} else {
		BLOCK_START {
			FILE* fp = fopen(fullPath.c_str(), "rb");
			BREAK_IF(!fp);
			fseek(fp, 0, SEEK_END);
			unsigned long dataSize = ftell(fp);
			fseek(fp, 0, SEEK_SET);
			data = new unsigned char[dataSize];
			dataSize = fread(data, sizeof(data[0]), dataSize, fp);
			fclose(fp);
			if (dataSize) {
				size = dataSize;
			}
		}
		BLOCK_END
	}
	if (!data) {
		Error("failed to load file: {}", fullPath);
	}
	return data;
}

bool Content::loadByChunks(String filename, const std::function<bool(uint8_t*, int)>& handler) {
	if (filename.empty()) {
		return false;
	}
	std::string fullPath = Content::getFullPath(filename);
	if (isAndroidAsset(fullPath)) {
		if (_apkFile->getFileDataByChunks(getAndroidAssetName(fullPath), handler)) {
			return true;
		}
	} else {
		BLOCK_START {
			FILE* file = fopen(fullPath.c_str(), "rb");
			BREAK_IF(!file);
			uint8_t buffer[DORA_COPY_BUFFER_SIZE];
			int size = 0;
			do {
				size = s_cast<int>(fread(buffer, sizeof(uint8_t), DORA_COPY_BUFFER_SIZE, file));
				if (size > 0) {
					if (handler(buffer, size)) {
						return false;
					}
				}
			} while (size > 0);
			fclose(file);
			return true;
		}
		BLOCK_END
	}
	return false;
}

bool Content::isFileExist(String filename) {
	if (filename.empty()) {
		return false;
	}
	if (isAndroidAsset(filename)) {
		if (_apkFile->fileExists(getAndroidAssetName(filename))) {
			return true;
		}
	} else if (!isAbsolutePath(filename) && _apkFile->fileExists("assets/"s + filename)) {
		return true;
	}
	bool found = false;
	FILE* file = fopen(filename.c_str(), "r");
	if (file) {
		found = true;
		fclose(file);
	}
	return found;
}

bool Content::isPathFolder(String path) {
	if (isAndroidAsset(path)) {
		if (_apkFile->isFolder(getAndroidAssetName(path))) {
			return true;
		}
	} else if (!isAbsolutePath(path) && _apkFile->isFolder("assets/"s + path)) {
		return true;
	}
	return fs::is_directory(path.toString());
}
#endif // BX_PLATFORM_ANDROID

#if BX_PLATFORM_ANDROID || BX_PLATFORM_LINUX
bool Content::isAbsolutePath(String strPath) {
	if (strPath[0] == '/') {
		return true;
	}
	return false;
}
#endif // BX_PLATFORM_ANDROID || BX_PLATFORM_LINUX

#if BX_PLATFORM_WINDOWS
Content::Content()
	: _thread(SharedAsyncThread.newThread()) {
	_assetPath = fs::current_path().string();

	char* prefPath = SDL_GetPrefPath(DORA_DEFAULT_ORG_NAME, DORA_DEFAULT_APP_NAME);
	_writablePath = prefPath;
	SDL_free(prefPath);
}

bool Content::isAbsolutePath(String strPath) {
	if (strPath.size() > 2
		&& ((strPath[0] >= 'a' && strPath[0] <= 'z') || (strPath[0] >= 'A' && strPath[0] <= 'Z'))
		&& strPath[1] == ':') {
		return true;
	}
	return false;
}
#endif // BX_PLATFORM_WINDOWS

#if BX_PLATFORM_WINDOWS || BX_PLATFORM_LINUX
bool Content::isFileExist(String filePath) {
	std::string strPath = filePath;
	if (!Content::isAbsolutePath(strPath)) {
		strPath.insert(0, _assetPath);
	}
	std::error_code err;
	return fs::exists(strPath, err);
}
#endif // BX_PLATFORM_WINDOWS || BX_PLATFORM_LINUX

#if BX_PLATFORM_OSX || BX_PLATFORM_IOS
Content::Content()
	: _thread(SharedAsyncThread.newThread()) {
	char* currentPath = SDL_GetBasePath();
	_assetPath = currentPath;
	SDL_free(currentPath);

	char* prefPath = SDL_GetPrefPath(DORA_DEFAULT_ORG_NAME, DORA_DEFAULT_APP_NAME);
	_writablePath = prefPath;
	SDL_free(prefPath);
}
#endif // BX_PLATFORM_OSX || BX_PLATFORM_IOS

#if BX_PLATFORM_LINUX
Content::Content()
	: _thread(SharedAsyncThread.newThread()) {
	auto currentPath = NewArray<char>(PATH_MAX);
	::getcwd(currentPath.get(), PATH_MAX);
	_assetPath = currentPath.get();

	char* prefPath = SDL_GetPrefPath(DORA_DEFAULT_ORG_NAME, DORA_DEFAULT_APP_NAME);
	_writablePath = prefPath;
	SDL_free(prefPath);
}
#endif // BX_PLATFORM_LINUX

#if BX_PLATFORM_WINDOWS || BX_PLATFORM_OSX || BX_PLATFORM_IOS || BX_PLATFORM_LINUX

#if BX_PLATFORM_WINDOWS
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif // WIN32_LEAN_AND_MEAN
#include <windows.h>
static std::string toUTF8String(const std::string& str) {
	int wsize = MultiByteToWideChar(CP_ACP, 0, str.data(), str.length(), 0, 0);
	std::wstring wstr(wsize, 0);
	MultiByteToWideChar(CP_ACP, 0, str.data(), str.length(), &wstr[0], wsize);
	int u8size = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), wstr.length(), nullptr, 0, nullptr, nullptr);
	std::string u8str(u8size, '\0');
	WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), wstr.length(), &u8str[0], u8size, nullptr, nullptr);
	return u8str;
}
#endif // BX_PLATFORM_WINDOWS

uint8_t* Content::loadUnsafe(String filename, int64_t& size) {
	if (filename.empty()) return nullptr;
	std::string fullPath =
#if BX_PLATFORM_WINDOWS
		toUTF8String(Content::getFullPath(filename));
#else
		Content::getFullPath(filename);
#endif // BX_PLATFORM_WINDOWS
	SDL_RWops* io = SDL_RWFromFile(fullPath.c_str(), "rb");
	if (io == nullptr) {
		Error("failed to load file: {}", filename);
		return nullptr;
	}
	size = SDL_RWsize(io);
	uint8_t* buffer = new uint8_t[s_cast<size_t>(size)];
	SDL_RWread(io, buffer, sizeof(uint8_t), s_cast<size_t>(size));
	SDL_RWclose(io);
	return buffer;
}

bool Content::loadByChunks(String filename, const std::function<bool(uint8_t*, int)>& handler) {
	if (filename.empty()) return false;
	std::string fullPath = Content::getFullPath(filename);
	SDL_RWops* io = SDL_RWFromFile(fullPath.c_str(), "rb");
	if (io == nullptr) {
		Error("failed to load file: \"{}\"", fullPath);
		return false;
	}
	uint8_t buffer[DORA_COPY_BUFFER_SIZE];
	int size = 0;
	while ((size = s_cast<int>(SDL_RWread(io, buffer, sizeof(uint8_t), DORA_COPY_BUFFER_SIZE)))) {
		if (handler(buffer, size)) {
			SDL_RWclose(io);
			return false;
		}
	}
	SDL_RWclose(io);
	return true;
}

bool Content::isPathFolder(String path) {
	return fs::is_directory(path.toString());
}
#endif // BX_PLATFORM_WINDOWS || BX_PLATFORM_OSX || BX_PLATFORM_IOS || BX_PLATFORM_LINUX

#if BX_PLATFORM_WINDOWS || BX_PLATFORM_ANDROID || BX_PLATFORM_LINUX
std::string Content::getFullPathForDirectoryAndFilename(String directory, String filename) {
	auto rootPath = fs::path(Content::isAbsolutePath(directory) ? Slice::Empty : _assetPath);
	std::string fullPath = (rootPath / directory.toString() / filename.toString()).string();
	if (!Content::isFileExist(fullPath)) {
		fullPath.clear();
	}
	return fullPath;
}
#endif // BX_PLATFORM_WINDOWS || BX_PLATFORM_ANDROID || BX_PLATFORM_LINUX

NS_DOROTHY_END
