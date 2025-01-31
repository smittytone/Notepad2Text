/*
========================================================================
Amstrad Notepad WordProcessor to RTF conversion utility

Author 	: Maksim Lin
Started	: 13/11/1996
Updated : 25/01/2025 by Tony Smith (@smittytone)
Version	: 1.1

Changes :
    - 1.0
        - added commandline input with auto output filename creation
          using remove_ext() & removed interactive input.
        - added simple help screen.
        - changed error message for unrecognised NP format code.
        - added checking and proper handling for \,{ & } characters.
    - 1.1
        - Fix to build under Linux/macOS.
        - Use Linux/macOS newlines style.
        - Add `--text` flag for text only output.
        - Add optional debug output.
        - Code tidy.
=========================================================================
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
//#include <process.h>  /* Windows only? I', focusing on Linux (TS) */

#define version "1.1" /* make sure to keep this up to date*/

#define doc_start  "{\\rtf1\\ansi{\\fonttbl{\\f0\\fs24 Times New Roman;}}\n"
#define text_start "\\pard\\plain\\fs24 "
#define cr_lf	   "\\par "

/* Note: that the default font is set as Times New Roman-12 point
   & the enlarge font is 14 point */

/* Notepad ASCII Format codes: (paired) */

#define bold 0xE2 /* bold */
#define ital 0xE9 /* italic */
#define undr 0xF5 /* underline */
#define subs 0xF3 /* subscript */
#define sups 0xF4 /* superscript */
#define larg 0xEC /* enlarged */
#define spc  0x90 /* soft space */

#define num_of_fcodes 6 /* number of paired format codes */

/* Notepad ASCII Format codes: (non-paired) */

#define escd 0x05 /* escape code for format code characters */
#define soft 0x8A /* "soft" carriage-return (occurs after a CR character) */
#define lnfd 0x0A /* line-feed (occurs after a CR char to indicate end of line */
#define cret 0x0D
#define eod  0x1A /* end of doc marker - usually get a line of these */

/* Special RTF characters: */

#define lbrack '{'
#define rbrack '}'
#define bslash '\\'

#define max_filename_size 257 /* allows for long filenames */
#define max_format_string 25

/* Allow for debugging, or comment out */
//#define DEBUG 1

int conv_wp(unsigned char code, char *out_str, int *first_time);
void remove_ext(char *in_name, char *out_name);
void debug(int code, int location);
void output_debug(char* message);
void help_scr(void);

/* ========================== MAIN ========================= */

int main(int argc, char *argv[]) {

    char out_filename[max_filename_size];
    char code_str[max_format_string];
    FILE *in_file, *out_file;
    /* Used to check each char of input file */
    unsigned char parse_ch;
    /* Used to tell `conv_wp()` that it's being called for the first time */
    int first = 1;
    int text_only = 0;

    /* ============ start of filenames input section ================ */

    if (argc == 1) {
        help_scr();
        exit(0);
    }

    if (argc > 3) {
        printf("[Error] Too many command line arguments\n");
        help_scr();
        exit(1);
    }
 
    int path_arg = 99;
    if (argc == 3) {
        if (strcmp(argv[1], "-t")== 0 || strcmp(argv[1], "--text") == 0) {
            path_arg = 2;
            text_only = 1;
        } else if (strcmp(argv[2], "-t") == 0 || strcmp(argv[2], "--text") == 0) {
            path_arg = 1;
            text_only = 1;
        } else {
            printf("[Error] Too many or incorrect command line arguments\n");
            help_scr();
            exit(1);
        }
    }

    if (argc == 2) {
        path_arg = 1;
    }

    in_file = fopen(argv[path_arg], "r");
    if (in_file == NULL) {
        /* check for error in fopen */
        printf("[Error] Failed to open input file\n");
        exit(1);
    }

    remove_ext(argv[path_arg], out_filename);
    strcat(out_filename, text_only ? ".txt" : ".rtf");
    out_file = fopen(out_filename, "w");
    if (out_file == NULL) {
        /* check for error in fopen */
        printf("[Error] Failed to open output file\n");
        exit(1);
    }

    /* ============ end of filenames input section ================ */

    if (text_only == 0) {
        fprintf(out_file, doc_start);  /* standard RTF begin doc codes */
        fprintf(out_file, text_start);
    }

    int char_count = 0;
    int do_debug = 0;
    while (!(feof(in_file))) {
        parse_ch = getc(in_file);
        switch(parse_ch) {
            /* check for CR's or ESC code */
            case escd:
                /* read the char after the esc code or CR */
                char_count++;
                parse_ch = getc(in_file);
#ifdef DEBUG
                debug(parse_ch, char_count);
#endif
                if (text_only == 0) {
                    /* convert & check return value for error from conv_wp() */
                    if (!(conv_wp(parse_ch, code_str, &first))) {
                        /* write RTF format string to file */
                        fprintf(out_file, "%s", code_str);
                    } else {
                        printf("[Warning] Unknown code found (0x%02X) at %d, output may be corrupted\n", parse_ch, char_count);
                        fprintf(out_file, "%s", code_str);
                    }
                }
                break;
                /* end of case escd */
            case lnfd:
                /* fs24 insures correct initial font size */
                if (text_only == 0) {
                    fprintf(out_file, "\\par\\fs24 ");
                } else {
                    fprintf(out_file, "%c", lnfd);
                }
                do_debug = 1;
                break;
            case eod:
            case cret:
            case soft:
            case 0xFF:
                /* ignore soft-CR and end-of-doc marker */
                do_debug = 1;
                break;
            case lbrack:
                /* check for left curly bracket (special RTF char) */
                if (text_only == 0) fprintf(out_file, "\\%c", parse_ch);
                do_debug = 1;
                break;
            case rbrack:
                /* check for right curly bracket (special RTF char) */
                if (text_only == 0) fprintf(out_file, "\\%c", parse_ch);
                break;
            case bslash:
                /* check for backslash (special RTF char) */
                if (text_only == 0) fprintf(out_file, "\\%c", parse_ch);
                do_debug = 1;
                break;
            default:
                fprintf(out_file, "%c", parse_ch);
                break;
        }
         /* end of switch */

#ifdef DEBUG
            if (do_debug) debug(parse_ch, char_count);
#endif
        char_count++;
        do_debug = 0;
    }

    /* this little hack goes back and inserts a } to end the rtf file,
    before the end of file marker which for some reason gets written in
    before the fclose operation, probably being picked up from the end of
    the Notepad file ?? */
    if (text_only == 0) {
        fseek(out_file, -2, SEEK_CUR);
        fprintf(out_file, "}");
    }

    fclose(in_file);
    fclose(out_file);

    printf("Conversion successful\n");
    exit(0);

} /* end of main */

/* =================== FUNCTIONS ============================= */

int conv_wp(unsigned char code, char *out_str, int *first_time) {

    int a;
    /* this holds the status of any pending formating
    the layout is : fcode[x] with x as per the defines below */
    static int f_code[(num_of_fcodes + 1)];

    #define b 0  /* bold */
    #define i 1  /* italic */
    #define u 2  /* underline */
    #define s 3  /* subscript */
    #define p 4  /* superscript */
    #define l 5  /* enlarged */

    /* init array to clear (all zeros) if this first time being called */
    if (*first_time == 1) {
        /* reset so not done again during this execution */
        *first_time = 0;
        a = 0;
        while (f_code[a] != 0) {
            f_code[a] = 0;
            a++;
        }
    }

    /* this switch will either set a format type as pending
    or clear a pending format for the paired format codes,
    otherwise it will simply set a hard-CR and will ignore a soft CR */
    switch(code) {
        case bold:
            f_code[b] = f_code[b] == 1 ? 0 : 1;
            break;
        case ital:
            f_code[i] = f_code[i] == 1 ? 0 : 1;
            break;
        case undr:
            f_code[u] = f_code[u] == 1 ? 0 : 1;
            break;
        case subs:
            f_code[s] = f_code[s] == 1 ? 0 : 1;
            break;
        case sups:
            f_code[p] = f_code[p] == 1 ? 0 : 1;
            break;
        case larg:
            f_code[l] = f_code[l] == 1 ? 0 : 1;
            break;
        case spc:
            // This appears to be an inserted space for justification
            strcat(out_str, "");
            return 0;
        default:
            /* return 1 to indicate unrecognised format code */
            return 1;
    } /* end of switch */

    /* adds the relevant RTF format codes to the format string code_str that */
    strcpy(out_str, "\\plain");
    if (f_code[b] == 1) strcat(out_str, "\\b");
    if (f_code[i] == 1) strcat(out_str, "\\i");
    if (f_code[u] == 1) strcat(out_str, "\\ul");
    if (f_code[s] == 1) strcat(out_str, "\\dn");
    if (f_code[p] == 1) strcat(out_str, "\\up");
    if (f_code[l] == 1) strcat(out_str, "\\fs28 ");
    else strcat(out_str, "\\fs24 ");

    /* return zero to indicate successful completion of function */
    return 0;

} /* end of function conv_wp */

/* ===================================================================== */

/* removes ext (if any) from a given filename string and passes back as
   out_name
 */
void remove_ext(char *in_name, char *out_name) {

    int cnt=0;
    while ((in_name[cnt] != '.') && (in_name[cnt] != '\0')) {
        /* step thru in_name looking for . or end of string */
        cnt++;
    }

    out_name[cnt] = '\0';
    /* this terminates out_name at the location of the "." in in_name string */

    /* now copy everything but filename ext into out_name */
    while (cnt>0) {
        cnt--; /* dec here so that we start with char before the "." */
        out_name[cnt] = in_name[cnt];
    }
}

/* ===================================================================== */

#ifdef DEBUG
void debug(int code, int location) {

    static int eod_reported = 0;
    char message[64] = "";
    char item[24] = "";

    // Generate a human-friendly code explanation
    switch(code) {
        case bold:
            sprintf(item, "bold");
            break;
        case ital:
            sprintf(item, "italic");
            break;
        case undr:
            sprintf(item, "underline");
            break;
        case subs:
            sprintf(item, "subscript");
            break;
        case sups:
            sprintf(item, "superscript");
            break;
        case larg:
            sprintf(item, "large text");
            break;
        case soft:
            sprintf(item, "soft CR");
            break;
        case lnfd:
            sprintf(item, "line feed");
            break;
        case lbrack:
            sprintf(item, "left brace");
            break;
        case rbrack:
            sprintf(item, "right brace");
            break;
        case bslash:
            sprintf(item, "backslash");
            break;
        case spc:
            sprintf(item, "padded space");
            break;
        case eod:
            if (!(eod_reported)) {
                sprintf(item, "end of doc");
                eod_reported = 1;
            } else {
                return;
            }
            break;
        default:
            break;
    }

    // Got an real name for the format? Print it out, or just state the code
    if (strlen(item) > 0) {
        sprintf(message, "Found %s marker at location %d", (const char*)item, location);
    } else {
        sprintf(message, "Found code 0x%02X at location %d", code, location);
    }

    // Output the final debug message
    output_debug(message);
}

void output_debug(char* message) {

    printf("[DEBUG] %s\n", message);
}
#endif

/* ===================================================================== */

void help_scr(void) {

    printf("\nAmstrad NC100 WP to RTF/TXT converter\n");
    printf("by Maksim Lin and Tony Smith, version %s\n", version);
    printf("\nUsage: notepad2text [--text] /path/to/nc100/word/file\n\n");
    printf("Options:\n");
    printf("  -t / --text    Output plain text rather than RTF.\n\n");
}
