# JeanLogger

This is a fast and dirty example of spyware.

## Note

I am not responsible for the use of this code.

## Where it does work

Tested on Windows 10

## Why and how

Needed to spy myself, of course. I made this on the fly on a few hours.

It is not perfect code, as I said I made this on the fly and there are probably aspects to improve, such as implementations on header files, but it works.


## Features

- Captures Chrome saved form data (url, user, password), and e-mails it

- Uses libcurl to send an e-mail via SMTP auth. Works with Google "smtp://smtp.gmail.com:587". Modify send_mail

- Cast a key logging thread, and e-mails the lines captured each 10 minutes if there is data to send.

- Installs itself on startup registry.

- Tries to avoid some AV detecting sandboxes, and some dynamic import is started. Avast detects it some seconds after it has already sent Chrome data, and if you are installing it, you can always add the exception to the AV. Your AV.


## Compile

Not every libraries are statically compiled. You are free to finish the work.

gcc -o main.exe -Lcurl\lib main.cpp dynimp.cpp sqlite3.c -w -lws2_32 -lcurl.dll -lstdc++ -fpermissive -lcrypt32 -static-libgcc -lstdc++ -static

Ah yeh, you will need openssl installed



## How to use

- Configure send_data.h
- Improve the code and merge on github, then,
- Compile
- Put it on a folder together with the dlls
- Execute and spy yourself
