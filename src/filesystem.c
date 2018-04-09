#include <filesystem.h>
#include <assert.h>
#include <stdlib.h>
#include <io.h>
#include <string.h>
#include <file.h>

//To have commands not go through after close the file if statement will be made for ex:
/*
if(fclose(this->_file))
    {
        perror("File system image must be opened first");
        return;
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

//G:

void filesystem_close(filesystem_t* this)
{   
    fclose( this->_file);
}

void info(filesystem_t* this)
{
    bootsector_t boot = this->_boot;

    printf( "\nBPB_BytesPerSector (base 10) = %d, (hexadecimal) = %x \n", boot.bytes_per_sector, boot.bytes_per_sector );

    printf( "\nBPB_SectorsPerClus (base 10) = %d, (hexadecimal) = %x\n", boot.sectors_per_cluster, boot.sectors_per_cluster );

    printf( "\nBPB_RsvdSecCnt (base 10) = %d, (hexadecimal) = %x\n", boot.reserved_sector_count, boot.reserved_sector_count );

    printf( "\nBPB_NumFATS (base 10) = %d, (hexadecimal) = %x\n", boot.fat_count, boot.fat_count );

    printf( "\nBPB_FATSz32 (base 10) = %d, (hexadecimal) = %x\n", boot.fat_sector_count, boot.fat_sector_count );

}

//Go back to fix just reading for the first cluster
void stat(filesystem_t* this, const char* file_name)
{
    directory_t dir = filesystem_list( this );

    unsigned int i;
    for ( i = 0u; i < dir.count; i++ )
    {
        file_t file = dir.files[ i ];

        if ( strcmp( file.name, file_name ) == 0 )
        {
            printf("\nAttributes: ");
            if ( file.attrs.read_only ) 
            {
                printf("read only\n");
            }
            if( file.attrs.hidden)
            {
                printf("hidden\n");
            }
            if(file.attrs.system)
            {
                printf("hidden\n");
            }
            if(file.attrs.volume_id)
            {
                printf("volume id\n");
            }
            if(file.attrs.archive)
            {
                printf("archive\n");
            }
            
            if(file.attrs.directory)
            {
                printf("directory\n");
                printf("Size: 0\n"); 
            }
            else
            {
                printf("Size: %d\n", file.size);
            }

            printf("Starting Cluster Number: %d\n", file.cluster_low);
             /*file.attrs*/
            return;
        }

    }

    printf("\nError: File not found\n");

    free( dir.files );
    
}


//void get(filesystem_t* this, const char* file_name)
//{    //I don't think this is correct, need to start over
    /*if(this->cwd && file_name)
    {
        directory_t dir = filesystem_list( this );

        unsigned int i;
        for ( i = 0u; i < dir.count; i++ )
        {
            file_t file = dir.files[ i ];

            if ( file.attrs.directory )
            {
                if ( strcmp( file.name, file_name ) == 0)
                {
                    file.
                }
            }
        }
        free( dir.files );

    }
    else
    {
        printf("File not found");
        return;
    }
    */
//}

/*
void read(filesystem_t* this, int postion, int numOfBytes)
{

}
*/

