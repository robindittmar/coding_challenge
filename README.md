# Greenbone Coding Challenge

This repository contains my progress and results of the
coding challenge, which is part of the Greenbone application process.

### Prerequisites

To build this project, [gvm-libs](https://github.com/greenbone/gvm-libs) must be installed.

#### System information

I developed this on Linux Mint 22.1 Cinnamon, using the kernel `6.8.0-62-generic`
along with
- `CLion 2025.1.3`
- `cc (Ubuntu 13.3.0-6ubuntu2~24.04) 13.3.0`
- `cmake 3.31.6`


### Thoughts/Log

I've decided to write down my thoughts everytime I sit down to work on this.
The idea is that if you are reviewing this, you can get a deeper insight into when/how I come
up with the solutions I came up with.

#### 2025-06-23
The first thing I did was try to understand which cypher is being used
to encode/decode the messages.
I realized that the sentences look correct in its format (First letter is upper case and the punctuation is correct),
and the very first two sentences are actually decoded correctly.

My initial idea was that the setting of the task (travelling through space **and time**) had to do with it.
Maybe the parser was skipping characters due to some memory corruption/off by one error or something along those lines.
If that was the case though, it would be highly unlikely that the punctuation would stay intact.
In addition to that, I do not have any input JSON file, so it is impossible (or at least extremely hard) to derive some weird memory corruption
behavior from just the output data.

After some more inspection I realized that the second word `lbh'yy` contained a language construct
that we use in English quite often, as in for example: `you'll`, `we've`, `I'd`.
Also, the single uppercase `V` letter was a bit suspicious, so I thought of the simplest cypher known to mankind: **ROT13**.

So I just pasted the encoded text to rot13.com and voilá: We can read the text in plain english :)

#### 2025-06-24
Today I received the `messages.json` file, which makes it a lot clearer what
the exercise is about in detail. I feel confident now that I am able
to answer the questions of the task and write the code to parse & decode said JSON file
(especially since I already figured out the encoding is ROT13)

My idea currently is to
1. Parse the JSON file using `gvm-libs`
2. Transform message objects into custom struct
3. Sort messages using the timestamp (id as fallback, if timestamps happen to be equal). Looking to use `qsort()`
4. Decode messages that have `"isEncoded": true` using a custom ROT13 encoder
(no explicit decoder necessary, as ROT13 is "symmetrical", just like XORing for example: if you run the operation twice you'll receive back your original input)
5. Iterate over message objects and output decoded messages
