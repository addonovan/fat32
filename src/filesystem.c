#include <filesystem.h>
#include <assert.h>
#include <stdlib.h>
#include <io.h>
#include <string.h>
#include <file.h>

//To have commands not go through after close the file if statement will be made for ex:
/*
if(filesytem_close(...)
{
    perror("File system image must be opened first")
    return false;
}
*/

bool filesystem_init( filesystem_t* this, const char* file_name )
{
    // open the image file for reading
    this->_file = fopen( file_name, "r" );
    if ( this->_file == NULL )
    {
        fprintf( stderr, "Failed to open '%s' for read\n", file_name );
        perror( "fopen()" );
        return false;
    }

    // load the bootsystem/fat32 metadata
    if ( !bootsector_init( &this->_boot, this->_file ) )
    {
        fprintf( stderr, "Failed to read fat32 bootsector!\n" );
        fprintf( stderr, "Are you sure this is a valid fat32 image?\n" );
        return false;
    }

    // calculate some useful sizes we'll need in a bit
    offset_t bps            = this->_boot.bytes_per_sector;
    offset_t size_reserved  = this->_boot.reserved_sector_count * bps;
    offset_t size_fat       = this->_boot.fat_sector_count * bps;

    // the first FAT starts immediately after the reserved section
    this->fat = size_reserved;

    // the fs clusters start after the (identical) FATs
    this->clusters = this->fat + ( size_fat * this->_boot.fat_count );

    // we start off in the root directory
    this->cwd = 2; // for some reason 2 is actually the root directory???

    return true;
}

directory_t filesystem_list( filesystem_t* this )
{
    // how many entries are inside a single cluster?
    uint32_t max_files = this->_boot.bytes_per_sector * this->_boot.sectors_per_cluster;
    max_files /= RAW_FILE_SIZE;

    int allocated = max_files;  // the amount of file_t's allocated in a list
    int count = 0;              // the number of displayable files we've discovered

    // allocate space for 1 cluster's-worth of files
    file_t* files = malloc( sizeof( file_t ) * allocated );

    // start off in the current working directory
    cluster_t cluster = this->cwd;
    do
    {
        // convert to a file offset
        offset_t position = cluster_to_offset( this, cluster );

        unsigned int i;
        for ( i = 0u; i < max_files; i++ )
        {
            file_init( files + count, this, position );

            if ( file_displayable( files + count ) )
            {
                count++;

                // if we need to allocate more space, let's just do that
                // real fast and allocate double the space
                if ( count == allocated )
                {
                    allocated *= 2;
                    files = realloc( files, sizeof( file_t ) * allocated );
                }
            }

            // try to read the next position
            position += RAW_FILE_SIZE;
        }
    }
    // continue until we run out of clusters for this directory listing
    while ( io_next_cluster( this, &cluster ) );

    // create the directory listing
    directory_t dir;
    dir.count = count;
    dir.files = files;

    return dir;
}

bool filesystem_cd( filesystem_t* this, const char* dir_name )
{
    directory_t dir = filesystem_list( this );

    unsigned int i;
    for ( i = 0u; i < dir.count; i++ )
    {
        file_t file = dir.files[ i ];

        // disregard anything that isn't a directory
        if ( !file.attrs.directory )
            continue;

        // compare the names
        if ( strcmp( dir_name, file.name ) != 0 )
            continue;

        // hey, they matched!
        // let's change the directory to that file's offset, and we're done!
        this->cwd = file.cluster_low;
        free( dir.files );
        return true;
    }

    free( dir.files );
    return false;
}

bool filesystem_close( filesystem_t* this, const char* file_name)
{
    //close the image file
    this->_file = fclose( file_name);
    
    //file can not close if not at opened first 
    if(!fopen(file_name, "r"));
    {
        perror("File system not open");
        return false;
    }

    return true;
}


