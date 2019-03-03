# Barnacle_2

## What is Barnacle?

Barnacle is a simple scripting language I developed in C and C++. It was done mainly out of interest in parsing and language interpreters.

## How to Build Barnacle

### On Linux

1. Clone the master branch to your computer
2. cd into the "src" directory
3. Run the command "make" to build barnacle from the Makefile

### On Windows

1. Clone the master branch to your computer
2. Include all .cpp and .h files into a single project using your favorite IDE (I recommend Visual Studio)
3. Build the project

## Running Barnacle

You can use Barnacle by running the produced executable. You can wither run it without any arguments, or you can specify the path to a file as an argument.

### Barnacle without any arguments

When running Barnacle without any arguments, a prompt will appear that looks like this

```
Barnacle
>>> [Single line commands can be entered]
```

### Barnacle with a file argument

When running Barnacle by passing a file as an argument, Barnacle will execute whatever is inside the file in a terminal.

*Example:*

testfile.bar contents:
```
function foo argument_1:
        var i = 0;
        while i < argument_1:
                cout "Hello World!";
                cout endl;
                i = i + 1;
        endloop
endfunc

call foo 3;
```

Command in terminal:
```
$ ./bar testfile.bar
```

Output:
```
--Parser--
Hello World!
Hello World!
Hello World!
```
