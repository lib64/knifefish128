#include "kf128.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <getopt.h>

#ifdef __linux__
#include <termios.h>
#elif _WIN32
#include <windows.h>
#include <ntsecapi.h>
#endif

#define MAX_ARGS 20

#define MAX_FILE_PATH 128
#define MAX_PASS 256
#define IV_SIZE 16

#ifdef __linux__

struct termios saved_attributes;

void reset_input_mode(void)
{
    tcsetattr (0, TCSANOW, &saved_attributes);
}

void set_input_mode(void)
{
    struct termios tattr;
    tcgetattr (0, &saved_attributes);
    tcgetattr (0, &tattr);
    tattr.c_lflag &= ~(ICANON | ECHO);
    tattr.c_cc[VMIN] = 1;
    tattr.c_cc[VTIME] = 0;
    tcsetattr (0, TCSAFLUSH, &tattr);
}

#endif

void usage(void)
{
    printf("Usage: ./kf128 [options]\n\n");
    printf("-e\t--encrypt \t-Encrypt mode.\n");
    printf("-d\t--decrypt \t-Decrypt mode.\n");
    printf("-i\t--input   \t-Input file.\n");
    printf("-o\t--output  \t-Output file.\n");
    printf("-p\t--pass    \t-The passphrase.\n");
    printf("-k\t--iv      \t-The initialization vector.\n");
    printf("-h\t--help    \t-Show help.\n");
    printf("\n");
}

int main(int argc, char **argv)
{

    int help_flag = 0;
    int encrypt_flag = 0;
    int decrypt_flag = 0;
    int input_flag = 0;
    int output_flag = 0;
    int passphrase_flag = 0;
    int iv_flag = 0;

    char input[MAX_FILE_PATH+1], output[MAX_FILE_PATH];
    char pass[MAX_PASS+1] = {0};
    char pass2[MAX_PASS+1] = {0};

    char iv[IV_SIZE+1] = {0};
    char padding[IV_SIZE+1] = {0};

    int c;

    while (1)
    {
        static struct option long_options[] = {
            {"help",     no_argument, 0, 'h'},
            {"encrypt",  no_argument, 0, 'e'},
            {"decrypt",  no_argument, 0, 'd'},
            {"input",    required_argument, 0, 'i'},
            {"output",   required_argument, 0, 'o'},
            {"pass",   required_argument, 0, 'p'},
            {"iv",   required_argument, 0, 'k'},

            {0, 0, 0, 0}
        };

        int option_index = 0;

        c = getopt_long (argc, argv, "hedi:o:p:k:",
                         long_options, &option_index);

        if (c == -1)
            break;

        switch (c)
        {

        case 'h':
            help_flag = 1;
            break;

        case 'e':
            encrypt_flag = 1;
            break;

        case 'd':
            decrypt_flag = 1;
            break;

        case 'i':
            input_flag = 1;
            strncpy(input, optarg, MAX_FILE_PATH);
            break;

        case 'o':
            output_flag = 1;
            strncpy(output, optarg, MAX_FILE_PATH);
            break;

        case 'p':
            passphrase_flag = 1;
            strncpy(pass, optarg, MAX_PASS);
            break;

        case 'k':
            iv_flag = 1;
            strncpy(iv, optarg, IV_SIZE);
            break;

        case '?':
            break;

        default:
            abort ();
        }
    }

    if(help_flag)
    {
        usage();
        return 0;
    }

    if(encrypt_flag && decrypt_flag)
    {
        printf("Error: -e and -d are incompatible flags.\n");
        return 0;
    }

    if(encrypt_flag || decrypt_flag)
    {
        if(!input_flag)
        {
            printf("Error: must specify an input file.\n");
            return 0;
        }
        if(!output_flag)
        {
            printf("Error: must specify an output file.\n");
            return 0;
        }

        if(!passphrase_flag)
        {
            int first = 1;
            do
            {
                if(!first)
                {
                    printf("Error: passwords do not match.\n");
                }
               
                printf("password:");
#ifdef __linux__
                set_input_mode();                
#elif _WIN32
                HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
                DWORD mode = 0;
                GetConsoleMode(hStdin, &mode);
                SetConsoleMode(hStdin, mode & (~ENABLE_ECHO_INPUT));                
 #endif    
                fgets(pass,MAX_PASS,stdin);
                printf("\nrepeat:");
                fgets(pass2,MAX_PASS,stdin);
#ifdef __linux__
				reset_input_mode();
#endif
                printf("\n");
                
                first = 0;
            }
            while(memcmp(pass,pass2,MAX_PASS) != 0);
        }

        if(iv_flag)
        {
            if(decrypt_flag)
            {
                printf("Error: no iv needs to be set when decrypting\n");
                return 0;
            }
            if(strlen(iv) != 16)
            {
                printf("Error: iv must be 16 characters.\n");
                return 0;
            }
        }
        else
        {
            if(encrypt_flag)
            {		
#ifdef __linux__
                FILE *in = fopen("/dev/urandom","r");
                if(!in)
                {
                    printf("Error - Cant generate an iv with /dev/urandom.\nExiting.\n");
                }
                fread(iv,sizeof (char), 16, in);
                fread(padding,sizeof (char), 16, in);
                fclose(in);
#elif _WIN32
                if(!RtlGenRandom(iv,16))
                {
                    printf("Error - Cant generate an iv with RtlGenRandom.\nExiting.\n");
                    return 0;
                }
#endif
            }
        }
		
        FILE *test = fopen(input,"r");
        if( !test )
        {
            printf("Error: cant open input file: %s\n",input);
            return 0;
        }
        fclose(test);

        if(encrypt_flag)
        {
            printf("Encrypting %s\n",input);
            kf_encrypt_file_cbc(input,output,pass,iv,padding);
        }

        if(decrypt_flag)
        {
            printf("Decrypting %s\n",input);
            kf_decrypt_file_cbc(input,output,pass);
        }
        printf("Finished.\n");
    }
    else
    {
        usage();
    }

    return 0;
}
