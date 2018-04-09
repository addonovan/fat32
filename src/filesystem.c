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

bool filesystem_close( filesystem_t* this, const char* file_name)
{   
    //info command complete structure wise, thinking to store it into a struct in main? I am not sure yet
    //file can not close if not at opened first 
    if(!filesystem_init(this, file_name))
    {
        perror("File system not open");
        return false;
    }
    else
    {
        //close the image file
        fclose( this->_file);
        return true;
    }
}

void infoSec(filesystem_t* this, bootsector_t boot)
{
    if(fclose(this->_file))
    {
        perror("File system image must be opened first");
        return;
    }

    printf( "\nBPB_BytesPerSector (base 10) = %d\n", boot.bytes_per_sector );
    deciToHex(boot.bytes_per_sector);

    printf( "\nBPB_SectorsPerClus = %d\n", boot.sectors_per_cluster );
    deciToHex(boot.sectors_per_cluster);

    printf( "\nBPB_RsvdSecCnt = %d\n", boot.reserved_sector_count );
    deciToHex(boot.reserved_sector_count);

    printf( "\nBPB_NumFATS = %d\n", boot.fat_count );
    deciToHex(boot.fat_count);

    printf( "\nBPB_FATSz32 = %d\n", boot.fat_sector_count );
    deciToHex(boot.fat_sector_count);

}

//possible to do a different algorithm for it
void deciToHex(int num)
{
    int temp = 0;
    char hexadecimal[50];

    int i = 0;

    int a;

    while(num != 0)
    {
        temp = num%16;

        if(temp < 10)
        {
            hexadecimal[i++] = 48 + temp;
        }
        else
        {
            hexadecimal[i++] = 55 + temp;
        }

        num = num/16;
    }

    printf("\n(hexadecimal) = ");
    for(a = i; a >= 0; a--)
    {
        printf("%c", hexadecimal[a]);
    }

}




