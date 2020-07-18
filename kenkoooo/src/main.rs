use std::collections::{BTreeMap, VecDeque};
use std::fmt::Formatter;
use std::fs::File;
use std::io;
use std::io::{BufReader, Read};

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
    let mut map = BTreeMap::new();
    let file = File::open("../blob/galaxy.txt").unwrap();
    let mut buf_reader = BufReader::new(file);
    let mut contents = String::new();
    buf_reader.read_to_string(&mut contents).unwrap();
    for line in contents.split("\n") {
        let (left, right) = tokenize(line);
        assert_eq!(left.len(), 1);
        map.insert(left[0].clone(), right);
    }

    let mut q = VecDeque::new();
    q.push_back(":1338");
    let mut final_line = vec![];
    while let Some(a) = q.pop_front() {
        if &a[..1] == ":" {
            map.get(a)
                .unwrap()
                .iter()
                .rev()
                .for_each(|a| q.push_front(a.as_str()));
        } else {
            final_line.push(a);
        }
        eprintln!("{}", final_line.len());
    }
}
