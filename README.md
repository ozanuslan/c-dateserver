**T.C.**

**DOKUZ EYLUL UNIVERSTY**

**FACULTY OF ENGINEERING**

**DEPARTMENT OF COMPUTER ENGINEERING**

**2021 - 2022 FALL SEMESTER**

**CME 3205 OPERATING SYSTEMS**

**ASSIGNMENT 1**

**TIME SERVER**

**DUE DATE 23:55 – 08.01.2022**

In this assignment, you are asked to create a simple time server that takes a time request from a client using “telnet” console command on a Linux machine. Similar to our lab classes on socket communication, you will create a server that takes date request from a client and send the correct date data that is requested.

To learn more about date formats, you should read the man pages of “date” console command that is available via console command “man date” or at the following website (https://man7.org/linux/man-pages/man1/date.1.html). Your server will basically receive date requests, use “date” command to get correct output and send this output to the client. Examples of this operation are given below (assume the current time and date was 01:23:45 – Saturday – 01/01/2022).



|**CLIENT’S DATE REQUEST**|**SERVER’S RESPONSE**|
| - | - |
|GET\_DATE %H:%M:%S|01:23:45|
|GET\_DATE|INCORRECT REQUEST|
|<p>GET\_DATE %E</p><p>(Capital E as an option does not exist)</p>|INCORRECT REQUEST|
|GET\_DATE %Y/%m/%d - %H:%M:%S|2022/01/01 - 01:23:45|
You should be able to work with every option that is given in manual page of “date” console command. The basic function of your time server would be getting date format for “date” console command and returning the output to the client. However, if the given date format is not correct (contains incorrect or unavailable options for “date” console command) just send “INCORRECT REQUEST” to the client without needing any explanation of this reply.

Only one connection (one client) is enough for time server. You are not required to implement a multi user time server for this assignment.

**UPLOAD REQUIREMENTS:**

You are required to upload the C language code file you have written to the SAKAI. You should compile and test it to make sure it works before upload. You can use a IDE during development but your code must work correctly using console complication and execution commands (using “gcc” and “./”). If we cannot correctly compile and execute your code, your grade will be reduced according to grading criteria. You do not need to 

upload a complied version of your code, just your C language code, because we cannot prove if it is a original compilation or not. For this reason, uploading a complied file is not necessary nor it is requested.

The file you are required to upload are given below with an explanation and an example.

**(STUDENT\_NUMBER)\_(STUDENT\_NAME).c (Source code you have written in C language) Example = 2021510123\_FATIH\_DICLE.c**

Late or no submissions will be graded zero. You can see the basic grading table of this assignment below.



|**CRITERIA**|**GRADE**|
| - | - |
|Correct naming of upload files|10|
|Correct English variable names and English comments.|10|
|Correct Code Quality|10|
|Correct Code Execution and Successful Connection|10|
|Correct Date Output to Client|60|
|TOTAL GRADE|100|
|CHEATING OR ANY OTHER FORM OF PLAGIARISM|–∞|
If you have any questions or problems regarding this lab paper, you can ask about it in our lab sessions.

If you wish, you can also ask it in class forums or assignment page comments.

However, please do not send emails because that would lead to asking the same questions over and over again.

**GOOD LUCK TO YOU ALL!**
