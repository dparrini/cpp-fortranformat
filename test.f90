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
end program hello