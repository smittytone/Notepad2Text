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
        - Fix to build under Linux.
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

#define num_of_fcodes 6 /* number of paired format codes */

/* Notepad ASCII Format codes: (non-paired) */

#define escd 0x05 /* escape code for format code characters */
#define soft 0x8A /* "soft" carriage-return (occurs after a CR character) */
#define lnfd 0x0A /* line-feed (occurs after a CR char to indicate end of line */

/* Special RTF characters: */

#define lbrack '{'
#define rbrack '}'
#define bslash '\\'

#define max_filename_size 257 /* allows for long filenames */
#define max_format_string 25

int conv_wp(unsigned char code,char *out_str,int *first_time);
void remove_ext(char *in_name,char *out_name);
void help_scr(void);

/* ========================== MAIN ========================= */

void main(int argc, char *argv[]) {

    char out_filename[max_filename_size];
    FILE *in_file, *out_file;
    unsigned char parse_ch;	/* used to check each char of input file */
    int first=1;		/* used to tell conv_wp that it's being
               called for the first time */
    char code_str[max_format_string];

    /* ============ start of filenames input section ================ */

    if (argc == 1) {
        help_scr();
        exit(0);
    }

    if (argc > 2) {
        printf("[Error] Too many command line arguments");
        help_scr();
        exit(1);
    }

    if (argc == 2) {
        in_file = fopen(argv[1], "r");
        if (in_file == NULL) { 
            /* check for error in fopen */
            printf("[Error] Failed to open input file");
            exit(1);
        }
  
        remove_ext(argv[1], out_filename);
        strcat(out_filename, ".rtf");
        out_file = fopen(out_filename, "w");
        if (out_file == NULL) {
            /* check for error in fopen */
            printf("[Error] Failed to open output file");
            exit(1);
        }
    }

    /* ============ end of filenames input section ================ */

    fprintf(out_file,doc_start);  /* standard RTF begin doc codes */
    fprintf(out_file,text_start);

    while (!(feof(in_file))) {
        parse_ch = getc(in_file);
        switch(parse_ch) {
            /* check for CR's or ESC code */
            case escd:
                /* read the char after the esc code or CR */
                parse_ch = getc(in_file);  

                /* convert & check return value for error from conv_wp() */
                if (!(conv_wp(parse_ch, code_str, &first))) {
                    /* write RTF format string to file */
                    fprintf(out_file, "%s", code_str); 	
                } else {
                    printf("[Warning] Unknown code found, output may be corrupted");
                    fprintf(out_file, "%s", code_str);
                }
                break; 
                /* end of case escd */
            case lnfd:
                /* fs24 insures correct initial font size */
                fprintf(out_file, "\\par\\fs24 ");
                break;
            case soft:
                /* ignore soft-CR */
                break;
            case lbrack: 
                /* check for left curly bracket (special RTF char) */
                fprintf(out_file, "\\%c", parse_ch);
                break;
            case rbrack:
                /* check for right curly bracket (special RTF char) */
                fprintf(out_file, "\\%c", parse_ch);
                break;
            case bslash: 
                /* check for backslash (special RTF char) */
                fprintf(out_file, "\\%c", parse_ch);
                break;
            default:
                fprintf(out_file, "%c", parse_ch);
                break;
        } /* end of switch */
    }

    /* this little hack goes back and inserts a } to end the rtf file,
    before the end of file marker which for some reason gets written in
    before the fclose operation, probably being picked up from the end of
    the Notepad file ?? */

    fseek(out_file, -2, SEEK_CUR);
    fprintf(out_file, "}");

    fclose(in_file);
    fclose(out_file);

    printf("conversion successful \n");

} /* end of main */

/* =================== FUNCTIONS ============================= */

int conv_wp(unsigned char code, char *out_str, int *first_time) {

    int a;
    static int f_code[(num_of_fcodes + 1)]; /* this holds the status of any pending formating
                                            the layout is : fcode[x] with x as per the defines below */
    #define b 0  /* bold */
    #define i 1  /* italic */
    #define u 2  /* underline */
    #define s 3  /* subscript */
    #define p 4  /* superscript */
    #define l 5  /* enlarged */

    /* init array to clear (all zeros) if this first time being called */
    if (*first_time == 1) {
        *first_time = 0; /* reset so not done again during this execution */
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
            if (f_code[b] == 1) f_code[b] = 0;
            else f_code[b] = 1;
            break;
        case ital:
            if (f_code[i] == 1) f_code[i] = 0;
            else f_code[i] = 1;
            break;
        case undr:
            if (f_code[u] == 1) f_code[u] = 0;
            else f_code[u] = 1;
            break;
        case subs:
            if (f_code[s] == 1) f_code[s] = 0;
            else f_code[s] = 1;
            break;
        case sups:
            if (f_code[p] == 1) f_code[p] = 0;
            else f_code[p] = 1;
            break;
        case larg:
            if (f_code[l] == 1) f_code[l] = 0;
            else f_code[l] = 1;  
            break;
        default:
            return 1; /* return 1 to indicate unrecognised format code */
    } /* end of switch */

    /* adds the relevant RTF format codes to the format string code_str that */
    strcpy(out_str,"\\plain");
    if (f_code[b] == 1) strcat(out_str, "\\b");
    if (f_code[i] == 1) strcat(out_str, "\\i");
    if (f_code[u] == 1) strcat(out_str, "\\ul");
    if (f_code[s] == 1) strcat(out_str, "\\dn");
    if (f_code[p] == 1) strcat(out_str, "\\up");
    if (f_code[l] == 1) strcat(out_str, "\\fs28 ");
    else strcat(out_str, "\\fs24 ");

    return 0;  /* return zero to indicate successful completion of function */

} /* end of function conv_wp */

/* ===================================================================== */

/* removes ext (if any) from a given filename string and passes back as
   out_name 
 */
void remove_ext(char *in_name,char *out_name) {

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

void help_scr(void) {

    printf("\n");
    printf("Notepad WP to RTF converter \n");
    printf("by Maksim Lin, version %s \n", version);
    printf("usage: np-rtf filename \n");
    printf("\n");
}
