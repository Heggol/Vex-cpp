#include "utils.h"
#include <iostream>
#include <fstream>
#include <filesystem>
#include "archive.h"
#include "archive_entry.h"

namespace fs = std::filesystem;

void extractTarball(const std::string& buffer, const fs::path& extractPath) {
    std::cout << "DEBUG: Starting tarball extraction, buffer size: " << buffer.size() << " bytes" << std::endl;
    
    if (buffer.empty()) {
        std::cerr << "ERROR: Empty buffer provided for extraction" << std::endl;
        return;
    }

    struct archive *a = archive_read_new();
    if (!a) {
        std::cerr << "ERROR: Failed to create archive struct" << std::endl;
        return;
    }
    
    std::cout << "DEBUG: Setting up archive filters and formats" << std::endl;
    
    int r1, r2;
    r1 = archive_read_support_filter_gzip(a);
    if (r1 != ARCHIVE_OK) {
        std::cerr << "ERROR: Failed to enable gzip support: " << archive_error_string(a) << std::endl;
        archive_read_free(a);
        return;
    }
    
    r2 = archive_read_support_format_tar(a);
    if (r2 != ARCHIVE_OK) {
        std::cerr << "ERROR: Failed to enable tar format support: " << archive_error_string(a) << std::endl;
        archive_read_free(a);
        return;
    }

    std::cout << "DEBUG: Opening archive from memory" << std::endl;
    int r3 = archive_read_open_memory(a, buffer.c_str(), buffer.size());
    if (r3 != ARCHIVE_OK) {
        std::cerr << "ERROR: Could not open memory for archive: " << archive_error_string(a) << std::endl;
        archive_read_free(a);
        return;
    }

    std::cout << "DEBUG: Archive opened successfully" << std::endl;
    
    struct archive_entry *entry;
    fs::path rootPrefix;

    int entry_count = 0;
    int r4;
    
    while ((r4 = archive_read_next_header(a, &entry)) == ARCHIVE_OK) {
        entry_count++;
        const char* pathname = archive_entry_pathname(entry);
        if (!pathname) {
            std::cerr << "ERROR: No pathname for entry #" << entry_count << std::endl;
            continue;
        }
        
        fs::path entryPath(pathname);
        std::cout << "DEBUG: Processing entry: " << entryPath.string() << std::endl;
        
        if (rootPrefix.empty() && !entryPath.empty()) {
            rootPrefix = *entryPath.begin();
            std::cout << "DEBUG: Root prefix set to: " << rootPrefix.string() << std::endl;
        }

        fs::path relativePath;
        auto it = entryPath.begin();
        if (it != entryPath.end() && *it == rootPrefix) {
            it++;
            for (; it != entryPath.end(); ++it) {
                relativePath /= *it;
            }
        } else {
            relativePath = entryPath;
        }
        
        fs::path filePath = extractPath / relativePath;
        std::cout << "DEBUG: Extracting to: " << filePath.string() << std::endl;

        try {
            if (archive_entry_filetype(entry) == AE_IFDIR) {
                std::cout << "DEBUG: Creating directory: " << filePath.string() << std::endl;
                fs::create_directories(filePath);
            } else {
                std::cout << "DEBUG: Creating file: " << filePath.string() << std::endl;
                fs::create_directories(filePath.parent_path());
                std::ofstream out(filePath, std::ios::binary);
                if (!out) {
                    std::cerr << "ERROR: Failed to open output file: " << filePath.string() << std::endl;
                    continue;
                }
                
                const void *buff;
                size_t size;
                la_int64_t offset;
                int read_result;

                while ((read_result = archive_read_data_block(a, &buff, &size, &offset)) == ARCHIVE_OK) {
                    out.write(static_cast<const char*>(buff), size);
                    if (out.fail()) {
                        std::cerr << "ERROR: Failed to write to file: " << filePath.string() << std::endl;
                        break;
                    }
                }
                
                if (read_result != ARCHIVE_EOF && read_result != ARCHIVE_OK) {
                    std::cerr << "ERROR: Error reading data block: " << archive_error_string(a) << std::endl;
                }
                
                out.close();
            }
        } catch (const std::exception& e) {
            std::cerr << "ERROR: Exception during extraction of " << filePath.string() << ": " << e.what() << std::endl;
        }
        
        archive_read_data_skip(a);
    }
    
    if (r4 != ARCHIVE_EOF) {
        std::cerr << "ERROR: Error reading archive: " << archive_error_string(a) << std::endl;
    }
    
    std::cout << "DEBUG: Extraction complete, processed " << entry_count << " entries" << std::endl;
    archive_read_free(a);
}

