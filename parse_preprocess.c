#include <stdarg.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "assemble.h"


int parse_preprocess(struct token_list *tokens, struct program_info *info) {
    int found_errors = FALSE;
    struct token *here = tokens->first;

    while (here) {

        // skip labels
        if (here->type == tt_identifier && here->next && here->next->type == tt_colon) {
            here = here->next->next;
            continue;
        }

        // define statements
        if (here->type == tt_identifier && strcmp(here->text, ".define") == 0) {
            struct token *start = here;
            here = here->next;

            if (here->type != tt_identifier) {
                report_error(&here->origin, "expected identifier");
                skip_line(&here);
                continue;
            }
            const char *name = here->text;
            here = here->next;

            if (here->type != tt_integer) {
                report_error(&here->origin, "expected integer, found %s", token_name(here));
                skip_line(&here);
                found_errors = TRUE;
                continue;
            }

            if (get_label(info->first_label, name) != NULL) {
                report_error(&here->origin, "name %s already in use", name);
                skip_line(&here);
                found_errors = TRUE;
                continue;
            }

            if (!add_label(&info->first_label, name, here->i)) {
                report_error(&here->origin, "error creating constant");
                skip_line(&here);
                found_errors = TRUE;
                continue;
            }
            here = remove_line(tokens, start);
            continue;
        }

        // included files
        if (token_check_identifier(here, ".include")) {
            struct token *before = here->prev;
            struct token *start = here;
            struct token_list *new_tokens = NULL;

            if (!here->next) {
                report_error(&here->origin, "Unexpected end of tokens");
                return FALSE;
            }
            here = here->next;

            if (here->type != tt_string) {
                report_error(&here->origin, "Expected string");
                skip_line(&here);
                found_errors = TRUE;
                continue;
            }

            if (here->next && here->next->type != tt_eol) {
                report_error(&here->origin, "Expected EOL");
                skip_line(&here);
                found_errors = TRUE;
                continue;
            }

            new_tokens = lex_file(here->text);

            struct token *start_next = start->next;
            remove_token(tokens, start_next);
            free_token(start_next);
            remove_token(tokens, start);
            free_token(start);

            if (new_tokens == NULL) {
                skip_line(&here);
                found_errors = TRUE;
                continue;
            }
            merge_token_list(tokens, new_tokens, before);
            if (before) {
                here = before->next;
            } else {
                here = tokens->first;
            }
            continue;
        }

        // otherwise nothing for us on this line, skip to the next
        skip_line(&here);

    }

    return !found_errors;
}


