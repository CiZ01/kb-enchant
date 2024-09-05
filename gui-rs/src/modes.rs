use anyhow::Result;
use std::fs::OpenOptions;
use std::io::{Read, Write};

pub struct Mode {
    pub name: &'static str,
    pub icon: &'static str,
    pub value: u8,
}

pub const MODES: [&Mode; 3] = [
    &Mode {
        name: "Volume",
        icon: "audio-volume-high",
        value: 0,
    },
    &Mode {
        name: "Brigthness",
        icon: "colorfx",
        value: 1,
    },
    &Mode {
        name: "Mode 3",
        icon: "transform-move-vertical",
        value: 2,
    },
];

const CLASS_NAME: &str = "kb_ench";

/*
    Writes the value to the /sys/class/kb_ench/mode file.
*/
pub fn set_mode(value: u32) -> Result<()> {
    let mut file = OpenOptions::new()
        .write(true)
        .open(format!("/sys/class/{}/mode", CLASS_NAME))?;
    writeln!(file, "{}", value)?;
    Ok(())
}

/*
    Reads the value from the /sys/class/kb_ench/mode file.
*/
pub fn get_mode() -> Result<u8> {
    let mut file = OpenOptions::new()
        .read(true)
        .open(format!("/sys/class/{}/mode", CLASS_NAME))?;
    let mut buf = String::new();
    file.read_to_string(&mut buf)?;
    Ok(buf.trim().parse()?)
}
