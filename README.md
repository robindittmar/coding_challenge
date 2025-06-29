# Coding Challenge

This repository contains my result of a coding challenge, which is part of
an application process.

## Answering the questions

##### Can you figure out what is wrong with your decoder?

Yes! The decoder is either ignoring the `"isEncoded"` flag, or missing the **ROT13**
decoding step altogether.

(Technically, the program is also only iterating over the first
five messages :D)


##### How do you prove it?

The first piece of proof, for that the decoder is not decoding anything, is comparing the output of the program with
the contents of `messages.json`. The first five entries of the `[messages][i][message]` object from the JSON
are identical to the output shown in the task description PDF.

The second piece of proof is running the ROT13 cipher on the outputted text, either
manually or using tools like [rot13.com](https://rot13.com/)

The output of deciphering the encoded text is perfectly readable
(as long as you know English, of course)

There were some hints pointing towards a simple encoding like ROT13, as:
- Natural punctuation
- Letter and spaces distributions match those of natural language
- (find more detailed thoughts in "Thoughts/Log" section)


##### Can you write a program that reads the encoded messages in the JSON file, decodes and print them to stdout?

Yes, please find my source code and thought process in this repository :)


## Prerequisites

To build this project, `glib`, `cJSON` and [gvm-libs](https://github.com/greenbone/gvm-libs) must be installed.

#### System information

I developed this on Linux Mint 22.1 Cinnamon, using the kernel `6.8.0-62-generic`
along with
- `CLion 2025.1.3`
- `gcc (Ubuntu 13.3.0-6ubuntu2~24.04) 13.3.0`
- `cmake 3.31.6`


## Thoughts/Log

I've decided to write down my thoughts everytime I sit down to work on this.
The idea is that if you are reviewing this, you can get a deeper insight into when/how I come
up with the solutions I came up with.

#### 2025-06-23
The first thing I did was try to understand which cipher is being used
to encode/decode the messages.
I realized that the sentences look correct in its format (the first letter is upper case and the punctuation is correct),
and the very first two sentences are actually decoded correctly.

My initial idea was that the setting of the task (traveling through space **and time**) had to do with it.
Maybe the parser was skipping characters due to some memory corruption/off by one error or something along those lines.
If that was the case, though, it would be highly unlikely that the punctuation would stay intact.
In addition to that, I do not have any input JSON file, so it is impossible (or at least extremely hard) to derive some
weird memory corruption behavior from just the output data.

After some more inspection I realized that the second word `lbh'yy` contained a language construct
that we use in English quite often, as in for example: `you'll`, `we've`, `I'd`.
Also, the single uppercase `V` letter was a bit suspicious, so I thought of the simplest cipher known to people: **ROT13**.

So I just pasted the encoded text to rot13.com and voilà: We can read the text in plain English :)

#### 2025-06-24
Today I received the `messages.json` file, which makes it a lot clearer what
the exercise is about in detail. I feel confident now that I am able
to answer the questions of the task and write the code to parse and decode the provided JSON file
(especially since I already figured out the encoding is ROT13)

My idea currently is to
1. Parse the JSON file using `gvm-libs`
2. Transform message objects into a custom struct
3. Sort messages using the timestamp (id as fallback, if timestamps happen to be equal). Looking to use `qsort()`
4. Decode messages that have `"isEncoded": true` using a custom ROT13 encoder
(no explicit decoder necessary, as ROT13 is "symmetrical", just like XORing for example: if you run the operation twice, you'll receive back your original input)
5. Iterate over message objects and output decoded messages

#### 2025-06-25
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
- One issue I could potentially see with this is either creating hard dependencies
or making unnecessary copies of structs/values.

==> 2025-06-26: I have ultimately decided to decode the messages while parsing, BUT in a callback
that lives in the main code unit. This is in my opinion the best out of both worlds.
The `message_t` object can omit the "is_encoded" flag (since only decoded messages are stored), and
the parser does not need to know about `message_t` or any specific encoding (in other words, thinking
about it a bit differently, the cipher can be "injected" via the callback)

##### Should I abstract the message list further or use GList* directly?

- This is my first time utilizing `glib`, and I am not sure if I should use `GList*` directly.
There is an argument to be made, that creating a small abstraction layer on top would look nicer,
but using GList directly is not really verbose by itself.

==> 2025-06-26: I have decided to keep `GList*` without any further abstractions. But, using the
callback idiom in the parser, the parser does not need know about any list :)


#### 2025-06-26

I decided to add some error handling, which is never pretty in vanilla C.
Since imo there are various ways to handle errors properly, I have somewhat oriented
on the way `gvm_json_pull_expand_container(...)` does it.

Additionally, I have decided to run `valgrind` to find any memory leaks, and it seems
that I have made a mistake in using the `gvm-libs` json utility.
The memory in question is allocated as a result of calling `gvm_json_pull_parser_next`,
but I don't see any more cleanup that I would need to take care of.

That being said, I might just be missing something, and I am not sure if I will find the time to find and fix
these memory leaks.
I am going to add the valgrind output here for completeness's sake.

```
$ valgrind --tool=memcheck --leak-check=full ./coding_challenge

==12987== 63 bytes in 6 blocks are definitely lost in loss record 111 of 247
==12987==    at 0x4846828: malloc (in /usr/libexec/valgrind/vgpreload_memcheck-amd64-linux.so)
==12987==    by 0x49BA355: ??? (in /usr/lib/x86_64-linux-gnu/libcjson.so.1.7.17)
==12987==    by 0x49BBCD6: cJSON_ParseWithLengthOpts (in /usr/lib/x86_64-linux-gnu/libcjson.so.1.7.17)
==12987==    by 0x49DD056: gvm_json_pull_parse_buffered (jsonpull.c:217)
==12987==    by 0x49DD40E: gvm_json_pull_parse_string (jsonpull.c:320)
==12987==    by 0x49DD813: gvm_json_pull_parse_key (jsonpull.c:439)
==12987==    by 0x49DE21A: gvm_json_pull_parser_next (jsonpull.c:723)
==12987==    by 0x109B98: parse_message_json_file (parse.c:50)
==12987==    by 0x10974A: main (main.c:47)
==12987== 
==12987== 125 bytes in 5 blocks are definitely lost in loss record 231 of 247
==12987==    at 0x4846828: malloc (in /usr/libexec/valgrind/vgpreload_memcheck-amd64-linux.so)
==12987==    by 0x49BA355: ??? (in /usr/lib/x86_64-linux-gnu/libcjson.so.1.7.17)
==12987==    by 0x49BBCD6: cJSON_ParseWithLengthOpts (in /usr/lib/x86_64-linux-gnu/libcjson.so.1.7.17)
==12987==    by 0x49DD056: gvm_json_pull_parse_buffered (jsonpull.c:217)
==12987==    by 0x49DD40E: gvm_json_pull_parse_string (jsonpull.c:320)
==12987==    by 0x49DDCDF: gvm_json_pull_parse_value (jsonpull.c:577)
==12987==    by 0x49DE23D: gvm_json_pull_parser_next (jsonpull.c:729)
==12987==    by 0x109B98: parse_message_json_file (parse.c:50)
==12987==    by 0x10974A: main (main.c:47)
==12987== 
==12987== 1,200 bytes in 50 blocks are definitely lost in loss record 245 of 247
==12987==    at 0x484D953: calloc (in /usr/libexec/valgrind/vgpreload_memcheck-amd64-linux.so)
==12987==    by 0x48D27B1: g_malloc0 (in /usr/lib/x86_64-linux-gnu/libglib-2.0.so.0.8000.0)
==12987==    by 0x49DCBC4: gvm_json_pull_path_elem_new (jsonpull.c:26)
==12987==    by 0x49DDF25: gvm_json_pull_parse_value (jsonpull.c:630)
==12987==    by 0x49DE23D: gvm_json_pull_parser_next (jsonpull.c:729)
==12987==    by 0x109C94: json_read_messages_array (parse.c:71)
==12987==    by 0x109BDE: parse_message_json_file (parse.c:54)
==12987==    by 0x10974A: main (main.c:47)
```

---
After thinking about my solution for half a day, I have decided to refactor some parts of the code.
The message_t was doing too much business logic, in my opinion, and I wanted to move more of the logic to main.

In addition, the parser does not know about `message_t` anymore, but I rather introduced a
`parse_context_t`, and a `parse_message_callback_t`. Main is now fully responsible for maintaining the message `GList`

As a last note, I decided to not bother with the memory leaks. The way they are created makes me believe it
**could** be a bug in `gvm-libs` itself, or I am simply using it wrong. That being said, I don't think this detail will
make or break this coding challenge (I hope), and you probably know memory leaks can be a hassle to locate.

