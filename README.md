# Greenbone Coding Challenge

This repository contains my progress and results of the
coding challenge, which is part of the Greenbone application process.

## Answering the questions

##### Can you figure out what is wrong with your decoder?

Yes! The decoder is either ignoring the `"isEncoded"` flag, or missing the **ROT13**
decoding step altogether. Technically, is it also only iterating over the first
five messages :)


##### How do you prove it?

The first piece of proof is comparing the output of the program with the contents of the
`messages.json` file. The first five entries of the `[messages][i][message]` object
are identical to the outputted text.

The second piece of proof is running the ROT13 cipher on the outputted text, either
manually or utilizing tools like [rot13.com](https://rot13.com/)

The output of deciphering the encoded text is perfectly readable
(as long as you know English, of course)


##### Can you write a program that reads the encoded messages in the JSON file, decodes and print them to stdout?

Yes, please find my source code and thought process in this repository :)


### Prerequisites

To build this project, [gvm-libs](https://github.com/greenbone/gvm-libs) must be installed.

#### System information

I developed this on Linux Mint 22.1 Cinnamon, using the kernel `6.8.0-62-generic`
along with
- `CLion 2025.1.3`
- `gcc (Ubuntu 13.3.0-6ubuntu2~24.04) 13.3.0`
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

#### 2025-06-26
I managed to understand the usage of the `jsonpull` util from `gvm-libs` and also managed to
get a running example of the exercise.
I have also decided to answer the questions asked in the document, for completeness’s sake. (Even though I believe the
coding part is way more interesting for both parties)

Later I have started cleaning up the code, splitting it into meaningful units and started documenting the functions.

I am planning to clean the code up a lot more, but I would like to point out two things I am uncertain about at this
time:

##### Should the messages be decoded while parsing the JSON, or should it be an extra step?

- My reasoning is just separating concerns. It would be nice to have a main function that
reads something like
  1. parse_json()
  2. process_messages()
  3. print_messages()

##### Should I abstract the message list further, or use GList* directly?

- This is my first time utilizing `glib`, and I am not sure if I should use `GList*` directly.
There is an argument to be made, that creating a small abstraction layer on top would look nicer,
but using GList directly is not really verbose by itself.
