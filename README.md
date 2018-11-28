# OTP

This project contains five small programs that encrypt and decrpyt information using a one-time pad-like system. These programs will combine multi-processing code and socket-based inter-process communication. The programs will also be accessible from the command line using standard UNIX features like input/output redirection and job control. The project will serve as a capstone to the topics I have learned in the class: Oregon State University - CS 344, Operating Systems.

## Compile

In root directory, run the compilation script

```bash
$ compileall
```

## Usage

### keygen

`keygen` creates a key file of specified length where the characters in the file generated will be any of the 27 allowed characters (A-Z and a space character). The syntax for `keygen` is as follows:

```bash
keygen keylength
```

In the syntax above, _keylength_ is the length of the key file in characters. `keygen` outputs to stdout. Below is an example run, which redirects stdout to a key file of 256 characters called "mykey" (mykey is 257 characters long because of the newline):

```bash
$ keygen 256 > mykey
```

### otp_enc_d

`otp_enc_d` will run in the background as a daemon. Upon execution, `otp_enc_d` must output an error if it cannot be run due to a network error, such as the ports being unavailable. Its function is to perform the actual encoding. This program will listen on a particular port/socket, assigned when it is first ran. The syntax for `otp_enc_d` is as follows:

```bash
otp_enc_d listening_port
```

In the syntax above, _listening_port_ is the port that `otp_enc_d` should listen on. You will always start otp_enc_d in the background, as follows (the port 57171 is just an example; you should be able to use any port):

```bash
$ otp_enc_d 57171 &
```

### otp_enc

`otp_enc` connects to `otp_enc_d` and asks it to perform a one-time pad style encryption. The syntax for `otp_enc` is as follows:

```bash
otp_enc plaintext key port
```

In the syntax above, _plaintext_ is the name of a file in the current directory that contains the plaintext you wish to encrypt. _key_ contains the encryption key you wish to use to encrypt the text. _port_ is the port that `otp_enc` should attempt to connect to `otp_enc_d` on.

When `otp_enc` receives the ciphertext back from `otp_enc_d`, it should output it to stdout. Thus, `otp_enc` can be launched in any of the following methods, and should send its output appropriately:

```bash
$ otp_enc myplaintext mykey 57171
$ otp_enc myplaintext mykey 57171 > myciphertext
$ otp_enc myplaintext mykey 57171 > myciphertext &
```

### otp_dec_d

This program performs exactly like `otp_enc_d`, in syntax and usage. In this case, however, `otp_dec_d` will decrypt ciphertext it is given, using the passed-in ciphertext and key. Thus, it returns plaintext again to `otp_dec`.

### otp_dec

Similarly, this program will connect to `otp_dec_d` and will ask it to decrypt ciphertext using a passed-in ciphertext and key, and otherwise performs exactly like `otp_enc`, and must be runnable in the same three ways.

### p4gradingscript

This is the grading script ran against the program to make sure that it meets all requirements. The syantax for `p4gradingscript` is:

```bash
$ ./p4gradingscript PORT1 PORT2 > mytestresults 2>&1
```
