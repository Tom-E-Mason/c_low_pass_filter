
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

#define WINDOW_TYPE_KAISSER "kaisser"         // ------------
#define WINDOW_TYPE_HAMMING "hamming"         // 
#define WINDOW_TYPE_HANNING "hanning"         // window type
#define WINDOW_TYPE_BLACKMAN "blackman"       // id codes
#define WINDOW_TYPE_BARTLETT "bartlett"       // 
#define WINDOW_TYPE_RECTANGULAR "rectangular" // ------------

#define eprintf(...) fprintf(stderr, __VA_ARGS__)

enum errors {                 // exit status codes
    NO_ERROR,                 // program runs successfully
    COMMAND_LINE_ARGS_ERROR,  // too many inputs from user
    CUTOFF_VALUE_ERROR,       // invalid cutoff value
    FILTER_FILE_ERROR,        // error in filter function
    INPUT_FILE_OPEN_ERROR,    // input file doesn't exist
    INPUT_FILE_FORMAT_ERROR,  // .wav extension missing on input file name
    OUTPUT_FILE_FORMAT_ERROR, // .wav extension missing on output file name
    UNKNOWN_WINDOW_ERROR,
};

void print_usage(const char* prog_name);
void print_manual_page(const char* prog_name);
bool is_wav_file(const char* file_name);
float get_cutoff(const char* file_name);
const char* get_window_type(const char* window_type);

int main(int argc, const char** argv)
{
    if (argc == 1)
    {
        print_manual_page(argv[0]);
        return NO_ERROR;
    }
    else if (argc != 4 && argc != 6)
    {
        eprintf("invalid arguments\n\n");
        print_usage(argv[0]);
        return COMMAND_LINE_ARGS_ERROR;
    }

    const char* input_file_name = argv[1];
    if (!is_wav_file(input_file_name))
    {
        eprintf("input file %s does not exist\n", input_file_name);
        return INPUT_FILE_FORMAT_ERROR;
    }

    const char* output_file_name = argv[2];
    if (!is_wav_file(output_file_name))
    {
        eprintf("output file %s does not exist\n", output_file_name);
        return OUTPUT_FILE_FORMAT_ERROR;
    }

    float cutoff = get_cutoff(argv[3]);
    if (!cutoff)
    {
        eprintf("cutoff frequency must be a positive numerical value between 20Hz and 20000Hz.\n");
        return CUTOFF_VALUE_ERROR;
    }
    
    char* window_type = WINDOW_TYPE_KAISSER;
    if (argc == 6)
    {
        if (strcmp(argv[4], "-w"))
        {
            eprintf("unknown command line option %s.\n", argv[4]);
            return COMMAND_LINE_ARGS_ERROR;
        }

        window_type = get_window_type(argv[5]);
        if (!window_type)
        {
            eprintf("unknown window type %s.\n", argv[5]);
            return UNKNOWN_WINDOW_ERROR;
        }
    }

}

//------------------------------------------------------------------------------
// prints usage instructions
//------------------------------------------------------------------------------
void print_usage(const char* prog_name) {

    printf("usage: %s [<input_wave_file> <output_wave_file> ", prog_name);
    printf("<cutoff_frequency> [-w <window_type>]]\n");
}

//------------------------------------------------------------------------------
// prints unix style user manual
//------------------------------------------------------------------------------
void print_manual_page(const char* prog_name) {

    printf("\n");
    printf("NAME\n\n");
    printf("%s - Low Pass Filter for WAVE files.\n\n", prog_name);

    printf("SYNOPSIS\n\n");
    print_usage(prog_name);
    printf("\n");

    printf("DESCRIPTION\n\n");
    printf("%s opens a WAVE file <input_wave_file> and filters ", prog_name);
    printf("it using a\n126th order FIR Low Pass Filter with cutoff ");
    printf("frequency <cutoff_frequency>\nbetween 20Hz and 20kHz. An ");
    printf("optional window function [-w <window_type>] can\nbe selected, ");
    printf("with the variants listed below. The default window is ");
    printf("bartlett.\nThe filtered data is then saved to a new WAVE file ");
    printf("<output_wave_file>.\n\n");
    printf("Valid window types are:\n");
    printf(" - %s (default)\n", WINDOW_TYPE_BARTLETT);
    printf(" - %s\n", WINDOW_TYPE_BLACKMAN);
    printf(" - %s\n", WINDOW_TYPE_HAMMING);
    printf(" - %s\n", WINDOW_TYPE_HANNING);
    printf(" - %s\n", WINDOW_TYPE_KAISSER);
    printf(" - %s\n\n", WINDOW_TYPE_RECTANGULAR);
    printf("Running the program without any arguments prints the manual ");
    printf("page.\n\n");

    printf("EXIT STATUS\n\n");
    printf(" %d - NO_ERROR\n", NO_ERROR);
    printf(" %d - COMMAND_LINE_ARGS_ERROR\n", COMMAND_LINE_ARGS_ERROR);
    printf(" %d - CUTOFF_VALUE_ERROR\n", CUTOFF_VALUE_ERROR);
    printf(" %d - FILTER_FILE_ERROR\n", FILTER_FILE_ERROR);
    printf(" %d - INPUT_FILE_OPEN_ERROR\n", INPUT_FILE_OPEN_ERROR);
    printf(" %d - INPUT_FILE_FORMAT_ERROR\n", INPUT_FILE_FORMAT_ERROR);
    printf(" %d - OUTPUT_FILE_FORMAT_ERROR\n\n", OUTPUT_FILE_FORMAT_ERROR);

    printf("EXAMPLES\n\n");
    printf("%s\n", prog_name);
    printf("%s input.wav output.wav 1000\n", prog_name);
    printf("%s input.wav output.wav 1000 -w bartlett\n", prog_name);
    printf("%s input.wav output.wav 1000 -w blackman\n", prog_name);
    printf("%s input.wav output.wav 1000 -w hamming\n", prog_name);
    printf("%s input.wav output.wav 1000 -w hanning\n", prog_name);
    printf("%s input.wav output.wav 1000 -w kaisser\n", prog_name);
    printf("%s input.wav output.wav 1000 -w rectangular\n\n", prog_name);

    printf("AUTHOR\n\n");
    printf("Tom Mason | University of Surrey (UG - Music and Media)\n\n");
}

bool is_wav_file(const char* file_name)
{
    if (strlen(file_name) < 4)
        return false;

    const char* extension = &file_name[strlen(file_name) - 4];

    if (strcmp(extension, ".wav"))
        return false;
    else
        return true;
}

float get_cutoff(const char* cutoff)
{
    if (cutoff[0] == '-')
        return 0;

    int num_decimal_points = 0;
    for (int i = 0; i < strlen(cutoff); ++i)
    {
        if (!isdigit(cutoff[i]) && cutoff[i] != '.')
            return 0;
        else if (cutoff[i] == '.')
            if (num_decimal_points++ > 0)
                return 0;
    }

    float cutoff_val = 0.0f;
    if (sscanf(cutoff, "%f", &cutoff_val) != 1)
        return 0;

    if (cutoff_val < 20.0f || cutoff_val > 20000.0f)
        return 0;

    return cutoff_val;
}

const char* get_window_type(const char* window_type)
{
    if (!strcmp(window_type, WINDOW_TYPE_KAISSER))          return window_type;
    else if (!strcmp(window_type, WINDOW_TYPE_HAMMING))     return window_type;
    else if (!strcmp(window_type, WINDOW_TYPE_HANNING))     return window_type;
    else if (!strcmp(window_type, WINDOW_TYPE_BLACKMAN))    return window_type;
    else if (!strcmp(window_type, WINDOW_TYPE_BARTLETT))    return window_type;
    else if (!strcmp(window_type, WINDOW_TYPE_RECTANGULAR)) return window_type;
    else
        return NULL;
}
