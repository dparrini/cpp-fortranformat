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
end program hello