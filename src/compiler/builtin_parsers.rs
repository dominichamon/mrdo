use crate::compiler::expression_parsers::*;
use crate::compiler::tokens::Token;

use nom::types::CompleteStr;
use nom::*;

named!(pub builtin<CompleteStr, Token>,
    ws!(
        do_parse!(
            tag!("do") >>
            args: ws!(
                delimited!(
                    tag!("("),
                    tuple!(
                        alpha,
                        opt!(pair!(tag!(","), separated_list!(tag!(","), rvalue)))
                    ),
                    tag!(")")
                )) >>
            (
                {
                    match args.1 {
                        Some(rvs) => {
                            Token::Builtin{ builtin: args.0.to_string(), args: rvs.1 }
                        },
                        None => {
                            Token::Builtin{ builtin: args.0.to_string(), args: vec![] }
                        }
                    }
                }
            )
        )
    )
);

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_builtin_noargs() {
        let result = builtin(CompleteStr("do(foo)"));
        assert!(result.is_ok());
        let (_, token) = result.unwrap();
        assert_eq!(
            token,
            Token::Builtin {
                builtin: "foo".to_string(),
                args: vec![],
            }
        );
    }
    #[test]
    fn test_builtin_onearg() {
        let result = builtin(CompleteStr("do(write, 42.0)"));
        assert!(result.is_ok());
        let (_, token) = result.unwrap();
        assert_eq!(
            token,
            Token::Builtin {
                builtin: "write".to_string(),
                args: vec![Token::Expression {
                    left: Box::new(Token::Term {
                        left: Box::new(Token::Factor {
                            value: Box::new(Token::Real { value: 42.0 })
                        }),
                        right: vec![],
                    }),
                    right: vec![],
                }],
            }
        );
    }

    #[test]
    fn test_builtin_multiple_args() {
        let result = builtin(CompleteStr("do (read, foo, 1.0)"));
        assert!(result.is_ok());
        let (_, token) = result.unwrap();
        assert_eq!(
            token,
            Token::Builtin {
                builtin: "read".to_string(),
                args: vec![
                    Token::Expression {
                        left: Box::new(Token::Term {
                            left: Box::new(Token::Factor {
                                value: Box::new(Token::Identifier {
                                    name: "foo".to_string()
                                })
                            }),
                            right: vec![],
                        }),
                        right: vec![],
                    },
                    Token::Expression {
                        left: Box::new(Token::Term {
                            left: Box::new(Token::Factor {
                                value: Box::new(Token::Real { value: 1.0 })
                            }),
                            right: vec![],
                        }),
                        right: vec![],
                    }
                ]
            }
        );
    }
}
