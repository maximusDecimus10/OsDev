#pragma once

#include "../bare-metal-c/libc.h"
#include "../bare-metal-c/console.h"
// ==============================
// FAT12 Filesystem API
// ==============================
//
// This header provides prototypes for core FAT12 filesystem operations,
// including file creation, deletion, directory management, and FAT table access.
//
// Assumes that disk_image, g_BootSector, and g_RootDirectory are defined globally elsewhere.
//

// ======================================================
// Gets the next cluster in a FAT12 cluster chain
// Params:
//    cluster - current cluster number
// Returns:
//    next cluster in the chain, or 0xFFF (end of file)
// ======================================================
uint16_t fat_get_next_cluster(uint16_t cluster);

// ======================================================
// Converts a FAT cluster number to its corresponding sector
// Params:
//    cluster - cluster number to convert
// Returns:
//    sector number (absolute, starting from disk)
// ======================================================
uint32_t fat_cluster_to_sector(uint16_t cluster);

// ======================================================
// Converts a long filename into FAT 8.3 format
// Params:
//    input  - original filename (e.g., "readme.txt")
//    output - destination buffer for 8.3 name (must be 11 bytes)
// ======================================================
void format_filename_to_83(const char* input, char* output);

// ======================================================
// Searches for a file in the root directory
// Params:
//    filename - name of file in 8.3 format or regular string
// Returns:
//    Pointer to FAT12_DirectoryEntry if found, NULL otherwise
// ======================================================
FAT12_DirectoryEntry* fat_find_file(const char *filename);

// ======================================================
// Reads multiple sectors (non-contiguous clusters) into a buffer
// Params:
//    locations  - array of cluster numbers
//    count      - number of clusters
//    buffer     - destination buffer
//    file_size  - size of the file in bytes
// ======================================================
void read_sectors(uint16_t* locations, uint16_t count, uint8_t* buffer, uint32_t file_size);

// ======================================================
// Reads a file's contents into a buffer
// Params:
//    filename - name of the file to read
//    buffer   - destination buffer (should be large enough)
// Returns:
//    0 on success, -1 on failure
// ======================================================
int fat_read_file(const char* filename, uint8_t* buffer);

// ======================================================
// Prints all entries in the root directory
// Displays filenames and sizes
// ======================================================
void fat_list_root_directory();

// ======================================================
// Writes a value to a specific FAT table entry
// Used to link or unlink clusters
// Params:
//    cluster - index in the FAT
//    value   - new value (e.g., next cluster or 0xFFF for EOF)
// ======================================================
void fat_set_cluster(uint16_t cluster, uint16_t value);

// ======================================================
// Frees a cluster chain starting from the given cluster
// Used when deleting a file
// Params:
//    start_cluster - first cluster of the file
// ======================================================
void fat_free_cluster_chain(uint16_t start_cluster);

// ======================================================
// Deletes a file from the root directory and frees its clusters
// Params:
//    filename - name of file to delete
// Returns:
//    0 on success, -1 on failure
// ======================================================
int fat_delete_file(const char* filename);

// ======================================================
// Finds the next available (free) cluster in the FAT
// Returns:
//    cluster number, or 0xFFFF if disk is full
// ======================================================
uint16_t fat_get_free_cluster();

// ======================================================
// Creates a new file and writes its contents to disk
// Params:
//    filename - name of the new file
//    data     - pointer to file content
//    size     - size in bytes
// Returns:
//    0 on success, -1 on failure
// ======================================================
int fat_create_file(const char* filename, const uint8_t* data, uint32_t size);

// ======================================================
// Fills a FAT directory entry structure with metadata
// Params:
//    entry         - pointer to an empty directory entry
//    name          - file name (regular format)
//    start_cluster - first cluster number for the file
//    size          - size in bytes
// ======================================================
void create_directory_entry(FAT12_DirectoryEntry* entry, const char* name, uint16_t start_cluster, uint32_t size);

// ======================================================
// Prints the entire FAT12 table with cluster mappings
// Shows which cluster links to which (or EOF / bad cluster)
// Useful for debugging
// ======================================================
void print_fat_table();

