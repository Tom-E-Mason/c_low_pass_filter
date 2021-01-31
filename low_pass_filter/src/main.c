
#include <stdio.h>

#define WINDOW_TYPE_KAISSER "kaisser"         // ------------
#define WINDOW_TYPE_HAMMING "hamming"         // 
#define WINDOW_TYPE_HANNING "hanning"         // window type
#define WINDOW_TYPE_BLACKMAN "blackman"       // id codes
#define WINDOW_TYPE_BARTLETT "bartlett"       // 
#define WINDOW_TYPE_RECTANGULAR "rectangular" // ------------

enum errors {                 // exit status codes
    NO_ERROR,                 // program runs successfully
    COMMAND_LINE_ARGS_ERROR,  // too many inputs from user
    CUTOFF_VALUE_ERROR,       // invalid cutoff value
    FILTER_FILE_ERROR,        // error in filter function
    INPUT_FILE_OPEN_ERROR,    // input file doesn't exist
    INPUT_FILE_FORMAT_ERROR,  // .wav extension missing on input file name
    OUTPUT_FILE_FORMAT_ERROR, // .wav extension missing on output file name

};

void print_usage(const char* prog_name);
void print_manual_page(const char* prog_name);

int main(int argc, const char** argv)
{
    print_manual_page(argv[0]);
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