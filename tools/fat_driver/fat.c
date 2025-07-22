#include "fat.h"

typedef struct {
    uint8_t  jmp[3];              // Jump instruction
    char     oem[8];              // OEM Name
    uint16_t bytes_per_sector;   // Bytes per sector
    uint8_t  sectors_per_cluster;// Sectors per cluster
    uint16_t reserved_sectors;   // Reserved sectors (usually 1 for FAT12)
    uint8_t  num_fats;           // Number of FATs
    uint16_t max_root_dir_entries; // Max root dir entries
    uint16_t total_sectors_short;  // Total sectors (if < 65536)
    uint8_t  media_descriptor;   // Media descriptor (e.g. 0x00 for floppy)
    uint16_t fat_size_sectors;   // Sectors per FAT
    uint16_t sectors_per_track;
    uint16_t num_heads;
    uint32_t hidden_sectors;
    uint32_t total_sectors_long;  // Total sectors (if >= 65536)

    // Extended boot record (not always used)
    uint8_t  drive_number;
    uint8_t  reserved1;
    uint8_t  boot_signature;
    uint32_t volume_id;
    char     volume_label[11];
    char     fs_type[8];
} __attribute__((packed)) FAT12_BootSector;

typedef struct {
    char     filename[11];       // Filename (padded with spaces)
    uint8_t  attributes;        // File attributes
    uint8_t  reserved;          // Reserved
    uint8_t  creation_time_tenths;
    uint16_t creation_time;
    uint16_t creation_date;
    uint16_t last_access_date;
    uint16_t high_starting_cluster; // Always 0 in FAT12
    uint16_t last_mod_time;
    uint16_t last_mod_date;
    uint16_t starting_cluster;  // First cluster
    uint32_t file_size;         // File size in bytes
} __attribute__((packed)) FAT12_DirectoryEntry;

FAT12_BootSector g_BootSector;
FAT12_DirectoryEntry* g_RootDirectory = NULL;
extern uint8_t* g_FAT_table;
extern uint8_t* disk_image;



uint16_t fat_get_next_cluster(uint16_t cluster){
    uint16_t offset = (cluster*3)/2;
    uint16_t next_cluster = 0;
     if (cluster % 2 == 0) {
        // Even cluster
        next_cluster =  ((g_FAT_table[offset + 1] & 0x0F) << 8)|g_FAT_table[offset];
    } else {
        // Odd cluster
        next_cluster = ((g_FAT_table[offset] & 0xF0) >> 4) | (g_FAT_table[offset + 1] << 4);
    }
    return next_cluster & 0x0FFF;
}
uint32_t fat_cluster_to_sector(uint16_t cluster) {
    return g_BootSector.reserved_sectors +
           (g_BootSector.fat_size_sectors * g_BootSector.num_fats) +
           ((g_BootSector.max_root_dir_entries * sizeof(FAT12_DirectoryEntry)) + (g_BootSector.bytes_per_sector - 1)) / g_BootSector.bytes_per_sector+
           (cluster - 2);
}
void format_filename_to_83(const char* input, char* output){
    int i = 0,j = 0;
    memset(output, ' ', 11);
    while(input[i]&&j<11){
        if(input[i] == '.'){
            j = 8;
            i++;
            continue;
        }
        output[j++] = toupper((unsigned char)input[i++]);
    }
}
FAT12_DirectoryEntry* fat_find_file(const char *filename){
    char f_output[11];
    format_filename_to_83(filename, f_output);
    for(uint32_t i = 0; i<g_BootSector.max_root_dir_entries; i++){
        if(memcmp(g_RootDirectory[i].filename, f_output, 11) == 0)
            return &g_RootDirectory[i];
    }
    return NULL;
}
void read_sectors(uint16_t* locations, uint16_t count, uint8_t* buffer, uint32_t file_size) {
    uint32_t bytes_read = 0;
    for (uint16_t i = 0; i < count && bytes_read < file_size; i++) {
        uint32_t sector = fat_cluster_to_sector(locations[i]);
        uint8_t* src = disk_image + sector * g_BootSector.bytes_per_sector;
        for (uint16_t j = 0; j < g_BootSector.bytes_per_sector && bytes_read < file_size; j++) {
            buffer[bytes_read++] = src[j];
        }
    }
}
int fat_read_file(const char* filename, uint8_t* buffer){
    FAT12_DirectoryEntry* file = fat_find_file(filename);
    if(!file) return -1;
    uint16_t t[2847];
    uint16_t i = 0;
    t[i++] = file->starting_cluster;
    while (1) {
        uint16_t next = fat_get_next_cluster(t[i - 1]);
        if (next >= 0xFF8) break;  // End of cluster chain
        t[i++] = next;
    }
    read_sectors(t, i,buffer, file->file_size);
    return file->file_size;
}
void fat_list_root_directory() {
    printf("Name\t\tSize\t\tStarting Cluster\n");
    for (uint16_t i = 0; i < g_BootSector.max_root_dir_entries; i++) {
        FAT12_DirectoryEntry* entry = &g_RootDirectory[i];
        if (entry->filename[0] == 0x00) break;
        if ((uint8_t)entry->filename[0] == 0xE5) continue;
        char name[9] = {0};
        char ext[4]  = {0};
        memcpy(name, entry->filename, 8);
        memcpy(ext,  entry->filename + 8, 3);
        for (int j = 7; j >= 0 && name[j] == ' '; j--) name[j] = '\0';
        for (int j = 2; j >= 0 && ext[j]  == ' '; j--) ext[j]  = '\0';
        if (ext[0] != '\0')
            printf("%s.%s\t\t%u\t\t%u\n", name, ext, entry->file_size, entry->starting_cluster);
        else
            printf("%s\t\t%u\t\t%u\n", name, entry->file_size, entry->starting_cluster);
    }
}
void fat_set_cluster(uint16_t cluster, uint16_t value) {
    uint32_t fat_offset = (cluster * 3) / 2;
    uint8_t* fat = disk_image + (g_BootSector.reserved_sectors * g_BootSector.bytes_per_sector);
    if (cluster % 2 == 0) {
        fat[fat_offset] = value & 0xFF;
        fat[fat_offset + 1] = (fat[fat_offset + 1] & 0xF0) | ((value >> 8) & 0x0F);
    } else {
        fat[fat_offset] = (fat[fat_offset] & 0x0F) | ((value << 4) & 0xF0);
        fat[fat_offset + 1] = (value >> 4) & 0xFF;
    }
}
void fat_free_cluster_chain(uint16_t start_cluster) {
    uint16_t current = start_cluster;
    while (current < 0xFF8) {
        uint16_t next = fat_get_next_cluster(current);
        fat_set_cluster(current, 0x000);  // Mark as free
        if (next >= 0xFF8) break;
        current = next;
    }
}
int fat_delete_file(const char* filename) {
    for (int i = 0; i < g_BootSector.max_root_dir_entries; i++) {
        FAT12_DirectoryEntry* entry = &g_RootDirectory[i];
        if (entry->filename[0] == 0x00)
            break;
        if ((entry->filename[0] != 0xE5) && strncmp(entry->filename, filename, 11) == 0) {
            fat_free_cluster_chain(entry->starting_cluster);
            entry->filename[0] = 0xE5; 
            return 0;
        }
    }
    return -1;
}

uint16_t fat_get_free_cluster(){
    for(uint16_t cluster = 2; cluster<4085; cluster++){
        uint16_t value;
        if (cluster & 1) {
            //odd cluster
            value = ((g_FAT_table[(3 * cluster) / 2] >> 4) | (g_FAT_table[(3 * cluster) / 2 + 1] << 4)) & 0x0FFF;
        } else {
            //even cluster
            value = (g_FAT_table[(3 * cluster) / 2] | ((g_FAT_table[(3 * cluster) / 2 + 1] & 0x0F) << 8)) & 0x0FFF;
        }
        if(value ==0x000){
            return cluster; //foujnd a free cluster
        }
        return 0xFFFF; //no free cluster you gotta pay 
    }
}
int fat_create_file(const char* filename, const uint8_t* data, uint32_t size) {
    //calculating needed clustss
    uint16_t needed_clusters = (size+g_BootSector.sectors_per_cluster*g_BootSector.bytes_per_sector-1)
                                /(g_BootSector.sectors_per_cluster*g_BootSector.bytes_per_sector);
    //finding freee clusters
    uint16_t free_clusters[needed_clusters];
    for(uint16_t i = 0; i<needed_clusters; i++){
        free_clusters[i] = fat_get_free_cluster();
        if(free_clusters[i] == 0xFFFF) {printf("Disk Full!!..\n");return -2;}
        //set FAT table
        if(i>0) fat_set_cluster(free_clusters[i-1], free_clusters[i]);
    }
    //set end of chain
    fat_set_cluster(free_clusters[needed_clusters-1], 0xFFF);
    // Write data to the diskk
    uint32_t data_written = 0;
    uint16_t i = 0;
    uint32_t cluster_size = g_BootSector.sectors_per_cluster * g_BootSector.bytes_per_sector;

    while (data_written < size) {
        // Get the address of the clustss starting sector in memory
        uint32_t offset = fat_cluster_to_sector(free_clusters[i++]) * g_BootSector.bytes_per_sector;
        uint8_t* places = disk_image + offset;

        // Write data into this clusterss byte by byte
        for (uint32_t j = 0; j < cluster_size && data_written < size; j++) {
            places[j] = data[data_written++];
        }
    }
    //find free root directory
    for(int i = 0; i<g_BootSector.max_root_dir_entries; i++){
        if(g_RootDirectory[i].filename[0] == 0x00 || g_RootDirectory[i].filename[0] == 0xE5){
            create_directory_entry(&g_RootDirectory[i], filename, free_clusters[0], size);
            return 0;
        }
    }
    return -1;
}
void create_directory_entry(FAT12_DirectoryEntry* entry, const char* name, uint16_t start_cluster, uint32_t size) {
    format_filename_to_83(name,entry->filename);

    entry->attributes = 0x20; // Archive
    entry->reserved = 0;
    entry->creation_time_tenths = 0;

    // Time/date: for now just dummy values
    entry->creation_time = 0;
    entry->creation_date = 0;
    entry->last_access_date = 0;

    ///////////////////////////////////////////
    entry->starting_cluster = start_cluster;
    entry->file_size = size;
}
uint16_t fat12_get_entry(uint16_t index) {
    uint32_t fat_offset = index + index / 2; // 1.5 * index
    uint8_t* fat = disk_image + (g_BootSector.reserved_sectors * g_BootSector.bytes_per_sector);

    uint16_t value;
    if (index % 2 == 0) {
        value = (fat[fat_offset] | ((fat[fat_offset + 1] & 0x0F) << 8));
    } else {
        value = ((fat[fat_offset] >> 4) | (fat[fat_offset + 1] << 4));
    }

    return value;
}
void print_fat_table() {
    printf("FAT Table:\n");
    uint16_t total_entries = (g_BootSector.fat_size_sectors * g_BootSector.bytes_per_sector * 8) / 12;
    // Cluster 0 and 1 are reserved
    for (uint16_t i = 2; i < total_entries; i++) { 
        uint16_t val = fat12_get_entry(i);
        if (val == 0x000)
            continue; // free cluster
        printf("Cluster %3u -> ", i);
        if (val >= 0xFF8)
            printf("EOF\n");
        else if (val == 0xFF7)
            printf("Bad Cluster\n");
        else
            printf("%3u\n", val);
    }
}