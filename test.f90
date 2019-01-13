!gfortran

program hello
implicit none
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

    print *, "(A3), abcdefg"
    print *, "-------------"
    print "(A3)", "abcdefg"
    print *, "--"
end program hello