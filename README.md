# RokuYoutubeKeyboard
Allows use of computer keyboard and curl utility to interact with Roku's Youtube application

Depends on <curl/curl.h>, acquired by apt-get install libcurl4-openssl-dev

Compiled on Debian 
  uname -a output: Linux deb-nb 4.9.0-6-amd64 #1 SMP Debian 4.9.88-1+deb9u1 (2018-05-07) x86_64 GNU/Linux
  gcc version 6.3.0 20170516 (Debian 6.3.0-18+deb9u1)
  command used:  gcc -lcurl roku.c -o roku
