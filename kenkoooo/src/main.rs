use std::fmt::Formatter;
use std::io;

fn tokenize(s: &str) -> (Vec<String>, Vec<String>) {
    let a = s.split("=").collect::<Vec<_>>();
    let left = a[0]
        .split_whitespace()
        .map(|s| s.to_string())
        .collect::<Vec<_>>();
    let right = a[1]
        .split_whitespace()
        .map(|s| s.to_string())
        .collect::<Vec<_>>();
    (left, right)
}

fn main() {
    loop {
        let mut buffer = String::new();
        io::stdin().read_line(&mut buffer).unwrap();
        if buffer.len() == 0 {
            break;
        }
        let (left, right) = tokenize(&buffer);
        let left = process(left);
        let right = process(right);
        eprintln!("{} {}", left.len(), right.len());
    }
}

fn process(mut tokens: Vec<String>) -> Vec<Element> {
    let mut stack = Vec::<Element>::new();
    while let Some(tail) = tokens.pop() {
        if tail.as_str() == "ap" {
            let mut last = stack.pop().unwrap();
            let prev = stack.pop().unwrap();
            last.arguments.push(Box::new(prev));
            stack.push(last);
        } else {
            let element = parse(tail);
            stack.push(element);
        }
    }
    assert_eq!(stack.len(), 0, "{:?}", stack);
    stack
}

fn parse(name: String) -> Element {
    let argument_count = match name.as_str() {
        "inc" | "dec" | "mod" | "dem" | "neg" | "pwr2" | "i" | "nil" => Some(1),
        "add" | "mul" | "div" | "eq" | "lt" | "t" | "f" | "car" | "cdr" => Some(2),
        "s" | "c" | "b" | "cons" | "if0" => Some(3),
        _ => None,
    };
    Element {
        name,
        arguments: Vec::new(),
        argument_count,
    }
}

type Pointer<T> = Option<Box<T>>;

enum Function {
    Increment(Pointer<Function>),
    Decrement(Pointer<Function>),
    Modulate(Pointer<Function>),
    Demodulate(Pointer<Function>),
    Negate(Pointer<Function>),
    Power2(Pointer<Function>),
    Identify(Pointer<Function>),
    Nil(Pointer<Function>),
    Add(Pointer<Function>, Pointer<Function>),
    Multiply(Pointer<Function>, Pointer<Function>),
    Divide(Pointer<Function>, Pointer<Function>),
    Equals(Pointer<Function>, Pointer<Function>),
    LessThan(Pointer<Function>, Pointer<Function>),
    True(Pointer<Function>, Pointer<Function>),
    False(Pointer<Function>, Pointer<Function>),
    Car(Pointer<Function>, Pointer<Function>),
    Cdr(Pointer<Function>, Pointer<Function>),
    S(Pointer<Function>, Pointer<Function>, Pointer<Function>),
    C(Pointer<Function>, Pointer<Function>, Pointer<Function>),
    B(Pointer<Function>, Pointer<Function>, Pointer<Function>),
    Cons(Pointer<Function>, Pointer<Function>, Pointer<Function>),
    If0(Pointer<Function>, Pointer<Function>, Pointer<Function>),
    Unknown(String, Vec<Function>),
}

struct Element {
    name: String,
    arguments: Vec<Box<Element>>,
    argument_count: Option<usize>,
}

impl std::fmt::Debug for Element {
    fn fmt(&self, f: &mut Formatter<'_>) -> std::fmt::Result {
        let name = if self.argument_count.is_some() {
            format!("<{}>", self.name)
        } else {
            format!("<{}|N>", self.name)
        };

        let mut a = f.debug_tuple(&name);
        if let Some(count) = self.argument_count {
            for i in 0..count {
                if self.arguments.len() <= i {
                    a.field(&"_".to_owned());
                } else {
                    a.field(self.arguments[i].as_ref());
                }
            }
        } else {
            self.arguments.iter().for_each(|e| {
                a.field(e);
            });
        }
        a.finish()
    }
}
