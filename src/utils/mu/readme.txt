MU (Make Utility)

This utility is used by the make system build Pegasus. It implements a
handful of commands which may be executed in this form:

    mu <command> <arguments>

For example, here's how to remove a file:

    mu rm myfile.txt

This utility handles Unix C-Shell style wild cards. The following commands
are supported:

    rm - removes one or more files.

    rmdirhier - removes a single directory (and the hierarchy below it).

    mkdirhier - creates all directories along a path.

    echo - echos its arguments to standard output.

    touch - changes modification time of file to current time; creates file 
	if it does not already exist.

    pwd - prints the working directory with Unix style slashes ('/').

    copy - copies files.

    move - moves a file (possibliy renaming it).

    compare - compares two files; exits with code of zero if identical.

    depend - builds header dependencies for C++ source files.

    strip - strips lines from a file.

    prepend - prepends lines to a file.

