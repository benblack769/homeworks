
type token =
    | LITERAL
    | PLUS
    | TIMES
    | RIGHTPAREN
    | LEFTPAREN ;;

type symbol_set =
    | EXPR
    | PROD ;;
type term =
    | Terminal of token
    | Symbol of symbol_set ;;
type expansion = term list ;;
type expansion_set = expansion list ;;
type cfg = expansion_set list ;;

let test_cfg = [
    [
        [Symbol PROD, Terminal PLUS, Symbol EXPR],
        [Symbol PROD]
    ],
    [
        [Terminal LITERAL, Terminal TIMES, Symbol PROD],
        [Terminal LITERAL],
        [Terminal LEFTPAREN, Symbol EXPR, Terminal RIGHTPAREN]
    ]
] ;;
