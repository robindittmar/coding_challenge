# Greenbone Coding Challenge

This repository contains my progress and results of the
coding challenge, which is part of the Greenbone application process.



### Thoughts

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

