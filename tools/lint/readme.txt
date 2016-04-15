             --------------------------------------------------
                   --- Manual Additions/Modifications ---
             --------------------------------------------------
                       PC-lint for C/C++ Version 8.00e

    This readme.txt supplements the on-line PC-lint manual entitled
    "Reference Manual for PC-lint/Flexelint" found in the installation
    directory under the name "pc-lint.pdf"


                          ------ What's New ------

    To find out what we've added to the product since Version 7.50,
    check out Chapter 18 "What's New" in the Reference Manual.


                           ------ Front End ------

    Your linting experience will be considerably enhanced by adapting
    your favorite editor or compiler environment to the task of
    sequencing from error to error.  See Section 3.5 of the Reference
    Manual.


                        ------ Multiple Passes ------

    By default, PC-lint/FlexeLint will go through all your modules in
    one pass.  For projects not previously linted there will be enough
    messages to look at.  However, with just one pass, you will not be
    taking full advantage of our new interfunction value tracking.  With
    just one pass, we will not know about dangerous return values for
    functions that are defined later than they are called, and we will
    not know about dangerous arguments for functions that are defined
    early.  To introduce a second pass you need only to add the command
    line option:

        -passes(2)

    or, if this syntax presents a problem with your Shell, you may use:

        -passes[2]

    or, in some cases,

        -passes=2

    is needed.

    See Section 9.2.2, "Interfunction Value Tracking".




             ------ Improved Exception Handling in 8.00e ------

    Earlier patch levels (8.00d and earlier) would report that a
    function could throw an exception yet failed to provide an exception
    specification (i.e., was not declared with a throw-list).  For
    example:

        void f() { throw X(); }

    would be greeted with a Warning suggesting that this might be
    changed to:

        void f() throw(X) { throw X(); }

    But the absence of an exception specification indicates that a
    function can potentially throw any exception.  Thus, the first
    version is perfectly safe.  Moreover, if the constructor for X()
    does not have an exception specification, which is the current
    programming norm, the first version is actually safer than the
    second because a thrown exception other than what is declared for
    can cause the dreaded UnexpectedException to be thrown in its place.
    See, for example, reference [23], Scott Myers "More Effective C++",
    Item 14.  Hence we now reserve these warnings for when the
    programmer has supplied an exception specification Thus:

        void f() throw(Y) { throw X(); }

    will receive a warning that X is not on the list; moreover, if
    X::X() can throw exceptions not confined to Y, then this too will be
    reported.

    The text of messages 1549, 1550 and 1560 have been altered to
    reflect this improved handling of exceptions.





                ------ New or Improved Error Messages ------


    686  Option 'String' is suspicious because of 'Name' -- An option is
         considered suspicious for one of a variety of reasons that is
         specified by Name.  At this writing, the only reason code is
         'unbalanced quotes'.

    1076 Anonymous union assumed to be 'static' -- Anonymous unions need
         to be declared static.  This is because the names contained
         within are considered local to the module in which they are
         declared.

    1549 Exception thrown for function 'Symbol' not declared to throw --
         An exception was thrown (i.e., a throw was detected) within a
         function and not within a try block;  moreover the function was
         declared to throw but the exception thrown was not on the list.
         If you provide an exception specification, include all the
         exception types you potentially will throw.  [23, Item 14]

    1550 exception 'Name' thrown by function 'Symbol' is not on
         throw-list of function 'Symbol' -- A function was called (first
         Symbol) which was declared as potentially throwing an
         exception.  The call was not made from within a try block and
         the function making the call had an exception specification.
         Either add the exception to the list, or place the call inside
         a try block and catch the throw.  [23, Item 14]

    1560 Uncaught exception 'Name' not on throw-list of function
         'Symbol' -- A direct or indirect throw of the named exception
         occurred within a try block and was either not caught by any
         handler or was rethrown by the handler.  Moreover, the function
         has an exception specification and the uncaught exception is
         not on the list.  Note that a function that fails to declare a
         list of thrown exceptions is assumed to potentially throw any
         exception.




                        Gimpel Software
                        November 2001
