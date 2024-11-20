let load_file f =
  let ic = open_in f in
  let n = in_channel_length ic in
  let s = Bytes.create n in
  really_input ic s 0 n;
  close_in ic;
  (s)

let rec list_car_help ch pos ch_end =
    if pos == ch_end
        then []
        else (String.get ch pos)::(list_car_help ch (pos + 1) ch_end) ;;
let list_car ch = (list_car_help ch 0 (String.length ch)) ;;
let charlist_to_string chars =
      let buf = Buffer.create 16 in
      List.iter (Buffer.add_char buf) chars;
      Buffer.contents buf

type operator_ty =
    | COMMA
    | COLON
    | SEMICOLON
    | LEFT_PAREN
    | RIGHT_PAREN
    | LEFT_BRACK
    | RIGHT_BRACK
    | LEFT_CURLY
    | RIGHT_CURLY
    | DOT
    | PLUS
    | MINUS
    | TIMES
    | DIVIDE
    | ASSIGN
    | LEFT_RIGHT_SIGNS
    | LESS_THAN
    | LESS_EQUAL
    | GREATER_THAN
    | GREATER_EQUAL
    | AMERSAND
    | VERTI_BAR
    | COLON_EQUAL ;;

type keyword_ty =
    | ARRAY_KW
    | IF_KW
    | THEN_KW
    | ELSE_KW
    | WHILE_KW
    | FOR_KW
    | TO_KW
    | DO_KW
    | LET_KW
    | IN_KW
    | END_KW
    | OF_KW
    | BREAK_KW
    | NIL_KW
    | FUNCTION_KW
    | VAR_KW
    | TYPE_KW
    | IMPORT_KW
    | PRIMITIVE_KW ;;

type basic_token =
    | INTEGER of string
    | OPERATOR of operator_ty
    | STRING of string
    | IDENTIFIER of string
    | KEYWORD of keyword_ty ;;

type token_option =
    | SOME_TOKEN of basic_token
    | ERROR_TOKEN of string
    | NO_TOKEN ;;

type token_result = {
    len_t: int;
    rest_list: char list;
    token_opt: token_option;
} ;;
type line_col_pos = { line_p : int; col_p : int; } ;;

let splitters = [",", ":", ";", "(", ")", "[", "]", "{", "}", ".", "+", "-", "*", "/", "=", "<>", "<", "<=", ">", ">=", "&", "|", ":="]
let keywords = ["array", "if", "then", "else", "while", "for", "to", "do", "let", "in", "end", "of", "break", "nil", "function", "var", "type", "import", "primitive"]

let is_in_range bottom top ch =
    let top_n = (Char.code top) in
    let bottom_n = (Char.code bottom) in
    let ch_n = (Char.code ch) in
        (top_n >= ch_n && bottom_n <= ch_n)

let is_int = is_in_range '0' '9'
let is_octal = is_in_range '0' '7'
let is_hex ch = (is_int ch) || (is_in_range 'a' 'f' ch)
let is_letter ch = (is_in_range 'a' 'z' (Char.lowercase ch))
let is_reasonable_char = is_in_range '!' '~'
let is_valid_id ch = (is_letter ch) || (is_int ch) || (ch == '_')
let is_whitespace ch = match ch with
    | ' ' -> true
    | '\t' -> true
    | '\n' -> true
    | '\r' -> true
    | _ -> false

let to_int ch = ((Char.code ch) - 48)
let to_hex ch =
    if (is_int ch) then (to_int ch) else
        let code = (Char.code (Char.lowercase ch)) in
         (code - 96) + 10

let rec wind_list l pos =
    if pos <= 0 then l else  wind_list (List.tl l) (pos-1)

let list_slice =
    let rec list_slice_h build_l l size =
        if size <= 0 then build_l else (list_slice_h ((List.hd l)::build_l) (List.tl l) (size - 1))
    in list_slice_h []

type string_proc_opt =
    | ProccessedStr of string
    | ErrorStr of string

let octal_result rest_chars =
    if (List.length rest_chars) < 3 then None else
    let c1 = (List.nth rest_chars 0) in
    let c2 = (List.nth rest_chars 1) in
    let c3 = (List.nth rest_chars 2) in
    if (is_octal c1) && (is_octal c2) && (is_octal c3) then
        let intval = (((to_int c1) * 8 + (to_int c2)) * 8 + (to_int c3))
        in
            if intval > 255 then None else Some (Char.chr intval)
    else None

let hexidecimal_result rest_chars =
    if (List.length rest_chars) < 2 then None else
    let c1 = (List.nth rest_chars 0) in
    let c2 = (List.nth rest_chars 1) in
    if (is_hex c1) && (is_hex c2) then
        let intval = ((to_hex c1) * 16 + (to_hex c2)) in
         Some (Char.chr intval) (*Guarenteed to be between 0 and 255*)
    else None

let rec get_string_end pos str_chars = match str_chars with
    | [] -> None
    | '\\'::'"'::rest -> (get_string_end (pos + 2) rest)
    | '\\'::'\\'::rest -> (get_string_end (pos + 2) rest)
    | '\\'::rest -> (get_string_end (pos + 1) rest)
    | '"'::rest -> Some (pos + 1)
    | ch::rest -> (get_string_end (pos + 1) rest)


let rec build_string new_chars str_chars = match str_chars with
    | '\\'::'"'::rest -> (build_string ('"'::new_chars) rest)
    | '\\'::'\\'::rest -> (build_string ('\\'::new_chars) rest)
    | '\\'::'a'::rest -> (build_string ('a'::new_chars) rest)
    | '\\'::'b'::rest -> (build_string ('b'::new_chars) rest)
    | '\\'::'f'::rest -> (build_string ('f'::new_chars) rest)
    | '\\'::'n'::rest -> (build_string ('n'::new_chars) rest)
    | '\\'::'r'::rest -> (build_string ('r'::new_chars) rest)
    | '\\'::'t'::rest -> (build_string ('t'::new_chars) rest)
    | '\\'::'v'::rest -> (build_string ('v'::new_chars) rest)
    | '\\'::'x'::rest ->
        (match (hexidecimal_result rest) with
            | Some ch -> (build_string (ch::new_chars) (wind_list rest 2))
            | None -> ErrorStr ("bad hexidecimal escape code")
        )
    | '\\'::rest ->
        (match (octal_result rest) with
            | Some ch -> (build_string (ch::new_chars) (wind_list rest 3))
            | None -> ErrorStr (Printf.sprintf "bad escape code \\%c" (List.hd rest) )
        )
    | '"'::rest -> ProccessedStr (charlist_to_string new_chars)
    | ch::rest -> (build_string (ch::new_chars) rest)
    | [] -> ErrorStr "Reached end of file before end of string"

let process_string total_list =
    let str_no_leading_quote = (List.tl total_list) in
        match (get_string_end 1 str_no_leading_quote) with
            | None -> {len_t=100000000; rest_list=[]; token_opt=ERROR_TOKEN "Reached end of file before string literal termination"; }
            | Some str_len ->
                let leftover_list = (wind_list total_list str_len) in
                    match (build_string [] str_no_leading_quote) with
                        | ErrorStr err_str -> {len_t=str_len; rest_list=leftover_list; token_opt=ERROR_TOKEN err_str; }
                        | ProccessedStr res_str ->  {len_t=str_len; rest_list=leftover_list; token_opt=SOME_TOKEN (STRING res_str); }


let process_line_positions char_list =
    let rec process_line_positions_h line_no line_list char_pos l_start_list char_list = match char_list with
        | [] -> (line_list,l_start_list)
        | '\n'::'\r'::rest -> (process_line_positions_h (line_no + 1) (line_no::line_no::line_list) (char_pos + 2) ((char_pos + 2)::l_start_list) rest)
        | '\r'::'\n'::rest -> (process_line_positions_h (line_no + 1) (line_no::line_no::line_list) (char_pos + 2) ((char_pos + 2)::l_start_list) rest)
        | '\r'::rest -> (process_line_positions_h (line_no + 1) (line_no::line_list) (char_pos + 1) ((char_pos + 1)::l_start_list) rest)
        | '\n'::rest -> (process_line_positions_h (line_no + 1) (line_no::line_list) (char_pos + 1) ((char_pos + 1)::l_start_list) rest)
        | a::rest -> (process_line_positions_h line_no (line_no::line_list) (char_pos + 1) l_start_list rest) in
    let line_list, line_start_list = (process_line_positions_h 0 [] 0 [] char_list) in
        (Array.of_list (List.rev line_list) , Array.of_list (List.rev line_start_list)) ;;




let process_whitespace total_list = {len_t=1; rest_list=total_list; token_opt=NO_TOKEN; }
let process_possible_operator total_list =
    let oper_ty, oper_len = match total_list with
        | ','::rest -> (COMMA, 1)
        | ';'::rest -> (SEMICOLON, 1)
        | '('::rest -> (LEFT_PAREN, 1)
        | ')'::rest -> (RIGHT_PAREN, 1)
        | '['::rest -> (LEFT_BRACK, 1)
        | ']'::rest -> (RIGHT_BRACK, 1)
        | '{'::rest -> (LEFT_CURLY, 1)
        | '}'::rest -> (RIGHT_CURLY, 1)
        | '.'::rest -> (DOT, 1)
        | '+'::rest -> (PLUS, 1)
        | '-'::rest -> (MINUS, 1)
        | '*'::rest -> (TIMES, 1)
        | '/'::rest -> (DIVIDE, 1)
        | '='::rest -> (ASSIGN, 1)
        | '<'::'>'::rest -> (LEFT_RIGHT_SIGNS, 2)
        | '<'::'='::rest -> (LESS_EQUAL, 2)
        | '<'::rest -> (LESS_THAN, 1)
        | '>'::'='::rest -> (GREATER_EQUAL, 2)
        | '>'::rest -> (GREATER_THAN, 1)
        | '&'::rest -> (AMERSAND, 1)
        | '|'::rest -> (VERTI_BAR, 1)
        | ':'::'='::rest -> (COLON_EQUAL, 2)
        | ':'::rest -> (COLON, 1)
        | _ -> (COLON, -1) (*error*)
    in let tok_opt = if oper_len == -1 then ERROR_TOKEN ("Bad charachter") else SOME_TOKEN (OPERATOR oper_ty)
    in let act_oper_len = if oper_len == -1 then 1 else oper_len
    in
         {len_t=act_oper_len; rest_list=(wind_list total_list act_oper_len); token_opt=tok_opt;  }


let match_keyword str = match str with
    | "array" -> Some ARRAY_KW
    | "if" -> Some IF_KW
    | "then" -> Some THEN_KW
    | "else" -> Some ELSE_KW
    | "while" -> Some WHILE_KW
    | "for" -> Some FOR_KW
    | "to" -> Some TO_KW
    | "do" -> Some DO_KW
    | "let" -> Some LET_KW
    | "in" -> Some IN_KW
    | "end" -> Some END_KW
    | "of" -> Some OF_KW
    | "break" -> Some BREAK_KW
    | "nil" -> Some NIL_KW
    | "function" -> Some FUNCTION_KW
    | "var" -> Some VAR_KW
    | "type" -> Some TYPE_KW
    | "import" -> Some IMPORT_KW
    | "primitive" -> Some PRIMITIVE_KW
    | _ -> None

let rec get_valid_length is_valid_fn len char_list = match char_list with
    | [] -> len
    | ch::rest ->
        if (is_valid_fn ch) then (get_valid_length is_valid_fn (len + 1) rest)
        else len

let get_int_length = get_valid_length is_int 0
let get_identifier_length = get_valid_length is_valid_id 0


let process_letter total_list =
    let id_len = (get_identifier_length total_list) in
    let id_str = (charlist_to_string (list_slice total_list id_len)) in
    let token = match (match_keyword id_str) with
        | None -> IDENTIFIER id_str
        | Some keyword -> KEYWORD keyword
    in
        { len_t=id_len; rest_list=(wind_list total_list id_len); token_opt=SOME_TOKEN token; }

let process_int total_list =
    let int_len = (get_int_length total_list) in
    let int_str = (charlist_to_string (list_slice total_list int_len)) in
        { len_t=int_len; rest_list=(wind_list total_list int_len); token_opt=SOME_TOKEN (INTEGER int_str); }

let rec find_nested_comment_end char_list pos comment_count =
    if comment_count == 0 then Some pos else
        match char_list with
        | [] -> None
        | '/'::'*'::rest -> find_nested_comment_end rest (pos + 2) (comment_count + 1)
        | '*'::'/'::rest -> find_nested_comment_end rest (pos + 2) (comment_count - 1)
        | a::rest -> find_nested_comment_end rest (pos + 1) comment_count

let process_comment total_list =
    let no_leading_comment = (List.tl (List.tl total_list)) in
    let comment_size = 2 in
    let start_depth = 1 in
    let comment_end = (find_nested_comment_end no_leading_comment comment_size start_depth) in
        match comment_end with
            | None -> { len_t=100000000; rest_list=[]; token_opt=ERROR_TOKEN "Found end of file before end of nested comment"; }
            | Some comment_end_pos -> { len_t=comment_end_pos; rest_list=(wind_list total_list comment_end_pos); token_opt=NO_TOKEN; }

let process__main total_list = { len_t=5; rest_list=(wind_list total_list 5); token_opt=SOME_TOKEN (IDENTIFIER "_main"); }

let rec token_start_ends char_list =
    let rec token_start_ends token_list pos char_list = match char_list with
        | [] -> []
        | rest ->
            let process_fn = match rest with
                | [] -> process__main (*can never happen*)
                | '/'::'*'::rest -> process_comment
                | '"'::rest -> process_string
                | '_'::'m'::'a'::'i'::'n'::rest -> process__main
                | ch::rest ->
                    if (is_whitespace ch) then process_whitespace
                    else if (is_int ch) then process_int
                    else if (is_letter ch) then process_letter
                    else process_possible_operator
            in
            let proc_result = (process_fn char_list)
            in match proc_result.token_opt with
                | SOME_TOKEN (token) -> token_start_ends (token::token_list) proc_result.len_t proc_result.rest_list
                | NO_TOKEN -> token_start_ends token_list proc_result.len_t proc_result.rest_list
                (*| ERROR_TOKEN err_str -> []*)
    in
        token_start_ends [] 0


let str = load_file "arg.ocaml" ;;
let arg = Printf.printf "%s" str ;;
let char_list = list_car str ;;
let _ = List.iter (Printf.printf "%c") char_list ;;
let _ = List.iter (Printf.printf "%c") (split_tokens char_list) ;;
let line_list, line_start_list =  (process_line_positions char_list);;
(*let _ = Array.iter (Printf.printf "%d\n") line_list ;;*)
let _ = Array.iter (Printf.printf "%d\n")  line_start_list;;
let line_positions = process_line_positions char_list ;;
Printf.printf "%x" line_positions ;;
