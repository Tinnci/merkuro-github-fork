// SPDX-FileCopyrightText: 2022 Carl Schwan <carlschwan@kde.org>
//
// SPDX-License-Identifier: LGPL-2.0-or-later

extern crate ammonia;
extern crate maplit;
extern crate html2text;
extern crate cxx;

use ammonia::Builder;
use maplit::hashset;
use html2text::from_read;
use std::io::Cursor;
use String;

fn html_clean(document: &str) -> String {
    let tag_content_blocklist = hashset!["script", "style", "head"];
    let tags_allowlist = hashset!["a", "address", "b", "big", "blockquote",
        "br", "center", "cite", "code", "dd", "dfn", "div", "dl", "dt",
        "em", "font", "h1", "h2", "h3", "h4", "h5", "h6", "hr", "i",
        "img", "kdb", "li", "nobr", "ol", "p", "pre", "s", "samp",
        "smal", "span", "strong", "sub", "sup", "u", "ul"];

    Builder::default()
        .clean_content_tags(tag_content_blocklist)
        .tags(tags_allowlist)
        .clean(document)
        .to_string()
}

fn html_to_text(document: &str, width: usize) -> String {
    from_read(Cursor::new(document), width)
}

#[cxx::bridge]
mod ffi {
    extern "Rust" {
        fn html_clean(document: &str) -> String;
        fn html_to_text(document: &str, width: usize) -> String;
    }
}

