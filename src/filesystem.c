#include <filesystem.h>
#include <assert.h>
#include <stdlib.h>
#include <io.h>
#include <string.h>
#include <file.h>

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
    this->cwd = this->_boot.root_cluster; // for some reason 2 is actually the root directory???

    this->dir = calloc( sizeof( char ), 2 );
    this->dir[ 0 ] = '/';

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

        // compare the names
        if ( !file_name( &file, dir_name ) )
            continue;

        if ( !file.attrs.directory )
            continue;

        // hey, they matched!
        // let's change the directory to that file's offset, and we're done!
        this->cwd = file.cluster_low;
        free( dir.files );

        // oh boy, gotta love this hack
        if ( this->cwd == 0 )
            this->cwd = this->_boot.root_cluster;

        if ( strcmp( file.name, ".." ) == 0 )
        {
            // cut the dir at the last slash
            int j = strlen( this->dir ) - 1;
            this->dir[ j ] = '\0';
            for ( j = j - 1; j >= 0 && this->dir[ j ] != '/'; j-- )
            {
                this->dir[ j ] = '\0';
            }
        }
        else if ( strcmp( file.name, "." ) == 0 )
        {
            // don't do anything to the path
        }
        else
        {
            int dir_length = strlen( this->dir );
            int name_length = strlen( file.name );
            this->dir = realloc( this->dir, dir_length + name_length + 2 );
            memcpy( this->dir + dir_length, file.name, name_length );
            this->dir[ dir_length + name_length + 0 ] = '/';
            this->dir[ dir_length + name_length + 1 ] = '\0';
        }

        return true;
    }

    free( dir.files );
    return false;
}

//G:

void filesystem_close(filesystem_t* this)
{   
    fclose( this->_file );
    free( this->dir );
}

void filesystem_info(filesystem_t* this)
{
    bootsector_t boot = this->_boot;

    printf( "\nBPB_BytesPerSector (base 10) = %d, (hexadecimal) = %x \n", boot.bytes_per_sector, boot.bytes_per_sector );

    printf( "\nBPB_SectorsPerClus (base 10) = %d, (hexadecimal) = %x\n", boot.sectors_per_cluster, boot.sectors_per_cluster );

    printf( "\nBPB_RsvdSecCnt (base 10) = %d, (hexadecimal) = %x\n", boot.reserved_sector_count, boot.reserved_sector_count );

    printf( "\nBPB_NumFATS (base 10) = %d, (hexadecimal) = %x\n", boot.fat_count, boot.fat_count );

    printf( "\nBPB_FATSz32 (base 10) = %d, (hexadecimal) = %x\n", boot.fat_sector_count, boot.fat_sector_count );

}

//Go back to fix just reading for the first cluster
void filesystem_stat( filesystem_t* this, const char* name )
{
    directory_t dir = filesystem_list( this );

    unsigned int i;
    for ( i = 0u; i < dir.count; i++ )
    {
        file_t file = dir.files[ i ];

        if ( file_name( &file, name ) )
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

void filesystem_get( filesystem_t* this, const char* file_name )
{
    FILE *out = fopen(file_name, "w+");   
    filesystem_read(this, 0, -1, file_name, out);
    fclose(out);
}

void filesystem_read(filesystem_t* this, int startOffset, int numOfBytes, const char* name, FILE* out)
{
    directory_t dir = filesystem_list( this );
    unsigned int i;
    for ( i = 0u; i < dir.count; i++ )
    {
        file_t file = dir.files[ i ];

        if ( file_name( &file, name ) )
        {
            int size = file.size;
            size /= this->_boot.sectors_per_cluster*this->_boot.bytes_per_sector;
            char* buffer = io_clalloc(this, (size+1));
            file_read(&file, buffer);

            char*buffer2;

            if( numOfBytes >= 0)
            {
                buffer2 = malloc(numOfBytes);
            }
            else
            {
                buffer2 = malloc(file.size);
                numOfBytes = file.size;
            }
            memcpy(buffer2, buffer+startOffset, numOfBytes);
            fprintf(out, "%s", buffer2);
            free(buffer2);

            io_free(buffer);
        }
    }
}
