/*
 * wav2vag
 * 
 * Converts a WAV file to a PlayStation VAG file.
 * Based on PSX VAG-Packer 0.1 by bITmASTER.
 *       
 */

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#include "endian.c"

#define BUFFER_SIZE 128*28

short wave[BUFFER_SIZE];

void find_predict( short *samples, double *d_samples, int *predict_nr, int *shift_factor );
void pack( double *d_samples, short *four_bit, int predict_nr, int shift_factor );
void fputi( int d, FILE *fp );

int main( int argc, char *argv[] )
{
    FILE *fp, *vag;
    short *ptr;
    double d_samples[28];
    short four_bit[28];
    int predict_nr;
    int shift_factor;
    int flags;
    int size;
    int i, j, k;    
    unsigned char d;
    char s[4];
    int chunk_data;
    short e;
    int sample_freq = 0, sample_len;
    char internal_name[16];
	int enable_looping = 0;
	int raw_output = 0;
	int sraw = 0;
    short sample_size;
    unsigned char c;
	
    if (argc < 3)
    {
        printf("wav2vag - Convert a WAV file to a PlayStation VAG sound file\n");
	printf("usage: wav2vag [wav] [vag] <options>\n");
	printf("\n");
	printf("WAV files must have one channel (mono)\n");
	printf("WAV data format must be either unsigned 8-bit or signed 16-bit PCM\n");
	printf("\n");
	printf("Options:\n");
	printf("   -L           - Make a looping sample (when it ends it is played again)\n");
	printf("   -name=<name> - Set sample name\n");
	printf("   -raw         - Output only data, without VAG header\n");
	printf("   -sraw8       - Source is RAW data, in 8-bit format\n");
	printf("   -sraw16      - Source is RAW data, in 16-bit format\n");
	printf("   -freq=<freq> - Force frequency in output VAG\n");
	printf("\n");
	printf("This utility is based on PSX VAG-Packer by bITmASTER\n");
        return -1;
    }
    
    for(i = 0; i < sizeof(internal_name); i++)
	internal_name[i] = 0;
    
    strcpy(internal_name, "PSXSDK");
	
    fp = fopen(argv[1], "rb");
    if (fp == NULL)
    {
        printf("Can�t open %s. Aborting.\n", argv[1]);
        return -2;
    }
    
    for(i = 3; i < argc; i++)
	{
		if(strcmp(argv[i], "-L") == 0)
			enable_looping = 1;
		
		if(strncmp(argv[i], "-name=",6) == 0)
			strncpy(internal_name, argv[i]+6, 15);
		
		if(strcmp(argv[i], "-raw") == 0)
			raw_output = 1;
		
		if(strcmp(argv[i], "-sraw8") == 0)
		{
			fseek(fp, 0, SEEK_END);
			sample_len = ftell(fp);
			fseek(fp, 0, SEEK_SET);
			sample_size = 8;
			sample_freq = 44010;
			sraw = 1;
		}
		
		if(strcmp(argv[i], "-sraw16") == 0)
		{
			fseek(fp, 0, SEEK_END);
			sample_len = ftell(fp) / 2;
			fseek(fp, 0, SEEK_SET);
			sample_size = 16;
			sample_freq = 44010;
			sraw = 1;
		}
		
		if(strncmp(argv[i], "-freq=", 6) == 0)
		{
			sscanf(argv[i], "-freq=%d", &sample_freq);
		}
	}
    

    if(sraw == 1)
	    goto convert_to_vag;

    fread(s, 1, 4, fp);
    if (strncmp(s, "RIFF", 4))
    {
        printf("%s is not in WAV format\n", argv[1]);
        return -3;
    }

    fseek(fp, 8, SEEK_SET);
    fread(s, 1, 4, fp);
    
    if (strncmp(s, "WAVE", 4))
    {
        printf("%s is not in WAV format\n", argv[1]);
        return -3;
    }

    fseek(fp, 8 + 4, SEEK_SET);
    fread(s, 1, 4, fp);
    
    if (strncmp(s, "fmt", 3)) 
    {
        printf("%s is not in WAV format\n", argv[1]);
        return -3;
    }
    
   // fread(&chunk_data, sizeof(int), 1, fp); 
    chunk_data = read_le_dword(fp);
    chunk_data += ftell(fp);
    
  //  fread(&e, 2, 1, fp);
     e = read_le_word(fp);
    
    if (e!=1)
    {
        printf("No PCM found in %s. Aborting.\n", argv[1]);
        return -4;
    }   

//    fread(&e, 2, 1, fp);
    e = read_le_word(fp);
    
    if (e!=1)
    {
        //printf( "must be MONO\n" );
	printf("WAV file must have only one channel. Aborting.\n");
        return -5;
    }

    if(sample_freq != 0)
	fseek(fp, 4, SEEK_CUR);
    else
	//fread(&sample_freq, 4, 1, fp);
        sample_freq = read_le_dword(fp);
    
    fseek(fp, 4 + 2, SEEK_CUR);

//    fread(&sample_size, 2, 1, fp);
    sample_size = read_le_word(fp);
    
 /*   if (e!=16)
    {
        //printf( "only 16 bit samples\n" );
	printf("The size of the samples of the WAV file must be 16-bit."
	       "Aborting.\n");
        return -6;
    }*/
        
    fseek(fp, chunk_data, SEEK_SET);
    
    fread(s, 1, 4, fp);
    
    if (strncmp(s, "data", 4))
    {
        printf("No data chunk in %s. Aborting.\n", argv[1]);
        return -7;
    }

    // fread(&sample_len, 4, 1, fp);
    sample_len = read_le_dword(fp);
    
    if(sample_size == 16)
	sample_len /= 2;

    /*strcpy( fname, argv[1] );
    p = strrchr( fname, '.' );
    p++;
    strcpy( p, "vag" );*/
convert_to_vag:    
    vag = fopen(argv[2], "wb");
    
    if (vag == NULL)
    {
        printf("Can't open output file. Aborting.\n");
        return -8;
    }

/*	strcpy(internal_name, "PSXSDK");
	
    	for(i = 3; i < argc; i++)
	{
		if(strcmp(argv[i], "-L") == 0)
			enable_looping = 1;
		
		if(strncmp(argv[i], "-name=",6) == 0)
			strncpy(internal_name, argv[i]+6, 15);
		
		if(strcmp(argv[i], "-raw") == 0)
			raw_output = 1;
	}
*/
    
	if(raw_output == 0)
	{
		fprintf( vag, "VAGp" );             // ID
		fputi( 0x20, vag );                 // Version
		fputi( 0x00, vag );                 // Reserved
		size = sample_len / 28;
		if( sample_len % 28 )
			size++;
		fputi( 16 * ( size + 2 ), vag );    // Data size
		fputi( sample_freq, vag );          // Sampling frequency
    
		for ( i = 0; i < 12; i++ )          // Reserved
			fputc( 0, vag );

		fwrite(internal_name, sizeof(char), 16, vag);
    
		for( i = 0; i < 16; i++ )
			fputc( 0, vag );                // ???
	}
		
	if(enable_looping)
		flags = 6;
	else
		flags = 0;  
	
    while( sample_len > 0 ) {
        size = ( sample_len >= BUFFER_SIZE ) ? BUFFER_SIZE : sample_len; 
	    
	if(sample_size == 8)
	{
		for(i = 0; i < size; i++)
		{
			c = fgetc(fp);
			wave[i] = c;
			wave[i] ^= 0x80;
			wave[i] <<= 8;
		}
	}
	else
	{
		// fread( wave, sizeof( short ), size, fp );
		for(i = 0; i < size; i++)
			wave[i] = read_le_word(fp);
	}
	
        i = size / 28;
        if ( size % 28 ) {
            for ( j = size % 28; j < 28; j++ )
                wave[28*i+j] = 0;
            i++;
        }
        
        for ( j = 0; j < i; j++ ) {                                     // pack 28 samples
            ptr = wave + j * 28;
            find_predict( ptr, d_samples, &predict_nr, &shift_factor );
            pack( d_samples, four_bit, predict_nr, shift_factor );
            d = ( predict_nr << 4 ) | shift_factor;
            fputc( d, vag );
            fputc( flags, vag );
            for ( k = 0; k < 28; k += 2 ) {
                d = ( ( four_bit[k+1] >> 8 ) & 0xf0 ) | ( ( four_bit[k] >> 12 ) & 0xf );
                fputc( d, vag );
            }
            sample_len -= 28;
            if ( sample_len < 28 && enable_looping == 0)
                flags = 1;
	    
	    if(enable_looping)
		flags = 2;
        }
    }
    
    fputc( ( predict_nr << 4 ) | shift_factor, vag );
    
    if(enable_looping)
	    fputc(3, vag);
    else
	fputc( 7, vag );            // end flag
    
    for ( i = 0; i < 14; i++ )
        fputc( 0, vag );
    
    fclose( fp );
    fclose( vag );  
//    getch(); 
    return( 0 );
}


static double f[5][2] = { { 0.0, 0.0 },
                            {  -60.0 / 64.0, 0.0 },
                            { -115.0 / 64.0, 52.0 / 64.0 },
                            {  -98.0 / 64.0, 55.0 / 64.0 },
                            { -122.0 / 64.0, 60.0 / 64.0 } };
                  


void find_predict( short *samples, double *d_samples, int *predict_nr, int *shift_factor )
{
    int i, j;
    double buffer[28][5];
    double min = 1e10;
    double max[5];
    double ds;
    int min2;
    int shift_mask;
    static double _s_1 = 0.0;                            // s[t-1]
    static double _s_2 = 0.0;                            // s[t-2]
    double s_0, s_1, s_2;

    for ( i = 0; i < 5; i++ ) {
        max[i] = 0.0;
        s_1 = _s_1;
        s_2 = _s_2;
        for ( j = 0; j < 28; j ++ ) {
            s_0 = (double) samples[j];                      // s[t-0]
            if ( s_0 > 30719.0 )
                s_0 = 30719.0;
            if ( s_0 < - 30720.0 )
                s_0 = -30720.0;
            ds = s_0 + s_1 * f[i][0] + s_2 * f[i][1];
            buffer[j][i] = ds;
            if ( fabs( ds ) > max[i] )
                max[i] = fabs( ds );
//                printf( "%+5.2f\n", s2 );
                s_2 = s_1;                                  // new s[t-2]
                s_1 = s_0;                                  // new s[t-1]
        }
        
        if ( max[i] < min ) {
            min = max[i];
            *predict_nr = i;
        }
        if ( min <= 7 ) {
            *predict_nr = 0;
            break;
        }
        
    }

// store s[t-2] and s[t-1] in a static variable
// these than used in the next function call

    _s_1 = s_1;
    _s_2 = s_2;
    
    for ( i = 0; i < 28; i++ )
        d_samples[i] = buffer[i][*predict_nr];

//  if ( min > 32767.0 )
//      min = 32767.0;
        
    min2 = ( int ) min;
    shift_mask = 0x4000;
    *shift_factor = 0;
    
    while( *shift_factor < 12 ) {
        if ( shift_mask  & ( min2 + ( shift_mask >> 3 ) ) )
            break;
        (*shift_factor)++;
        shift_mask = shift_mask >> 1;
    }
      
}

void pack( double *d_samples, short *four_bit, int predict_nr, int shift_factor )
{
    double ds;
    int di;
    double s_0;
    static double s_1 = 0.0;
    static double s_2 = 0.0;
    int i;

    for ( i = 0; i < 28; i++ ) {
        s_0 = d_samples[i] + s_1 * f[predict_nr][0] + s_2 * f[predict_nr][1];
        ds = s_0 * (double) ( 1 << shift_factor );

        di = ( (int) ds + 0x800 ) & 0xfffff000;

        if ( di > 32767 )
            di = 32767;
        if ( di < -32768 )
            di = -32768;
            
        four_bit[i] = (short) di;

        di = di >> shift_factor;
        s_2 = s_1;
        s_1 = (double) di - s_0;

    }
}

void fputi( int d, FILE *fp )
{
    fputc( d >> 24, fp );
    fputc( d >> 16, fp );   
    fputc( d >> 8,  fp );
    fputc( d,       fp );
}
