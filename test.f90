!gfortran

program hello
implicit none
    REAL X
    print *, "(I3), 20"
    print *, "-------------"
    print "(I3)", 20
    print *, "--"

    print *, "(I3.3), 20"
    print *, "-------------"
    print "(I3.3)", 20
    print *, "--"

    print *, "(I5.3), 20"
    print *, "-------------"
    print "(I5.3)", 20
    print *, "--"

    print *, "(F7.3), 3.001345"
    print *, "-------------"
    print "(F7.3)", 3.001345
    print *, "--"

    print *, "(F7.0), 3.001345"
    print *, "-------------"
    print "(F7.0)", 3.001345
    print *, "--"

    print *, "(L4), true"
    print *, "-------------"
    print "(L4)", .TRUE.
    print *, "--"

    print *, "(L4), false"
    print *, "-------------"
    print "(L4)", .FALSE.
    print *, "--"

    print *, "(A), test"
    print *, "-------------"
    print "(A)", "test"
    print *, "--"

    print *, "(A8), abcdefg"
    print *, "-------------"
    print "(A8)", "abcdefg"
    print *, "--"

    print *, "(1A3), abcdefg"
    print *, "-------------"
    print "(A3)", "abcdefg"
    print *, "--"


	print *, "(3I3), 1, 2, 3"
    print *, "-------------"
    print "(3I3)", 1, 2, 3
    print *, "--"

    print *, "  (((  3I3  )))  , 1, 2, 3"
    print *, "-------------"
    print "  (((  3I3  )))  ", 1, 2, 3
    print *, "--"

    print *, "(3I3) ignored, 1, 2, 3"
    print *, "-------------"
    print "(3I3) ignored", 1, 2, 3
    print *, "--"

    print *, "(3(I3)), 1, 2, 3"
    print *, "-------------"
    print "(3(I3))", 1, 2, 3
    print *, "--"

    print *, "(I3, I3, I3), 1, 2, 3"
    print *, "-------------"
    print "(I3, I3, I3)", 1, 2, 3
    print *, "--"

    print *, "(I3, I3, I3)ignored, 1, 2, 3"
    print *, "-------------"
    print "(I3, I3, I3)", 1, 2, 3
    print *, "--"

    print *, "(3(I3, 3X)), 1, 2, 3"
    print *, "-------------"
    print "(3(I3, 3X))", 1, 2, 3
    print *, "--"

    print *, "(A, 'test'), different"
    print *, "-------------"
    print "(A, 'test')", "different "
    print *, "--"

    print *, "(5Htest , A), different"
    print *, "-------------"
    print "(5Htest , A)", "different"
    print *, "--"

    print *, "(F10.3), 3.001345"
    print *, "-------------"
    print "(F10.3)", 3.001345
    print *, "--"

    print *, "(D10.3), 3.001345"
    print *, "-------------"
    print "(D10.3)", 3.001345
    print *, "--"

    print *, "(E10.3), 3.001345"
    print *, "-------------"
    print "(E10.3)", 3.001345
    print *, "--"

    print *, "(G10.3), 3.001345"
    print *, "-------------"
    print "(G10.3)", 3.001345
    print *, "--"

    X =  .345678
    print *, "(F10.3, F10.1, F10.0, F5.5, F5.4), X=.345678"
    print *, "-------------"
    print "(F10.3, F10.1, F10.0, F5.5, F5.4)", X, X, X, X, X
    print *, "--"

    X =  1.345678
    print *, "(F10.3, F10.1, F10.0, F5.5, F5.4), X=1.345678"
    print *, "-------------"
    print "(F10.3, F10.1, F10.0, F5.5, F5.4)", X, X, X, X, X
    print *, "--"

    print *, "(F1.0), 0.0"
    print *, "-------------"
    print "(F1.0)", 0.0
    print *, "--"
end program hello