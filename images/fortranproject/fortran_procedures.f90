!**************************************************************************************
!
! This file is part of FortranProject plugin for Code::Blocks IDE.
! It contains list of keywords and intrinsic procedures which are included in code-completion list.
!
! Description of procedures is based on GNU Fortran user manual.
!
! The file is licensed under the GNU General Public License, version 3
! http://www.gnu.org/licenses/gpl-3.0.html
!
! Author: Darius Markauskas
!
!**************************************************************************************

function list_of_other_fortran_keywords()
	! This pseudo function contains fortran keywords, which should be included in code-completion list, but
	! they are not functions or subroutines. The keywords are defined as variables of a type 'other'.
	type(other) :: logical, real, access, action, advance, allocatable, allocate, &
                    access, action, advance, allocatable, allocate, &
                apostrophe, assign, assignment, associate, asynchronous, backspace, &
                bind, blank, blockdata, call, case, character, class, close, common, &
                complex, contains, continue, cycle, data, deallocate, decimal, delim, &
                default, dimension, direct, do, dowhile, double, doubleprecision, else, &
                elseif, elsewhere, encoding, end, endassociate, endblockdata, enddo, &
                endfile, endforall, endfunction, endif, endinterface, endmodule, endprogram, &
                endselect, endsubroutine, endtype, endwhere, entry, eor, equivalence, &
                err, errmsg, exist, exit, external, file, flush, fmt, forall, form, format, &
                formatted, function, go, goto, id, if, implicit, in, include, inout, &
                integer, inquire, intent, interface, intrinsic, iomsg, iolength, &
                iostat, kind, len, logical, module, named, namelist, nextrec, nml, &
                none, nullify, number, only, open, opened, operator, optional, out, pad, &
                parameter, pass, pause, pending, pointer, pos, position, precision, &
                print, private, program, protected, public, quote, read, readwrite, &
                real, rec, recl, recursive, result, return, rewind, save, select, &
                selectcase, selecttype, sequential, stat, status, stop, stream, &
                subroutine, target, then, to, type, unformatted, unit, use, value, &
                volatile, wait, where, while, write, procedure, elemental, pure, sequence, &
                import, is, &
                null, new_line,  block, abstract, delegate, static, reference, round, &
                decorate, extends, extensible, generic, non_overridable, enum, endenum, enumerator, typealias, &
                submodule, endsubmodule, concurrent, contiguous, re, im, endblock, non_intrinsic

end function


!**************************************************************************************
!
! List of intrinsic procedures
!
!**************************************************************************************

function ABS(A)
    ! Computes the absolute value of A.
    ! Arguments:
    ! A   -The type of the argument shall be an INTEGER, REAL, or COMPLEX.
    integer, real, complex :: A
end function

character function ACHAR(I [, KIND])
    ! Returns the character located at position 'i' in the ASCII collating sequence.
    ! Syntax: RESULT = ACHAR(I [, KIND])
    ! Arguments:
    ! i   -The type shall be INTEGER.
    ! KIND (Optional) An INTEGER initialization expression indicating the kind parameter of the result.
    ! Return value:
    ! The return value is of type CHARACTER with a length of one.
    ! If the KIND argument is present, the return value is of the specified kind and of the default kind otherwise.
    ! Standard: Fortran 77 and later, with KIND argument Fortran 2003 and later.
    integer :: I, KIND
end function

function ACOS(X)
    ! Computes the arccosine of X (inverse of COS(X)).
    ! Arguments: X 	-The type shall either be REAL with a magnitude that is less than or equal to one - or the type shall be COMPLEX.
    ! Return value:
    ! The return value is of the same type and kind as X. The real part of the result
    ! is in radians and lies in the range 0 <= acos(x) <= pi.
    ! Standard: Fortran 77 and later, for a complex argument Fortran 2008 or later
    real, complex :: ACOS, X
end function

double precision function DACOS(X)
    ! This is specific procedure name of ACOS function.
    ! Computes the arccosine of X (inverse of COS(X)).
    ! Arguments: X 	-The type shall be DOUBLEPRECISION.
    ! Return value:
    ! The return value is of the same type and kind as X. The real part of the result
    ! is in radians and lies in the range 0 <= acos(x) <= pi.
    ! Standard: Fortran 77 and later
    double precision :: X
end function

function ACOSH(X)
    ! Computes the hyperbolic arccosine of X (inverse of COSH(X)).
    ! Syntax: RESULT = ACOSH(X)
    ! Arguments: X 	-The type shall be REAL or COMPLEX.
    ! Return value:
    ! The return value has the same type and kind as X. If X is complex, the imaginary part of the result is in radians
    ! and lies between 0 <= acosh(x) <= pi.
    ! Standard: Fortran 2008 and later.
    real, complex :: ACOSH, X
end function

character(len=*) function ADJUSTL(STRING)
    ! ADJUSTL(STRING) will left adjust a string by removing leading spaces.
    ! Spaces are inserted at the end of the string as needed.
    ! Arguments: STRING  -The type shall be CHARACTER.
    ! Standard: Fortran 90 and later.
    character(len=*) :: STRING
end function

character(len=*) function ADJUSTR(STRING)
    ! ADJUSTR(STRING) will right adjust a string by removing trailing spaces. Spaces are inserted at the start
    ! of the string as needed.
    ! Arguments: STRING  -The type shall be CHARACTER.
    ! Standard: Fortran 95 and later.
    character(len=*) :: STRING
end function

real(kind as Z) function AIMAG(Z)
    ! AIMAG(Z) yields the imaginary part of complex argument Z.
    ! Arguments: Z 	-The type of the argument shall be COMPLEX.
    ! Return value: The return value is of type REAL with the kind type parameter of the argument.
    ! Standard: Fortran 77 and later
    complex :: Z
end function

real function AINT(A [, KIND])
    ! Real value truncated to a whole number.
    ! Return the largest whole number whose magnitude is less than or equal to |A|
    ! and whose sign is the same as A.
    real :: A
    integer :: KIND
end function

double precision function DINT(A)
    ! Real value truncated to a whole number.
    ! Return the largest whole number whose magnitude is less than or equal to |A|
    ! and whose sign is the same as A.
    double precision :: A
end function

logical function ALL(MASK [, DIM])
    ! Determines if all the values are true in MASK in the array along dimension DIM.
    ! Arguments:
    ! MASK 	-The type of the argument shall be LOGICAL and it shall not be scalar.
    ! DIM 	-(Optional) DIM shall be a scalar integer with a value that lies between one and the rank of MASK.
    ! Return value:
    ! ALL(MASK) returns a scalar value of type LOGICAL where the kind type parameter is the same as the kind type
    !    parameter of MASK. If DIM is present, then ALL(MASK, DIM) returns an array with the rank of
    !    MASK minus 1. The shape is determined from the shape of MASK where the DIM dimension is elided.
    ! (A)
    !   ALL(MASK) is true if all elements of MASK are true. It also is true if MASK has zero size; otherwise, it is false.
    ! (B)
    !   If the rank of MASK is one, then ALL(MASK,DIM) is equivalent to ALL(MASK). If the rank is greater than one,
    !   then ALL(MASK,DIM) is determined by applying ALL to the array sections.
    ! Standard: Fortran 95 and later.
    logical :: MASK(:[,...])
    integer :: DIM
end function

logical function ALLOCATED(ARRAY)
    ! Checks the status of whether X is allocated.
    ! Arguments:
    ! ARRAY  -The argument shall be an ALLOCATABLE array.
    ! Return value:
    ! The return value is a scalar LOGICAL with the default logical kind type parameter.
    ! If ARRAY is allocated, ALLOCATED(ARRAY) is .TRUE.; otherwise, it returns .FALSE.
    ! Standard: Fortran 95 and later.
    type(any_allocatable_array) :: ARRAY 
end function

real function ANINT(A [, KIND])
    ! Rounds its argument to the nearest whole number.
    ! Arguments:
    ! A   -The type of the argument shall be REAL.
    ! KIND   -(Optional) An INTEGER initialization expression indicating the kind parameter of the result.
    ! Return value:
    ! The return value is of type real with the kind type parameter of the argument if the optional KIND is absent;
    ! otherwise, the kind type parameter will be given by KIND. If A is greater than zero, ANINT(A) returns AINT(X+0.5).
    ! If A is less than or equal to zero then it returns AINT(X-0.5).
    ! Standard: Fortran 77 and later.
    real :: A
    integer :: KIND
end function

double precision function DNINT(A)
    ! Rounds its argument to the nearest whole number.
    ! Arguments:
    ! A   -The type of the argument shall be DOUBLE PRECISION.
    ! Return value:
    ! The return value is of type real with the kind type parameter of the argument if the optional KIND is absent;
    ! otherwise, the kind type parameter will be given by KIND. If A is greater than zero, ANINT(A) returns AINT(X+0.5).
    ! If A is less than or equal to zero then it returns AINT(X-0.5).
    ! Standard: Fortran 77 and later.
    double precision :: A
end function

logical function ANY(MASK [, DIM])
    ! Determines if any of the values in the logical array MASK along dimension DIM are .TRUE..
    ! Arguments:
    ! MASK   -The type of the argument shall be LOGICAL and it shall not be scalar.
    ! DIM  -(Optional) DIM shall be a scalar integer with a value that lies between one and the rank of MASK.
    ! Return value:
    ! ANY(MASK) returns a scalar value of type LOGICAL where the kind type parameter is the same as the kind type
    ! parameter of MASK. If DIM is present, then ANY(MASK, DIM) returns an array with the rank of MASK minus 1.
    ! The shape is determined from the shape of MASK where the DIM dimension is elided.
    ! (A)
    !   ANY(MASK) is true if any element of MASK is true; otherwise, it is false. It also is false if MASK has zero size.
    ! (B)
    !   If the rank of MASK is one, then ANY(MASK,DIM) is equivalent to ANY(MASK). If the rank is greater than one,
    !   then ANY(MASK,DIM) is determined by applying ANY to the array sections.
    ! Standard: Fortran 95 and later.
    logical :: MASK(:[,...])
    integer :: DIM
end function

function ASIN(X)
    ! Computes the arcsine of its X (inverse of SIN(X)).
    ! Arguments:
    ! X   -The type shall be either REAL and a magnitude that is less than or equal to one - or be COMPLEX.
    ! Return value:
    ! The return value is of the same type and kind as X. The real part of the result is in radians and
    ! lies in the range -pi/2 <= asin(x) <= pi/2.
    ! Standard: Fortran 77 and later, for a complex argument Fortran 2008 or later.
    real, complex :: ASIN, X
end function

function DASIN(X)
    ! This is specific name of ASIN function.
    ! Computes the arcsine of its X (inverse of SIN(X)).
    ! Arguments:
    ! X   -The type shall be DOUBLE PRECISION and a magnitude that is less than or equal to one.
    ! Return value:
    ! The return value is of the same type and kind as X. The real part of the result is in radians and
    ! lies in the range -pi/2 <= asin(x) <= pi/2.
    ! Standard: Fortran 77 and later
    double precision :: DASIN, X
end function

function ASINH(X)
    ! Computes the hyperbolic arcsine of X (inverse of SINH(X)).
    ! Arguments:
    ! X     -The type shall be REAL or COMPLEX.
    ! Return value:
    ! The return value is of the same type and kind as X. If X is complex, the imaginary part of the result
    ! is in radians and lies between -pi/2 <= asinh(x) <= pi/2.
    ! Standard: Fortran 2008 and later
    real, complex :: ASINH, X
end function

logical function ASSOCIATED(POINTER [, TARGET])
    ! Determines the status of the pointer POINTER or if POINTER is associated with the target TARGET.
    ! Arguments:
    ! POINTER 	-POINTER shall have the POINTER attribute and it can be of any type.
    ! TARGET    -(Optional) TARGET shall be a pointer or a target. It must have the same type,
    !            kind type parameter, and array rank as POINTER.
    ! The association status of neither POINTER nor TARGET shall be undefined.
    ! Return value:
    ! ASSOCIATED(POINTER) returns a scalar value of type LOGICAL(4). There are several cases:
    ! (A) When the optional TARGET is not present then
    !     ASSOCIATED(POINTER) is true if POINTER is associated with a target; otherwise, it returns false.
    ! (B) If TARGET is present and a scalar target, the result is true if
    !     TARGET is not a zero-sized storage sequence and the target associated with POINTER occupies
    !     the same storage units. If POINTER is disassociated, the result is false.
    !(C) If TARGET is present and an array target, the result is true if
    !     TARGET and POINTER have the same shape, are not zero-sized arrays, are arrays whose elements
    !     are not zero-sized storage sequences, and TARGET and POINTER occupy the same storage units
    !     in array element order. As in case(B), the result is false, if POINTER is disassociated.
    !(D) If TARGET is present and an scalar pointer, the result is true
    !    if TARGET is associated with POINTER, the target associated with TARGET are not zero-sized storage
    !    sequences and occupy the same storage units. The result is false, if either TARGET or POINTER is disassociated.
    !(E) If TARGET is present and an array pointer, the result is true if
    !    target associated with POINTER and the target associated with TARGET have the same shape,
    !    are not zero-sized arrays, are arrays whose elements are not zero-sized storage sequences, and TARGET
    !    and POINTER occupy the same storage units in array element order. The result is false, if either TARGET
    !    or POINTER is disassociated.
    ! Standard: Fortran 95 and later
    type(any_pointer) :: POINTER, TARGET
end function

function ATAN(X)
    ! Computes the arctangent of X.
    ! Syntax:
    ! RESULT = ATAN(X) RESULT = ATAN(Y, X)
    ! Arguments:
    ! X  -The type shall be REAL or COMPLEX; if Y is present, X shall be REAL.
    ! Y  -shall be of the same type and kind as X.
    ! Return value:
    ! The return value is of the same type and kind as X. If Y is present, the result is identical to ATAN2(Y,X).
    ! Otherwise, it the arcus tangent of X, where the real part of the result is in radians and lies
    ! in the range -pi/2 <= atan(x) <= pi/2.
    ! Standard: Fortran 77 and later, for a complex argument and for two arguments Fortran 2008 or later.
    real, complex :: ATAN, X
end function

function DATAN(X)
    ! This is specific name of function ATAN.
    ! Computes the arctangent of X.
    ! Arguments:
    ! X  -The type shall be DOUBLE PRECISION
    ! Return value:
    ! The return value is of the same type and kind as X. If Y is present, the result is identical to ATAN2(Y,X).
    ! Otherwise, it the arcus tangent of X, where the real part of the result is in radians and lies
    ! in the range -pi/2 <= atan(x) <= pi/2.
    ! Standard: Fortran 77 and later
    double precision :: DATAN, X
end function

real function ATAN2(Y, X)
    ! Computes the principal value of the argument function of the complex number X + i Y.
    ! This function can be used to transform from carthesian into polar coordinates and allows to determine
    ! the angle in the correct quadrant.
    ! Arguments:
    ! Y   -The type shall be REAL.
    ! X   -The type and kind type parameter shall be the same as Y. If Y is zero, then X must be nonzero.
    ! Return value:
    ! The return value has the same type and kind type parameter as Y. It is the principal value of the complex
    ! number X + i Y. If X is nonzero, then it lies in the range -pi <= atan(x) <= pi. The sign is positive
    ! if Y is positive. If Y is zero, then the return value is zero if X is positive and \pi if X is negative.
    ! Finally, if X is zero, then the magnitude of the result is pi/2.
    ! Standard: Fortran 77 and later.
    real :: Y, X
end function

double precision function DATAN2(Y, X)
    ! This is specific name of ATAN2 function.
    ! Computes the principal value of the argument function of the complex number X + i Y.
    ! This function can be used to transform from carthesian into polar coordinates and allows to determine
    ! the angle in the correct quadrant.
    ! Arguments:
    ! Y   -The type shall be DOUBLE PRECISION.
    ! X   -The type and kind type parameter shall be the same as Y. If Y is zero, then X must be nonzero.
    ! Return value:
    ! The return value has the same type and kind type parameter as Y. It is the principal value of the complex
    ! number X + i Y. If X is nonzero, then it lies in the range -pi <= atan(x) <= pi. The sign is positive
    ! if Y is positive. If Y is zero, then the return value is zero if X is positive and \pi if X is negative.
    ! Finally, if X is zero, then the magnitude of the result is pi/2.
    ! Standard: Fortran 77 and later.
    DOUBLE PRECISION :: Y, X
end function

function ATANH(X)
    ! Computes the hyperbolic arctangent of X (inverse of TANH(X)).
    ! Arguments:
    ! X  -The type shall be REAL or COMPLEX.
    ! Return value:
    ! The return value has same type and kind as X. If X is complex, the imaginary part of the result is
    ! in radians and lies between -pi/2 <= atanh(x) <= pi/2.
    ! Standard: Fortran 2008 and later.
    real, complex :: ATANH, X
end function

real function BESSEL_J0(X)
    ! Computes the Bessel function of the first kind of order 0 of X.
    ! Arguments:
    ! X   -The type shall be REAL, and it shall be scalar.
    ! Return value:
    ! The return value is of type REAL and lies in the range - 0.4027... <= Bessel(0,x) <= 1.
    ! It has the same kind as X.
    ! Standard: Fortran 2008 and later.
    real :: X
end function

real function BESSEL_J1(X)
    ! Computes the Bessel function of the first kind of order 1 of X.
    ! Arguments:
    ! X    -The type shall be REAL, and it shall be scalar.
    ! Return value:
    ! The return value is of type REAL and it lies in the range -0.5818... <= Bessel(0,x) <= 0.5818 .
    ! It has the same kind as X.
    ! Standard: Fortran 2008 and later.
    real :: X
end function

real function BESSEL_JN(N, X)
    ! Computes the Bessel function of the first kind of order N of X.
    ! Arguments:
    ! N  -Shall be a scalar or an array of type INTEGER.
    ! X  -Shall be a scalar or an array of type REAL.
    ! Return value:
    ! The return value is a scalar of type REAL. It has the same kind as X.
    ! Standard: Fortran 2008 and later.
    integer :: N
    real :: X
end function

real function BESSEL_Y0(X)
    ! Computes the Bessel function of the second kind of order 0 of X.
    ! Arguments:
    ! X  -The type shall be REAL, and it shall be scalar.
    ! Return value:
    ! The return value is a scalar of type REAL. It has the same kind as X.
	! Standard: Fortran 2008 and later.
	real :: X
end function

real function BESSEL_Y1(X)
    ! Computes the Bessel function of the second kind of order 1 of X.
    ! Arguments:
    ! X  -The type shall be REAL, and it shall be scalar.
    ! Return value:
    ! The return value is a scalar of type REAL. It has the same kind as X.
	! Standard: Fortran 2008 and later.
	real :: X
end function

real function BESSEL_YN(N, X)
    ! Computes the Bessel function of the second kind of order N of X.
    ! Arguments:
    ! N  -Shall be a scalar or an array of type INTEGER.
    ! X  -Shall be a scalar or an array of type REAL.
    ! If both arguments are arrays, their ranks and shapes shall conform.
    ! Return value:
    ! The return value is a scalar of type REAL. It has the same kind as X.
    integer :: N
    real :: X
end function

integer function BIT_SIZE(I)
    ! Returns the number of bits (integer precision plus sign bit) represented by the type of I.
    ! The result of BIT_SIZE(I) is independent of the actual value of I.
    ! Arguments:
    ! I  -The type shall be INTEGER.
    ! Return value:
    ! The return value is of type INTEGER.
    ! Standard: Fortran 95 and later.
    integer :: I
end function

logical function BTEST(I, POS)
    ! Returns logical .TRUE. if the bit at POS in I is set. The counting of the bits starts at 0.
    ! Arguments:
    ! I  -The type shall be INTEGER.
    ! POS  -The type shall be INTEGER.
    ! Return value:
    ! The return value is of type LOGICAL
    ! Standard: Fortran 95 and later.
    integer :: I, POS
end function

logical function C_ASSOCIATED(c_prt_1 [, c_ptr_2])
    ! Determines the status of the C pointer c_ptr_1 or if c_ptr_1 is associated with the target c_ptr_2.
    ! Arguments:
    ! c_ptr_1   -Scalar of the type C_PTR or C_FUNPTR.
    ! c_ptr_2   -(Optional) Scalar of the same type as c_ptr_1.
    ! Return value:
    ! The return value is of type LOGICAL; it is .false. if either c_ptr_1 is a C NULL pointer
    ! or if c_ptr1 and c_ptr_2 point to different addresses.
    ! Standard: Fortran 2003 and later.
    type(c_ptr or c_funptr) :: c_prt_1, c_ptr_2
end function

subroutine C_F_POINTER(CPTR, FPTR[, SHAPE])
    ! Assign the target the C pointer CPTR to the Fortran pointer FPTR and specify its shape
    ! Arguments:
    ! CPTR  -scalar of the type C_PTR. It is INTENT(IN).
    ! FPTR  -pointer interoperable with cptr. It is INTENT(OUT).
    ! SHAPE -(Optional) Rank-one array of type INTEGER with INTENT(IN). It shall be present if and only
    !         if fptr is an array. The size must be equal to the rank of fptr.
    ! Standard: Fortran 2003 and later
    type(c_ptr) :: CPTR
end subroutine

subroutine C_F_PROCPOINTER(cptr, fptr)
    ! Assign the target of the C function pointer CPTR to the Fortran procedure pointer FPTR
    ! Arguments:
    ! FPTR  -procedure pointer interoperable with cptr. It is INTENT(OUT).
    ! Standard: Fortran 2003 and later
end subroutine

function C_FUNLOC(x)
    ! Determines the C address of the argument
    ! Arguments:
    ! x   -Interoperable function or pointer to such function.
	! Return value:
    ! The return value is of type C_FUNPTR and contains the C address of the argument.
    ! Standard: Fortran 2003 and later
end function

c_ptr function C_LOC(X)
    ! Determines the C address of the argument
	! Arguments:
    ! X  -Associated scalar pointer or interoperable scalar or allocated allocatable variable with TARGET attribute.
    ! Return value:
    ! The return value is of type C_PTR and contains the C address of the argument.
	! Standard: Fortran 2003 and later
end function

function C_SIZEOF(X)
    ! Calculates the number of bytes of storage the expression X occupies
    ! Arguments:
    ! X   -The argument shall be of any type, rank or shape.
    ! Return value:
    ! The return value is of type integer and of the system-dependent kind C_SIZE_T (from the ISO_C_BINDING module).
    ! Its value is the number of bytes occupied by the argument. If the argument has the POINTER attribute,
    ! the number of bytes of the storage area pointed to is returned. If the argument is of a derived type
    ! with POINTER or ALLOCATABLE components, the return value doesn't account for the sizes of the data pointed
    ! to by these components.
    ! Standard: Fortran 2008
end function

complex function CABS(A)
    ! It is specific name of ABS procedure.
    ! Computes the absolute value of A.
    ! Arguments:
    ! A   -The type of the argument shall be an default COMPLEX.
    complex :: A
end function

integer function CEILING(A [, KIND])
    ! Returns the least integer greater than or equal to A
	! Arguments:
    ! A  -The type shall be REAL.
    ! KIND  -(Optional) An INTEGER initialization expression indicating the kind parameter of the result.
    ! Return value:
    ! The return value is of type INTEGER(KIND) if KIND is present and a default-kind INTEGER otherwise.
	! Standard: Fortran 95 and later
	real :: A
    integer :: KIND
end function

CHARACTER(1) function CHAR(I [, KIND])
    ! Returns the character represented by the integer I
    ! Arguments:
    ! I     -The type shall be INTEGER.
    ! KIND  -(Optional) An INTEGER initialization expression indicating the kind parameter of the result.
    ! Return value:
    ! The return value is of type CHARACTER(1)
    ! Standard: Fortran 77 and later
    integer :: I, KIND
end function

complex function CMPLX(X [, Y [, KIND]])
    ! Returns a complex number where X is converted to the real component
    ! If Y is present it is converted to the imaginary component. If Y is not present then the imaginary
    ! component is set to 0.0. If X is complex then Y must not be present.
	! Arguments:
    ! X  -The type may be INTEGER, REAL, or COMPLEX.
    ! Y  -(Optional; only allowed if X is not COMPLEX.) May be INTEGER or REAL.
    ! KIND  -(Optional) An INTEGER initialization expression indicating the kind parameter of the result.
    ! Return value:
    ! The return value is of COMPLEX type, with a kind equal to KIND if it is specified. If KIND is not specified,
    ! the result is of the default COMPLEX kind, regardless of the kinds of X and Y.
	! Standard: Fortran 77 and later
    integer, real, complex :: X
    integer, real :: Y
    integer :: KIND
end function

integer function COMMAND_ARGUMENT_COUNT()
    ! Returns the number of arguments passed on the command line when the containing program was invoked
    ! Standard: Fortran 2003 and later
    ! Return value:
    ! The return value is an INTEGER of default kind.
end function

function CONJG(Z)
    ! Returns the conjugate of Z. If Z is (x, y) then the result is (x, -y)
    ! Standard: Fortran 77 and later
    ! Arguments:
    ! Z -The type shall be COMPLEX.
    ! Return value:
    ! The return value is of type COMPLEX.
end function

function COS(X)
    ! Computes the cosine of X.
    ! Standard: Fortran 77 and later
    ! Arguments:
    ! X  -The type shall be REAL or COMPLEX.
    ! Return value:
    ! The return value is of the same type and kind as X. The real part of the result is in radians.
    ! If X is of the type REAL, the return value lies in the range -1 <= cos(x) <= 1.
end function

function CCOS(X)
    ! This is specific name of COS function.
    ! Computes the cosine of X.
    ! Standard: Fortran 77 and later
    ! Arguments:
    ! X  -The type shall be COMPLEX.
    ! Return value:
    ! The return value is of the same type and kind as X. The real part of the result is in radians.
    ! If X is of the type REAL, the return value lies in the range -1 <= cos(x) <= 1.
    COMPLEX :: CCOS, X
end function

function DCOS(X)
    ! This is specific name of COS function.
    ! Computes the cosine of X.
    ! Standard: Fortran 77 and later
    ! Arguments:
    ! X  -The type shall be Doubleprecision.
    ! Return value:
    ! The return value is of the same type and kind as X. The real part of the result is in radians.
    ! If X is of the type REAL, the return value lies in the range -1 <= cos(x) <= 1.
    doubleprecision :: DCOS, X
end function

function COSH(X)
    ! Computes the hyperbolic cosine of X.
    ! Standard: Fortran 77 and later, for a complex argument Fortran 2008 or later
    ! Arguments:
    ! X  -The type shall be REAL or COMPLEX.
    ! Return value:
    ! The return value has same type and kind as X. If X is complex, the imaginary part of the result
    ! is in radians. If X is REAL, the return value has a lower bound of one, cosh(x) >= 1.
end function

function DCOSH(X)
    ! This is specific name of COSH function.
    ! Computes the hyperbolic cosine of X.
    ! Standard: Fortran 77 and later.
    ! Arguments:
    ! X  -The type shall be DOUBLE PRECISION.
    ! Return value:
    ! The return value has same type and kind as X. If X is complex, the imaginary part of the result
    ! is in radians. If X is REAL, the return value has a lower bound of one, cosh(x) >= 1.
    double precision :: DCOSH, X
end function

function COUNT(MASK [, DIM, KIND])
    ! Counts the number of .TRUE. elements in a logical MASK, or, if the DIM argument is supplied,
    ! counts the number of elements along each row of the array in the DIM direction. If the array
    ! has zero size, or all of the elements of MASK are .FALSE., then the result is 0.
    ! Standard: Fortran 95 and later, with KIND argument Fortran 2003 and later
    ! Arguments:
    ! MASK  -The type shall be LOGICAL.
    ! DIM   -(Optional) The type shall be INTEGER.
    ! KIND  -(Optional) An INTEGER initialization expression indicating the kind parameter of the result.
    ! Return value:
    ! The return value is of type INTEGER and of kind KIND. If KIND is absent, the return value is
    ! of default integer kind. If DIM is present, the result is an array with a rank one less than
    ! the rank of ARRAY, and a size corresponding to the shape of ARRAY with the DIM dimension removed.
end function

subroutine CPU_TIME(TIME)
    ! Returns a REAL value representing the elapsed CPU time in seconds. This is useful for testing
    ! segments of code to determine execution time.
    ! If a time source is available, time will be reported with microsecond resolution. If no time
    ! source is available, TIME is set to -1.0.
    ! Note that TIME may contain a system dependent, arbitrary offset and may not start with 0.0.
    ! For CPU_TIME, the absolute value is meaningless, only differences between subsequent calls
    ! to this subroutine, as shown in the example below, should be used.
    ! Standard: Fortran 95 and later
    ! Arguments:
    ! TIME  -The type shall be REAL with INTENT(OUT).
    real, intent(out) :: TIME
end subroutine

function CSHIFT(ARRAY, SHIFT [, DIM])
    ! Performs a circular shift on elements of ARRAY along the dimension of DIM. If DIM is omitted it is
    ! taken to be 1. DIM is a scalar of type INTEGER in the range of 1 \leq DIM \leq n) where n is the
    ! rank of ARRAY. If the rank of ARRAY is one, then all elements of ARRAY are shifted by SHIFT places.
    ! If rank is greater than one, then all complete rank one sections of ARRAY along the given dimension
    ! are shifted. Elements shifted out one end of each rank one section are shifted back in the other end.
    ! Standard: Fortran 95 and later
    ! Arguments:
    ! ARRAY  -Shall be an array of any type.
    ! SHIFT  -The type shall be INTEGER.
    ! DIM    -The type shall be INTEGER.
    ! Return value:
    ! Returns an array of same type and rank as the ARRAY argument.
end function

function DABS(A)
    ! It is specific name of ABS procedure.
    ! Computes the absolute value of A.
    ! Arguments:
    ! A   -The type of the argument shall be an default DOUBLEPRECISION.
    double precision :: DABS, A
end function

subroutine DATE_AND_TIME([DATE, TIME, ZONE, VALUES])
	! Gets the corresponding date and time information from the real-time system clock. DATE is INTENT(OUT)
	! and has form ccyymmdd. TIME is INTENT(OUT) and has form hhmmss.sss. ZONE is INTENT(OUT) and has
	! form (+-)hhmm, representing the difference with respect to Coordinated Universal Time (UTC).
	! Unavailable time and date parameters return blanks.
    ! VALUES is INTENT(OUT) and provides the following:
    !   VALUE(1): 	The year
    !   VALUE(2): 	The month
    !   VALUE(3): 	The day of the month
    !   VALUE(4): 	Time difference with UTC in minutes
    !   VALUE(5): 	The hour of the day
    !   VALUE(6): 	The minutes of the hour
    !   VALUE(7): 	The seconds of the minute
    !   VALUE(8): 	The milliseconds of the second
    ! Standard: Fortran 95 and later
    ! Arguments:
    ! DATE  -(Optional) The type shall be CHARACTER(LEN=8) or larger, and of default kind.
    ! TIME  -(Optional) The type shall be CHARACTER(LEN=10) or larger, and of default kind.
    ! ZONE  -(Optional) The type shall be CHARACTER(LEN=5) or larger, and of default kind.
    ! VALUES    -(Optional) The type shall be INTEGER(8).
    character(len=8 or larger), optional :: DATE
    character(len=10 or larger), optional :: TIME
    character(len=5 or larger), optional :: ZONE
    integer, dimension(8), optional :: VALUES
end subroutine

function DBLE(A)
    ! Converts A to double precision real type.
    ! Standard: Fortran 77 and later
    ! Arguments:
    ! A     -The type shall be INTEGER, REAL, or COMPLEX.
    ! Return value:
    ! The return value is of type double precision real.
end function

function DIGITS(X)
    ! Returns the number of significant binary digits of the internal model representation of X.
    ! For example, on a system using a 32-bit floating point representation, a default real number
    ! would likely return 24.
    ! Standard: Fortran 95 and later
    ! Arguments:
    ! X     -The type may be INTEGER or REAL.
    ! Return value:
    ! The return value is of type INTEGER.
	integer :: DIGITS
	integer or real :: X
end function

function DIM(X,Y)
    ! Returns the difference X-Y if the result is positive; otherwise returns zero.
	! Standard: Fortran 77 and later
    ! Arguments:
    ! X     -The type shall be INTEGER or REAL
    ! Y     -The type shall be the same type and kind as X.
    ! Return value:
    ! The return value is of type INTEGER or REAL.
end function

function IDIM(X,Y)
    ! This is specific name of DIM function.
    ! Returns the difference X-Y if the result is positive; otherwise returns zero.
	! Standard: Fortran 77 and later
    ! Arguments:
    ! X     -The type shall be INTEGER.
    ! Y     -The type shall be the same type and kind as X.
    ! Return value:
    ! The return value is of type INTEGER.
    integer :: IDIM, X, Y
end function

function DDIM(X,Y)
    ! This is specific name of DIM function.
    ! Returns the difference X-Y if the result is positive; otherwise returns zero.
	! Standard: Fortran 77 and later
    ! Arguments:
    ! X     -The type shall be DOUBLE PRECISION.
    ! Y     -The type shall be the same type and kind as X.
    ! Return value:
    ! The return value is of type DOUBLE PRECISION.
    DOUBLE PRECISION :: DDIM, X, Y
end function

function DOT_PRODUCT(VECTOR_A, VECTOR_B)
    ! Computes the dot product multiplication of two vectors VECTOR_A and VECTOR_B.
    ! The two vectors may be either numeric or logical and must be arrays of rank one
    ! and of equal size. If the vectors are INTEGER or REAL, the result is SUM(VECTOR_A*VECTOR_B).
    ! If the vectors are COMPLEX, the result is SUM(CONJG(VECTOR_A)*VECTOR_B). If the vectors
    ! are LOGICAL, the result is ANY(VECTOR_A .AND. VECTOR_B).
    ! Standard: Fortran 95 and later
    ! Arguments:
    ! VECTOR_A  -The type shall be numeric or LOGICAL, rank 1.
    ! VECTOR_B  -The type shall be numeric if VECTOR_A is of numeric type or LOGICAL if VECTOR_A
    !            is of type LOGICAL. VECTOR_B shall be a rank-one array.
    ! Return value:
    ! If the arguments are numeric, the return value is a scalar of numeric type, INTEGER, REAL,
    ! or COMPLEX. If the arguments are LOGICAL, the return value is .TRUE. or .FALSE..
end function

function DPROD(X,Y)
    ! Returns the product X*Y.
    ! Standard: Fortran 77 and later
    ! Arguments:
    ! X     -The type shall be REAL.
    ! Y     -The type shall be REAL.
    ! Return value:
    ! The return value is of type REAL(8).
    real(8) :: DPROD
    real    :: X, Y
end function

function EOSHIFT(ARRAY, SHIFT [, BOUNDARY, DIM])
	! Performs an end-off shift on elements of ARRAY along the dimension of DIM. If DIM is omitted it is
	! taken to be 1. DIM is a scalar of type INTEGER in the range of 1 \leq DIM \leq n) where n is the
	! rank of ARRAY. If the rank of ARRAY is one, then all elements of ARRAY are shifted by SHIFT places.
	! If rank is greater than one, then all complete rank one sections of ARRAY along the given dimension
	! are shifted. Elements shifted out one end of each rank one section are dropped. If BOUNDARY is present
	! then the corresponding value of from BOUNDARY is copied back in the other end. If BOUNDARY is not
	! present then the following are copied in depending on the type of ARRAY.
    ! "Array Type"  "Boundary Value"
    !  Numeric       0 of the type and kind of ARRAY.
    !  Logical       .FALSE..
    !  Character(len) len blanks.
    ! Standard: Fortran 95 and later
    ! Arguments:
    !   ARRAY 	May be any type, not scalar.
    !   SHIFT 	The type shall be INTEGER.
    !   BOUNDARY 	Same type as ARRAY.
    !   DIM 	The type shall be INTEGER.
    ! Return value:
    ! Returns an array of same type and rank as the ARRAY argument.
end function

function EPSILON(X)
	! Returns the smallest number E of the same kind as X such that 1 + E > 1.
    ! Standard: Fortran 95 and later
    ! Arguments:
    ! X     -The type shall be REAL.
    ! Return value:
    ! The return value is of same type as the argument.
end function

function ERF(X)
    ! Computes the error function of X.
    ! Standard: Fortran 2008 and later
    ! Arguments:
    ! X     -The type shall be REAL.
    ! Return value:
    ! The return value is of type REAL, of the same kind as X and lies in the range -1 <= erf(x) <= 1 .
end function

function ERFC(X)
	! Computes the complementary error function of X.
    ! Standard: Fortran 2008 and later
    ! Arguments:
    ! X     -The type shall be REAL.
    ! Return value:
    ! The return value is of type REAL and of the same kind as X. It lies in the range 0 <= erfc(x) <= 2 .
end function

function ERFC_SCALED(X)
    ! Computes the exponentially-scaled complementary error function of X.
    ! Standard: Fortran 2008 and later
    ! Arguments:
    ! X     -The type shall be REAL.
    ! Return value:
    ! The return value is of type REAL and of the same kind as X.
end function

function EXP(X)
	! Computes the base e exponential of X.
    ! Standard: Fortran 77 and later
    ! Arguments:
    ! X     -The type shall be REAL or COMPLEX.
    ! Return value:
    ! The return value has same type and kind as X.
end function

function CEXP(X)
    ! This is specific name of EXP function.
	! Computes the base e exponential of X.
    ! Standard: Fortran 77 and later
    ! Arguments:
    ! X     -The type shall be COMPLEX.
    ! Return value:
    ! The return value has same type and kind as X.
    COMPLEX :: CEXP, X
end function

function DEXP(X)
    ! This is specific name of EXP function.
	! Computes the base e exponential of X.
    ! Standard: Fortran 77 and later
    ! Arguments:
    ! X     -The type shall be DOUBLE PRECISION.
    ! Return value:
    ! The return value has same type and kind as X.
    DOUBLE PRECISION :: DEXP, X
end function

function EXPONENT(X)
    ! Returns the value of the exponent part of X. If X is zero the value returned is zero.
    ! Standard: Fortran 95 and later
    ! Arguments:
    ! X     -The type shall be REAL.
    ! Return value:
    ! The return value is of type default INTEGER.
end function

function FLOAT(A)
	! Converts the integer A to a default real value.
	! Standard: Fortran 77 and later
    ! Arguments:
    ! A   -The type shall be INTEGER.
    ! Return value:
    ! The return value is of type default REAL.
end function

function FLOOR(A [, KIND])
	! Returns the greatest integer less than or equal to X.
    ! Standard: Fortran 95 and later
    ! Arguments:
    ! A      -The type shall be REAL.
    ! KIND   -(Optional) An INTEGER initialization expression indicating the kind parameter of the result.
    ! Return value:
    ! The return value is of type INTEGER(KIND) if KIND is present and of default-kind INTEGER otherwise.
end function

function FRACTION(X)
    ! Returns the fractional part of the model representation of X.
    ! Standard: Fortran 95 and later
    ! Arguments:
    ! X    -The type of the argument shall be a REAL.
    ! Return value:
    ! The return value is of the same type and kind as the argument. The fractional part of the model
    ! representation of X is returned; it is X * RADIX(X)**(-EXPONENT(X)).
end function

function GAMMA(X)
	! Computes Gamma of X. For positive, integer values of X the Gamma function simplifies to the factorial
	! function \Gamma(x)=(x-1)!.
    ! Standard: Fortran 2008 and later
    ! Arguments:
    ! X   -Shall be of type REAL and neither zero nor a negative integer.
    ! Return value:
    ! The return value is of type REAL of the same kind as X.
end function

subroutine GET_COMMAND([COMMAND, LENGTH, STATUS])
    ! Retrieve the entire command line that was used to invoke the program.
    ! Standard: Fortran 2003 and later
    ! Arguments:
    ! COMMAND     -(Optional) shall be of type CHARACTER and of default kind.
    ! LENGTH      -(Optional) Shall be of type INTEGER and of default kind.
    ! STATUS      -(Optional) Shall be of type INTEGER and of default kind.
    !
    ! Return value:
    ! If COMMAND is present, stores the entire command line that was used to invoke the program in COMMAND.
    ! If LENGTH is present, it is assigned the length of the command line.
    ! If STATUS is present, it is assigned 0 upon success of the command, -1 if COMMAND is too short
    ! to store the command line, or a positive value in case of an error.
	character(len=*), optional :: COMMAND
	integer, optional          :: LENGTH
	integer, optional          :: STATUS
end subroutine

subroutine GET_COMMAND_ARGUMENT(NUMBER[, VALUE, LENGTH, STATUS])
	! Retrieve the NUMBER-th argument that was passed on the command line when the containing program was invoked.
    ! Standard: Fortran 2003 and later
    ! Arguments:
    ! NUMBER 	Shall be a scalar of type INTEGER and of default kind, NUMBER >= 0
    ! VALUE 	(Optional) Shall be a scalar of type CHARACTER and of default kind.
    ! LENGTH 	(Optional) Shall be a scalar of type INTEGER and of default kind.
    ! STATUS 	(Optional) Shall be a scalar of type INTEGER and of default kind.
    !Return value:
    ! After GET_COMMAND_ARGUMENT returns, the VALUE argument holds the NUMBER-th command line argument.
    ! If VALUE can not hold the argument, it is truncated to fit the length of VALUE.
    ! If there are less than NUMBER arguments specified at the command line, VALUE will be filled with blanks.
    ! If NUMBER = 0, VALUE is set to the name of the program (on systems that support this feature).
    ! The LENGTH argument contains the length of the NUMBER-th command line argument.
    ! If the argument retrieval fails, STATUS is a positive number; if VALUE contains a truncated
    ! command line argument, STATUS is -1; and otherwise the STATUS is zero.
    integer :: NUMBER
    character(len=*), optional :: VALUE
    integer, optional :: LENGTH
    integer, optional :: STATUS
end subroutine

subroutine GET_ENVIRONMENT_VARIABLE(NAME[, VALUE, LENGTH, STATUS, TRIM_NAME)
	! Get the VALUE of the environmental variable NAME.
    ! Standard: Fortran 2003 and later
    ! Arguments:
    ! NAME  -Shall be a scalar of type CHARACTER and of default kind.
    ! VALUE -Shall be a scalar of type CHARACTER and of default kind.
    ! LENGTH -Shall be a scalar of type INTEGER and of default kind.
    ! STATUS -Shall be a scalar of type INTEGER and of default kind.
    ! TRIM_NAME -Shall be a scalar of type LOGICAL and of default kind.
    ! Return value:
    ! Stores the value of NAME in VALUE. If VALUE is not large enough to hold the data, it is truncated.
    ! If NAME is not set, VALUE will be filled with blanks. Argument LENGTH contains the length needed
    ! for storing the environment variable NAME or zero if it is not present. STATUS is -1 if VALUE is
    ! present but too short for the environment variable; it is 1 if the environment variable does not exist
    ! and 2 if the processor does not support environment variables; in all other cases STATUS is zero.
    ! If TRIM_NAME is present with the value .FALSE., the trailing blanks in NAME are significant; otherwise
    ! they are not part of the environment variable name.
    character(len=*) :: NAME
    character(len=*), optional :: VALUE
    integer, optional :: LENGTH
    integer, optional :: STATUS
    logical, optional :: TRIM_NAME
end subroutine

function HUGE(X)
    ! Returns the largest number that is not an infinity in the model of the type of X.
    ! Standard: Fortran 95 and later
    ! Arguments:
    ! X   -Shall be of type REAL or INTEGER.
    ! Return value:
    ! The return value is of the same type and kind as X
end function

function HYPOT(X, Y)
    ! HYPOT(X,Y) is the Euclidean distance function. It is equal to sqrt(X^2 + Y^2), without undue underflow or overflow.
	! Standard: Fortran 2008 and later
    ! Arguments:
    ! X   -The type shall be REAL.
    ! Y   -The type and kind type parameter shall be the same as X.
    ! Return value:
    ! The return value has the same type and kind type parameter as X.
end function

function IABS(A)
    ! It is specific name of ABS procedure.
    ! Computes the absolute value of A.
    ! Arguments:
    ! A   -The type of the argument shall be an default INTEGER.
    integer :: IABS, A
end function

function IACHAR(C [, KIND])
	! Returns the code for the ASCII character in the first character position of C.
    ! Standard: Fortran 95 and later, with KIND argument Fortran 2003 and later
    ! Arguments:
    ! C    -Shall be a scalar CHARACTER, with INTENT(IN)
    ! KIND  -(Optional) An INTEGER initialization expression indicating the kind parameter of the result.
    ! Return value:
    ! The return value is of type INTEGER and of kind KIND. If KIND is absent, the return value is of default integer kind.
end function

function IAND(I, J)
	! Bitwise logical AND.
    ! Standard: Fortran 95 and later
    ! Arguments:
    ! I    -The type shall be INTEGER.
    ! J    -The type shall be INTEGER, of the same kind as I.
    ! Return value:
    ! The return type is INTEGER, of the same kind as the arguments.
end function

function IBCLR(I, POS)
    !returns the value of I with the bit at position POS set to zero.
    ! Standard: Fortran 95 and later
    ! Arguments:
    ! I     -The type shall be INTEGER.
    ! POS   -The type shall be INTEGER.
    ! Return value:
    ! The return value is of type INTEGER and of the same kind as I.
    integer :: IBCLR, I, POS
end function

function IBITS(I, POS, LEN)
	! IBITS extracts a field of length LEN from I, starting from bit position POS and extending left for LEN bits.
	! The result is right-justified and the remaining bits are zeroed. The value of POS+LEN must be less
	! than or equal to the value BIT_SIZE(I).
    ! Standard: Fortran 95 and later
    ! Arguments:
    ! I     -The type shall be INTEGER.
    ! POS   -The type shall be INTEGER.
    ! LEN   -The type shall be INTEGER.
    ! Return value:
    ! The return value is of type INTEGER and of the same kind as I.
    integer :: IBITS, I, POS, LEN
end function

function IBSET(I, POS)
	! IBSET returns the value of I with the bit at position POS set to one.
    ! Standard: Fortran 95 and later
    ! Arguments:
    ! I     -The type shall be INTEGER.
    ! POS   -The type shall be INTEGER.
    ! Return value:
    ! The return value is of type INTEGER and of the same kind as I.
    integer :: IBSET, I, POS
end function

function ICHAR(C [, KIND])
	! Returns the code for the character in the first character position of C in the system's
	! native character set.
    ! Standard: Fortan 95 and later, with KIND argument Fortran 2003 and later
    ! Arguments:
    ! C     -Shall be a scalar CHARACTER, with INTENT(IN)
    ! KIND  -(Optional) An INTEGER initialization expression indicating the kind parameter of the result.
    ! Return value:
    ! The return value is of type INTEGER and of kind KIND. If KIND is absent,
    ! the return value is of default integer kind.
end function

function IEOR(I, J)
	! Returns the bitwise boolean exclusive-OR of I and J.
    ! Standard: Fortran 95 and later
    ! Arguments:
    ! I     -The type shall be INTEGER.
    ! J     -The type shall be INTEGER, of the same kind as I.
    ! Return value:
    ! The return type is INTEGER, of the same kind as the arguments.
    integer :: IEOR, I, J
end function

function INDEX(STRING, SUBSTRING [, BACK [, KIND]])
    ! Returns the position of the start of the first occurrence of string SUBSTRING as a substring in STRING,
    ! counting from one. If SUBSTRING is not present in STRING, zero is returned. If the BACK argument is
    ! present and true, the return value is the start of the last occurrence rather than the first.
    ! Standard: Fortran 77 and later, with KIND argument Fortran 2003 and later
    ! Arguments:
    ! STRING        -Shall be a scalar CHARACTER, with INTENT(IN)
    ! SUBSTRING     -Shall be a scalar CHARACTER, with INTENT(IN)
    ! BACK          -(Optional) Shall be a scalar LOGICAL, with INTENT(IN)
    ! KIND          -(Optional) An INTEGER initialization expression indicating the kind parameter of the result.
    ! Return value:
    ! The return value is of type INTEGER and of kind KIND. If KIND is absent,
    ! the return value is of default integer kind.
    integer :: INDEX
	character(len=*) :: STRING, SUBSTRING
	logical, optional :: BACK
	integer, optional :: KIND
end function

function INT(A [, KIND))
	! Convert to integer type
    ! Standard: Fortran 77 and later
    ! Arguments:
    ! A     -Shall be of type INTEGER, REAL, or COMPLEX.
    ! KIND  -(Optional) An INTEGER initialization expression indicating the kind parameter of the result.
    ! Return value:
    ! These functions return a INTEGER variable or array under the following rules:
    ! (A)
    !    If A is of type INTEGER, INT(A) = A
    !(B)
    !    If A is of type REAL and |A| < 1, INT(A) equals 0. If |A| >= 1, then INT(A) equals the largest
    !    integer that does not exceed the range of A and whose sign is the same as the sign of A.
    ! (C)
    !    If A is of type COMPLEX, rule B is applied to the real part of A.
end function

function IFIX(A)
    ! This is specific name of INT function.
	! Convert to integer type
    ! Standard: Fortran 77 and later
    REAL :: IFIX, A
end function

function IDINT(A)
    ! This is specific name of INT function.
	! Convert to integer type
    ! Standard: Fortran 77 and later
    DOUBLE PRECISION :: IDINT, A
end function

function IOR(I, J)
	! IOR returns the bitwise boolean inclusive-OR of I and J.
    ! Standard: Fortran 95 and later
    ! Arguments:
    ! I     -The type shall be INTEGER.
    ! J 	-The type shall be INTEGER, of the same kind as I.
    ! Return value:
    ! The return type is INTEGER, of the same kind as the arguments.
    integer :: IOR, I, J
end function

function IS_IOSTAT_END(I)
	! Tests whether an variable has the value of the I/O status “end of file”. The function is
	! equivalent to comparing the variable with the IOSTAT_END parameter of the intrinsic module ISO_FORTRAN_ENV.
    ! Standard: Fortran 2003 and later
    ! Arguments:
    ! I   -Shall be of the type INTEGER.
    ! Return value:
    ! Returns a LOGICAL of the default kind, which .TRUE. if I has the value which indicates an end of file
    ! condition for IOSTAT= specifiers, and is .FALSE. otherwise.
end function

function IS_IOSTAT_EOR(I)
	! Tests whether an variable has the value of the I/O status “end of record”. The function is equivalent
	! to comparing the variable with the IOSTAT_EOR parameter of the intrinsic module ISO_FORTRAN_ENV.
    ! Standard: Fortran 2003 and later
    ! Arguments:
    ! I     -Shall be of the type INTEGER.
    ! Return value:
    ! Returns a LOGICAL of the default kind, which .TRUE. if I has the value which indicates an end of file
    ! condition for IOSTAT= specifiers, and is .FALSE. otherwise.
    logical :: IS_IOSTAT_EOR
    integer :: i
end function

function ISHFT(I, SHIFT)
	! Returns a value corresponding to I with all of the bits shifted SHIFT places. A value of SHIFT
	! greater than zero corresponds to a left shift, a value of zero corresponds to no shift, and a value
	! less than zero corresponds to a right shift. If the absolute value of SHIFT is greater than BIT_SIZE(I),
	! the value is undefined. Bits shifted out from the left end or right end are lost; zeros are shifted
	! in from the opposite end.
    ! Standard: Fortran 95 and later
    ! Arguments:
    ! I         -The type shall be INTEGER.
    ! SHIFT     -The type shall be INTEGER.
    ! Return value:
    ! The return value is of type INTEGER and of the same kind as I.
end function

function ISHFTC(I, SHIFT [, SIZE])
	! Returns a value corresponding to I with the rightmost SIZE bits shifted circularly SHIFT places;
	! that is, bits shifted out one end are shifted into the opposite end. A value of SHIFT greater than
	! zero corresponds to a left shift, a value of zero corresponds to no shift, and a value less than
	! zero corresponds to a right shift. The absolute value of SHIFT must be less than SIZE. If the SIZE
	! argument is omitted, it is taken to be equivalent to BIT_SIZE(I).
    !Standard:
    !   Fortran 95 and later
    ! Arguments:
    !   I    -The type shall be INTEGER.
    ! SHIFT  -The type shall be INTEGER.
    ! SIZE   -(Optional) The type shall be INTEGER; the value must be greater than zero and less than or equal to BIT_SIZE(I).
    ! Return value:
    ! The return value is of type INTEGER and of the same kind as I.
end function

function KIND(X)
    ! Returns the kind value of the entity X.
    ! Standard:
    !   Fortran 95 and later
    ! Arguments:
    ! X     -Shall be of type LOGICAL, INTEGER, REAL, COMPLEX or CHARACTER.
    ! Return value:
    ! The return value is a scalar of type INTEGER and of the default integer kind.
end function

function LBOUND(ARRAY [, DIM [, KIND]])
    ! Returns the lower bounds of an array, or a single lower bound along the DIM dimension.
    ! Standard:
    !   Fortran 95 and later, with KIND argument Fortran 2003 and later
    ! Arguments:
    ! ARRAY     -Shall be an array, of any type.
    ! DIM       -(Optional) Shall be a scalar INTEGER.
    ! KIND      -(Optional) An INTEGER initialization expression indicating the kind parameter of the result.
    ! Return value:
    ! The return value is of type INTEGER and of kind KIND. If KIND is absent, the return value is of default
    ! integer kind. If DIM is absent, the result is an array of the lower bounds of ARRAY. If DIM is present,
    ! the result is a scalar corresponding to the lower bound of the array along that dimension. If ARRAY is
    ! an expression rather than a whole array or array structure component, or if it has a zero extent along
    ! the relevant dimension, the lower bound is taken to be 1.
end function

function LEADZ(I)
    ! Returns the number of leading zero bits of an integer.
    ! Standard:
    !   Fortran 2008 and later
    ! Arguments:
    ! I     -Shall be of type INTEGER.
    ! Return value:
    ! The type of the return value is the default INTEGER. If all the bits of I are zero, the result value is BIT_SIZE(I).
end function

function LEN(STRING [, KIND])
    ! Returns the length of a character string. If STRING is an array, the length of an element of STRING
    ! is returned. Note that STRING need not be defined when this intrinsic is invoked, since only the length,
    ! not the content, of STRING is needed.
    ! Standard:
    !   Fortran 77 and later, with KIND argument Fortran 2003 and later
    ! Arguments:
    ! STRING    -Shall be a scalar or array of type CHARACTER, with INTENT(IN)
    ! KIND      -(Optional) An INTEGER initialization expression indicating the kind parameter of the result.
    ! Return value:
    ! The return value is of type INTEGER and of kind KIND. If KIND is absent,
    ! the return value is of default integer kind.
end function

function LEN_TRIM(STRING [, KIND])
	! Returns the length of a character string, ignoring any trailing blanks.
    ! Standard:
    !   Fortran 95 and later, with KIND argument Fortran 2003 and later
    ! Arguments:
    ! STRING    -Shall be a scalar of type CHARACTER, with INTENT(IN)
    ! KIND      -(Optional) An INTEGER initialization expression indicating the kind parameter of the result.
    ! Return value:
    ! The return value is of type INTEGER and of kind KIND. If KIND is absent,
    ! the return value is of default integer kind.
end function

function LGE(STRING_A, STRING_B)
	! Determines whether one string is lexically greater than or equal to another string,
	! where the two strings are interpreted as containing ASCII character codes.
	! If the String A and String B are not the same length, the shorter is compared as if spaces
	! were appended to it to form a value that has the same length as the longer.
    !
    ! In general, the lexical comparison intrinsics LGE, LGT, LLE, and LLT differ from
    ! the corresponding intrinsic operators .GE., .GT., .LE., and .LT., in that the latter use the
    ! processor's character ordering (which is not ASCII on some targets), whereas the former
    ! always use the ASCII ordering.
    ! Standard:
    !   Fortran 77 and later
    ! Arguments:
    ! STRING_A      -Shall be of default CHARACTER type.
    ! STRING_B      -Shall be of default CHARACTER type.
    ! Return value:
    ! Returns .TRUE. if STRING_A >= STRING_B, and .FALSE. otherwise, based on the ASCII ordering.
end function

function LGT(STRING_A, STRING_B)
	! Determines whether one string is lexically greater than another string, where the two strings
	! are interpreted as containing ASCII character codes. If the String A and String B are not the
	! same length, the shorter is compared as if spaces were appended to it to form a value that has
	! the same length as the longer.
    ! In general, the lexical comparison intrinsics LGE, LGT, LLE, and LLT differ from the corresponding
    ! intrinsic operators .GE., .GT., .LE., and .LT., in that the latter use the processor's character
    ! ordering (which is not ASCII on some targets), whereas the former always use the ASCII ordering.
    ! Standard:
    !   Fortran 77 and later
    ! Arguments:
    ! STRING_A  -Shall be of default CHARACTER type.
    ! STRING_B  -Shall be of default CHARACTER type.
    ! Return value:
    ! Returns .TRUE. if STRING_A > STRING_B, and .FALSE. otherwise, based on the ASCII ordering.
end function

function LLE(STRING_A, STRING_B)
	! Determines whether one string is lexically less than or equal to another string, where the
	! two strings are interpreted as containing ASCII character codes. If the String A and String B
	! are not the same length, the shorter is compared as if spaces were appended to it to form
	! a value that has the same length as the longer.
    ! In general, the lexical comparison intrinsics LGE, LGT, LLE, and LLT differ from the
    ! corresponding intrinsic operators .GE., .GT., .LE., and .LT., in that the latter use the
    ! processor's character ordering (which is not ASCII on some targets), whereas the former
    ! always use the ASCII ordering.
    ! Standard:
    !    Fortran 77 and later
    ! Arguments:
    !    STRING_A 	-Shall be of default CHARACTER type.
    !    STRING_B 	-Shall be of default CHARACTER type.
    ! Return value:
    !    Returns .TRUE. if STRING_A <= STRING_B, and .FALSE. otherwise, based on the ASCII ordering.
end function

function LLT(STRING_A, STRING_B)
	! Determines whether one string is lexically less than another string, where the two strings
	! are interpreted as containing ASCII character codes. If the String A and String B are not
	! the same length, the shorter is compared as if spaces were appended to it to form a value
	! that has the same length as the longer.
    ! In general, the lexical comparison intrinsics LGE, LGT, LLE, and LLT differ from the
    ! corresponding intrinsic operators .GE., .GT., .LE., and .LT., in that the latter use
    ! the processor's character ordering (which is not ASCII on some targets), whereas the
    ! former always use the ASCII ordering.
    ! Standard:
    !    Fortran 77 and later
    ! Arguments:
    !    STRING_A 	Shall be of default CHARACTER type.
    !    STRING_B 	Shall be of default CHARACTER type.
    ! Return value:
    !    Returns .TRUE. if STRING_A < STRING_B, and .FALSE. otherwise, based on the ASCII ordering.
end function

function LOG(X)
	! Computes the logarithm of X.
    ! Standard:
    !    Fortran 77 and later
    ! Arguments:
    !    X  -The type shall be REAL or COMPLEX.
    ! Return value:
    !    The return value is of type REAL or COMPLEX. The kind type parameter is the same as X.
    !    If X is COMPLEX, the imaginary part \omega is in the range -pi <= \omega <= pi.
end function

function ALOG(X)
    ! This is specific name of LOG function.
	! Computes the logarithm of X.
    ! Standard:
    !    Fortran 77 and later
    ! Arguments:
    !    X  -The type shall be REAL.
    real :: ALOG, X
end function

function CLOG(X)
    ! This is specific name of LOG function.
	! Computes the logarithm of X.
    ! Standard:
    !    Fortran 77 and later
    ! Arguments:
    !    X  -The type shall be COMLEX.
    complex :: CLOG, X
end function

function DLOG(X)
    ! This is specific name of LOG function.
	! Computes the logarithm of X.
    ! Standard:
    !    Fortran 77 and later
    ! Arguments:
    !    X  -The type shall be DOUBLE PRECISION.
    real :: DLOG, X
end function

function LOG10(X)
	! Computes the base 10 logarithm of X.
    ! Standard:
    !    Fortran 77 and later
    ! Arguments:
    !    X    -The type shall be REAL.
    ! Return value:
    !    The return value is of type REAL or COMPLEX. The kind type parameter is the same as X.
end function

function ALOG10(X)
    ! This is specific name of LOG10 function.
	! Computes the base 10 logarithm of X.
    ! Standard:
    !    Fortran 77 and later
    ! Arguments:
    !    X    -The type shall be REAL.
    ! Return value:
    !    The return value is of type REAL.
    real :: ALOG10, X
end function

function DLOG10(X)
    ! This is specific name of LOG10 function.
	! Computes the base 10 logarithm of X.
    ! Standard:
    !    Fortran 77 and later
    ! Arguments:
    !    X    -The type shall be DOUBLE PRECISION.
    ! Return value:
    !    The return value is of type DOUBLE PRECISION.
    double precision :: DLOG10, X
end function

function LOG_GAMMA(X)
	! Computes the natural logarithm of the absolute value of the Gamma function.
    ! Standard:
    !    Fortran 2008 and later
    ! Arguments:
    !    X  -Shall be of type REAL and neither zero nor a negative integer.
    ! Return value:
    !    The return value is of type REAL of the same kind as X.
end function

function LOGICAL(L [, KIND])
	! Converts one kind of LOGICAL variable to another.
    ! Standard:
    !    Fortran 95 and later
    ! Arguments:
    !    L      -The type shall be LOGICAL.
    !    KIND   -(Optional) An INTEGER initialization expression indicating the kind parameter of the result.
    ! Return value:
    !    The return value is a LOGICAL value equal to L, with a kind corresponding to KIND, or of the
    !    default logical kind if KIND is not given.
end function

function MATMUL(MATRIX_A, MATRIX_B)
	! Performs a matrix multiplication on numeric or logical arguments.
    ! Standard:
    !    Fortran 95 and later
    ! Arguments:
    !    MATRIX_A   -An array of INTEGER, REAL, COMPLEX, or LOGICAL type, with a rank of one or two.
    !    MATRIX_B   -An array of INTEGER, REAL, or COMPLEX type if MATRIX_A is of a numeric type; otherwise,
    !                an array of LOGICAL type. The rank shall be one or two, and the first (or only) dimension
    !                of MATRIX_B shall be equal to the last (or only) dimension of MATRIX_A.
    ! Return value:
    !    The matrix product of MATRIX_A and MATRIX_B. The type and kind of the result follow the usual type
    !    and kind promotion rules, as for the * or .AND. operators.
end function

function MAX(A1, A2[, A3, A4, A5, ...])
    ! Returns the argument with the largest (most positive) value.
    ! Standard:
    !    Fortran 77 and later
    ! Syntax:
    !    RESULT = MAX(A1, A2 [, A3 [, ...]])
    ! Arguments:
    !    A1     -The type shall be INTEGER or REAL.
    !    A2, A3, ...    -An expression of the same type and kind as A1.
    ! Return value:
    !    The return value corresponds to the maximum value among the arguments, and has the same
    !    type and kind as the argument.
end function

function MAX0(A1, A2)
    ! This is specific name of MAX function.
    ! Returns the argument with the largest (most positive) value.
    ! Standard:
    !    Fortran 77 and later
    ! Syntax:
    !    RESULT = MAX(A1, A2 [, A3 [, ...]])
    ! Arguments:
    !    A1     -The type shall be INTEGER.
    !    A2, A3, ...    -An expression of the same type and kind as A1.
    ! Return value:
    !    The return value corresponds to the maximum value among the arguments, and has the same
    !    type and kind as the argument.
    integer :: MAX0, A1, A2
end function

function AMAX1(A1, A2)
    ! This is specific name of MAX function.
    ! Returns the argument with the largest (most positive) value.
    ! Standard:
    !    Fortran 77 and later
    ! Syntax:
    !    RESULT = MAX(A1, A2 [, A3 [, ...]])
    ! Arguments:
    !    A1     -The type shall be REAL.
    !    A2, A3, ...    -An expression of the same type and kind as A1.
    ! Return value:
    !    The return value corresponds to the maximum value among the arguments, and has the same
    !    type and kind as the argument.
    real :: AMAX1, A1, A2
end function

function DMAX1(A1, A2)
    ! This is specific name of MAX function.
    ! Returns the argument with the largest (most positive) value.
    ! Standard:
    !    Fortran 77 and later
    ! Syntax:
    !    RESULT = MAX(A1, A2 [, A3 [, ...]])
    ! Arguments:
    !    A1     -The type shall be DOUBLE PRECISION.
    !    A2, A3, ...    -An expression of the same type and kind as A1.
    ! Return value:
    !    The return value corresponds to the maximum value among the arguments, and has the same
    !    type and kind as the argument.
    double precision :: DMAX1, A1, A2
end function

function MAX1(A1, A2)
    ! This is specific name of MAX function.
    ! MAX1 is equivalent to INT ( MAX ( . . .) )
    ! Returns the argument with the largest (most positive) value.
    ! Standard:
    !    Fortran 77 and later
    ! Syntax:
    !    RESULT = MAX(A1, A2 [, A3 [, ...]])
    ! Arguments:
    !    A1     -The type shall be REAL.
    !    A2, A3, ...    -An expression of the same type and kind as A1.
    ! Return value:
    !    The return value corresponds to the maximum value among the arguments, and has the same
    !    type and kind as the argument.
    integer :: MAX1
    real :: A1, A2
end function

function AMAX0(A1, A2)
    ! This is specific name of MAX function.
    ! AMAX0 is equivalent to REAL ( MAX ( . . . ) )
    ! Returns the argument with the largest (most positive) value.
    ! Standard:
    !    Fortran 77 and later
    ! Syntax:
    !    RESULT = MAX(A1, A2 [, A3 [, ...]])
    ! Arguments:
    !    A1     -The type shall be INTEGER.
    !    A2, A3, ...    -An expression of the same type and kind as A1.
    ! Return value:
    !    The return value corresponds to the maximum value among the arguments, and has the same
    !    type and kind as the argument.
    real :: AMAX0
    integer :: A1, A2
end function

function MAXEXPONENT(X)
    ! Returns the maximum exponent in the model of the type of X.
    ! Standard:
    !    Fortran 95 and later
    ! Arguments:
    !    X  -Shall be of type REAL.
    ! Return value:
    !    The return value is of type INTEGER and of the default integer kind.
end function

function MAXLOC(ARRAY [, DIM] [, MASK] [, KIND])
	! Determines the location of the element in the array with the maximum value, or, if the DIM
	! argument is supplied, determines the locations of the maximum element along each row of the
	! array in the DIM direction. If MASK is present, only the elements for which MASK is .TRUE.
	! are considered. If more than one element in the array has the maximum value, the location
	! returned is that of the first such element in array element order. If the array has zero size,
	! or all of the elements of MASK are .FALSE., then the result is an array of zeroes. Similarly,
	! if DIM is supplied and all of the elements of MASK along a given row are zero, the result value
	! for that row is zero.
    ! Standard:
    !    Fortran 95 and later
    ! Syntax:
    !    RESULT = MAXLOC(ARRAY, DIM [, MASK])
    !    RESULT = MAXLOC(ARRAY [, MASK])
    ! Arguments:
    !    ARRAY  -Shall be an array of type INTEGER or REAL.
    !    DIM    -(Optional) Shall be a scalar of type INTEGER, with a value between one and the rank
    !             of ARRAY, inclusive. It may not be an optional dummy argument.
    !    MASK   -Shall be an array of type LOGICAL, and conformable with ARRAY.
    ! Return value:
    !    If DIM is absent, the result is a rank-one array with a length equal to the rank of ARRAY.
    !    If DIM is present, the result is an array with a rank one less than the rank of ARRAY, and
    !    a size corresponding to the size of ARRAY with the DIM dimension removed. If DIM is present
    !    and ARRAY has a rank of one, the result is a scalar. In all cases, the result is of default INTEGER type.
end function

function MAXVAL(ARRAY [, DIM] [, MASK])
	! Determines the maximum value of the elements in an array value, or, if the DIM argument is supplied,
	! determines the maximum value along each row of the array in the DIM direction. If MASK is present,
	! only the elements for which MASK is .TRUE. are considered. If the array has zero size, or all of the
	! elements of MASK are .FALSE., then the result is -HUGE(ARRAY) if ARRAY is numeric, or a string of nulls
	! if ARRAY is of character type.
    ! Standard:
    !    Fortran 95 and later
    ! Syntax:
    !    RESULT = MAXVAL(ARRAY, DIM [, MASK])
    !    RESULT = MAXVAL(ARRAY [, MASK])
    ! Arguments:
    !    ARRAY  -Shall be an array of type INTEGER or REAL.
    !    DIM    -(Optional) Shall be a scalar of type INTEGER, with a value between one and the rank of
    !             ARRAY, inclusive. It may not be an optional dummy argument.
    !    MASK   -Shall be an array of type LOGICAL, and conformable with ARRAY.
    ! Return value:
    !    If DIM is absent, or if ARRAY has a rank of one, the result is a scalar. If DIM is present,
    !    the result is an array with a rank one less than the rank of ARRAY, and a size corresponding
    !    to the size of ARRAY with the DIM dimension removed. In all cases, the result is of the same
    !    type and kind as ARRAY.
end function

function MERGE(TSOURCE, FSOURCE, MASK)
	! Select values from two arrays according to a logical mask. The result is equal to TSOURCE if MASK
	! is .TRUE., or equal to FSOURCE if it is .FALSE..
    ! Standard:
    !    Fortran 95 and later
    ! Arguments:
    !    TSOURCE    -May be of any type.
    !    FSOURCE    -Shall be of the same type and type parameters as TSOURCE.
    !    MASK       -Shall be of type LOGICAL.
    ! Return value:
    !    The result is of the same type and type parameters as TSOURCE.
end function

function MIN(A1, A2[, A3, A4, A5, ...])
	! Returns the argument with the smallest (most negative) value.
    ! Standard:
    !    Fortran 77 and later
    ! Syntax:
    !    RESULT = MIN(A1, A2 [, A3, ...])
    ! Arguments:
    !    A1         -The type shall be INTEGER or REAL.
    !    A2, A3, ...    -An expression of the same type and kind as A1.
    ! Return value:
    !    The return value corresponds to the maximum value among the arguments, and has the same type
    !    and kind as the first argument.
end function

function MIN0(A1, A2)
    ! This is specific name of MIN function.
	! Returns the argument with the smallest (most negative) value.
    ! Standard:
    !    Fortran 77 and later
    ! Syntax:
    !    RESULT = MIN(A1, A2 [, A3, ...])
    ! Arguments:
    !    A1         -The type shall be INTEGER.
    !    A2, A3, ...    -An expression of the same type and kind as A1.
    ! Return value:
    !    The return value corresponds to the maximum value among the arguments, and has the same type
    !    and kind as the first argument.
    integer :: MIN0, A1, A2
end function

function AMIN0(A1, A2)
    ! This is specific name of MIN function.
    ! AMIN0 is equivalent to REAL ( MIN ( . . . ) )
	! Returns the argument with the smallest (most negative) value.
    ! Standard:
    !    Fortran 77 and later
    ! Syntax:
    !    RESULT = MIN(A1, A2 [, A3, ...])
    ! Arguments:
    !    A1         -The type shall be INTEGER.
    !    A2, A3, ...    -An expression of the same type and kind as A1.
    ! Return value:
    !    The return value corresponds to the maximum value among the arguments, and has the same type
    !    and kind as the first argument.
    real :: AMIN0
    integer :: A1, A2
end function

function AMIN1(A1, A2)
    ! This is specific name of MIN function.
	! Returns the argument with the smallest (most negative) value.
    ! Standard:
    !    Fortran 77 and later
    ! Syntax:
    !    RESULT = MIN(A1, A2 [, A3, ...])
    ! Arguments:
    !    A1         -The type shall be REAL.
    !    A2, A3, ...    -An expression of the same type and kind as A1.
    ! Return value:
    !    The return value corresponds to the maximum value among the arguments, and has the same type
    !    and kind as the first argument.
    real :: AMIN1, A1, A2
end function

function MIN1(A1, A2)
    ! This is specific name of MIN function.
    ! MIN1 is equivalent to INT ( MIN ( . . . ) )
	! Returns the argument with the smallest (most negative) value.
    ! Standard:
    !    Fortran 77 and later
    ! Syntax:
    !    RESULT = MIN(A1, A2 [, A3, ...])
    ! Arguments:
    !    A1         -The type shall be REAL.
    !    A2, A3, ...    -An expression of the same type and kind as A1.
    ! Return value:
    !    The return value corresponds to the maximum value among the arguments, and has the same type
    !    and kind as the first argument.
    integer :: MIN1
    real :: A1, A2
end function

function DMIN1(A1, A2)
    ! This is specific name of MIN function.
	! Returns the argument with the smallest (most negative) value.
    ! Standard:
    !    Fortran 77 and later
    ! Syntax:
    !    RESULT = MIN(A1, A2 [, A3, ...])
    ! Arguments:
    !    A1         -The type shall be DOUBLE PRECISION.
    !    A2, A3, ...    -An expression of the same type and kind as A1.
    ! Return value:
    !    The return value corresponds to the maximum value among the arguments, and has the same type
    !    and kind as the first argument.
    double precision :: DMIN1, A1, A2
end function

function MINEXPONENT(X)
	! Returns the minimum exponent in the model of the type of X.
    ! Standard:
    !    Fortran 95 and later
    ! Arguments:
    !    X  -Shall be of type REAL.
    ! Return value:
    !    The return value is of type INTEGER and of the default integer kind.
end function

function MINLOC(ARRAY [, DIM] [, MASK] [, KIND])
    ! Determines the location of the element in the array with the minimum value, or, if the DIM
    ! argument is supplied, determines the locations of the minimum element along each row of the
    ! array in the DIM direction. If MASK is present, only the elements for which MASK is .TRUE. are
    ! considered. If more than one element in the array has the minimum value, the location returned is
    ! that of the first such element in array element order. If the array has zero size, or all of the
    ! elements of MASK are .FALSE., then the result is an array of zeroes. Similarly, if DIM is supplied
    ! and all of the elements of MASK along a given row are zero, the result value for that row is zero.
    ! Standard:
    !    Fortran 95 and later
    ! Syntax:
    !    RESULT = MINLOC(ARRAY, DIM [, MASK])
    !    RESULT = MINLOC(ARRAY [, MASK])
    ! Arguments:
    !    ARRAY  -Shall be an array of type INTEGER or REAL.
    !    DIM    -(Optional) Shall be a scalar of type INTEGER, with a value between one and the rank
    !             of ARRAY, inclusive. It may not be an optional dummy argument.
    !    MASK   -Shall be an array of type LOGICAL, and conformable with ARRAY.
    ! Return value:
    !    If DIM is absent, the result is a rank-one array with a length equal to the rank of ARRAY.
    !    If DIM is present, the result is an array with a rank one less than the rank of ARRAY, and
    !    a size corresponding to the size of ARRAY with the DIM dimension removed. If DIM is present
    !    and ARRAY has a rank of one, the result is a scalar. In all cases, the result is of default INTEGER type.
end function

function MINVAL(ARRAY, DIM [, MASK])
	! Determines the minimum value of the elements in an array value, or, if the DIM argument is
	! supplied, determines the minimum value along each row of the array in the DIM direction.
	! If MASK is present, only the elements for which MASK is .TRUE. are considered. If the array
	! has zero size, or all of the elements of MASK are .FALSE., then the result is HUGE(ARRAY) if
	! ARRAY is numeric, or a string of CHAR(255) characters if ARRAY is of character type.
    ! Standard:
    !    Fortran 95 and later
    ! Syntax:
    !    RESULT = MINVAL(ARRAY, DIM [, MASK])
    !    RESULT = MINVAL(ARRAY [, MASK])
    ! Arguments:
    !    ARRAY  -Shall be an array of type INTEGER or REAL.
    !    DIM    -(Optional) Shall be a scalar of type INTEGER, with a value between one and the rank of
    !             ARRAY, inclusive. It may not be an optional dummy argument.
    !    MASK   -Shall be an array of type LOGICAL, and conformable with ARRAY.
    !
    ! Return value:
    !    If DIM is absent, or if ARRAY has a rank of one, the result is a scalar. If DIM is present,
    !    the result is an array with a rank one less than the rank of ARRAY, and a size corresponding
    !    to the size of ARRAY with the DIM dimension removed. In all cases, the result is of the same
    !    type and kind as ARRAY.
end function

function MOD(A, P)
	! Computes the remainder of the division of A by P. It is calculated as A - (INT(A/P) * P).
    ! Standard:
    !    Fortran 77 and later
    ! Arguments:
    !    A  -Shall be a scalar of type INTEGER or REAL
    !    P  -Shall be a scalar of the same type as A and not equal to zero
    ! Return value:
    !    The kind of the return value is the result of cross-promoting the kinds of the arguments.
end function

function AMOD(A, P)
    ! This is specific name of MOD function.
	! Computes the remainder of the division of A by P. It is calculated as A - (INT(A/P) * P).
    ! Standard:
    !    Fortran 77 and later
    ! Arguments:
    !    A  -Shall be a scalar of type default REAL
    !    P  -Shall be a scalar of the same type as A and not equal to zero
    ! Return value:
    !    The kind of the return value is the result of cross-promoting the kinds of the arguments.
    real :: AMOD, A, P
end function

function DMOD(A, P)
    ! This is specific name of MOD function.
	! Computes the remainder of the division of A by P. It is calculated as A - (INT(A/P) * P).
    ! Standard:
    !    Fortran 77 and later
    ! Arguments:
    !    A  -Shall be a scalar of type default DOUBLE PRECISION
    !    P  -Shall be a scalar of the same type as A and not equal to zero
    ! Return value:
    !    The kind of the return value is the result of cross-promoting the kinds of the arguments.
    double precision :: DMOD, A, P
end function

function MODULO(A, P)
	! Computes the A modulo P.
    ! Standard:
    !    Fortran 95 and later
    ! Arguments:
    !    A  -Shall be a scalar of type INTEGER or REAL
    !    P  -Shall be a scalar of the same type and kind as A
    ! Return value:
    !    The type and kind of the result are those of the arguments.
    !    If A and P are of type INTEGER:
    !        MODULO(A,P) has the value R such that A=Q*P+R, where Q is an integer and R is between
    !        0 (inclusive) and P (exclusive).
    !    If A and P are of type REAL:
    !        MODULO(A,P) has the value of A - FLOOR (A / P) * P.
    !    In all cases, if P is zero the result is processor-dependent.
end function

subroutine MOVE_ALLOC(FROM, TO)
	! Moves the allocation from FROM to TO. FROM will become deallocated in the process.
    ! Standard:
    !    Fortran 2003 and later
    ! Arguments:
    !    FROM   -ALLOCATABLE, INTENT(INOUT), may be of any type and kind.
    !    TO     -ALLOCATABLE, INTENT(OUT), shall be of the same type, kind and rank as FROM.
end subroutine

subroutine MVBITS(FROM, FROMPOS, LEN, TO, TOPOS)
	! Moves LEN bits from positions FROMPOS through FROMPOS+LEN-1 of FROM to positions TOPOS
	! through TOPOS+LEN-1 of TO. The portion of argument TO not affected by the movement of bits
	! is unchanged. The values of FROMPOS+LEN-1 and TOPOS+LEN-1 must be less than BIT_SIZE(FROM).
    ! Standard:
    !    Fortran 95 and later
    ! Arguments:
    !    FROM   -The type shall be INTEGER.
    !    FROMPOS    -The type shall be INTEGER.
    !    LEN    -The type shall be INTEGER.
    !    TO     -The type shall be INTEGER, of the same kind as FROM.
    !    TOPOS  -The type shall be INTEGER.
end subroutine

function NEAREST(X, S)
	! Returns the processor-representable number nearest to X in the direction indicated by the sign of S.
    ! Standard:
    !    Fortran 95 and later
    ! Arguments:
    !    X  -Shall be of type REAL.
    !    S  -(Optional) shall be of type REAL and not equal to zero.
    ! Return value:
    !    The return value is of the same type as X. If S is positive, NEAREST returns the
    !    processor-representable number greater than X and nearest to it. If S is negative, NEAREST
    !    returns the processor-representable number smaller than X and nearest to it.
end function

function NEW_LINE(C)
	! Returns the new-line character.
    ! Standard:
    !    Fortran 2003 and later
    ! Arguments:
    !    C  -The argument shall be a scalar or array of the type CHARACTER.
    ! Return value:
    !    Returns a CHARACTER scalar of length one with the new-line character of the same kind as parameter C.
end function

function NINT(A [, KIND])
    ! Rounds its argument to the nearest whole number.
    ! Standard:
    !    Fortran 77 and later, with KIND argument Fortran 90 and later
    ! Arguments:
    !    A      -The type of the argument shall be REAL.
    !    KIND   -(Optional) An INTEGER initialization expression indicating the kind parameter of the result.
    ! Return value:
    !    Returns A with the fractional portion of its magnitude eliminated by rounding to the nearest whole
    !    number and with its sign preserved, converted to an INTEGER of the default kind.
end function

function IDNINT(A)
    ! This is specific name of NINT function.
    ! Rounds its argument to the nearest whole number.
    ! Standard:
    !    Fortran 77 and later, with KIND argument Fortran 90 and later
    ! Arguments:
    !    A      -The type of the argument shall be DOUBLE PRECISION.
    !    KIND   -(Optional) An INTEGER initialization expression indicating the kind parameter of the result.
    ! Return value:
    !    Returns A with the fractional portion of its magnitude eliminated by rounding to the nearest whole
    !    number and with its sign preserved, converted to an INTEGER of the default kind.
    integer :: IDNINT, A
end function

function NOT(I)
	! Returns the bitwise boolean inverse of I.
    ! Standard:
    !    Fortran 95 and later
    ! Arguments:
    !    I  -The type shall be INTEGER.
    ! Return value:
    !    The return type is INTEGER, of the same kind as the argument.
end function

function NULL([MOLD])
    ! Returns a disassociated pointer.
    ! If MOLD is present, a dissassociated pointer of the same type is returned,
    ! otherwise the type is determined by context.
    ! In Fortran 95, MOLD is optional. Please note that Fortran 2003 includes cases
    ! where it is required.
    ! Standard:
    !    Fortran 95 and later
    ! Syntax:
    !    PTR => NULL([MOLD])
    ! Arguments:
    !    MOLD   -(Optional) shall be a pointer of any association status and of any type.
    ! Return value:
    !    A disassociated pointer.
end function

function PACK(ARRAY, MASK[,VECTOR])
	! Stores the elements of ARRAY in an array of rank one.
    ! The beginning of the resulting array is made up of elements whose MASK equals TRUE.
    ! Afterwards, positions are filled with elements taken from VECTOR.
    ! Standard:
    !    Fortran 95 and later
    ! Arguments:
    !    ARRAY  -Shall be an array of any type.
    !    MASK   -Shall be an array of type LOGICAL and of the same size as ARRAY. Alternatively,
    !            it may be a LOGICAL scalar.
    !    VECTOR -(Optional) shall be an array of the same type as ARRAY and of rank one. If present,
    !             the number of elements in VECTOR shall be equal to or greater than the number of
    !             true elements in MASK. If MASK is scalar, the number of elements in VECTOR shall be
    !             equal to or greater than the number of elements in ARRAY.
    ! Return value:
    !    The result is an array of rank one and the same type as that of ARRAY. If VECTOR is present,
    !    the result size is that of VECTOR, the number of TRUE values in MASK otherwise.
end function

function PRECISION(X)
	! Returns the decimal precision in the model of the type of X.
    ! Standard:
    !    Fortran 95 and later
    ! Arguments:
    !    X  -Shall be of type REAL or COMPLEX.
    ! Return value:
    !    The return value is of type INTEGER and of the default integer kind.
end function

function PRESENT(A)
    ! Determines whether an optional dummy argument is present.
    ! Standard:
    !    Fortran 95 and later
    ! Arguments:
    !    A  -May be of any type and may be a pointer, scalar or array value, or a dummy procedure.
    !        It shall be the name of an optional dummy argument accessible within the current subroutine or function.
    ! Return value:
    !    Returns either TRUE if the optional argument A is present, or FALSE otherwise.
end function

function PRODUCT(ARRAY [, DIM] [, MASK])
	! Multiplies the elements of ARRAY along dimension DIM if the corresponding element in MASK is TRUE.
    ! Standard:
    !    Fortran 95 and later
    ! Syntax:
    !    RESULT = PRODUCT(ARRAY[, MASK])
    !    RESULT = PRODUCT(ARRAY, DIM[, MASK])
    ! Arguments:
    !    ARRAY  -Shall be an array of type INTEGER, REAL or COMPLEX.
    !    DIM    -(Optional) shall be a scalar of type INTEGER with a value in the range from 1 to n,
    !            where n equals the rank of ARRAY.
    !    MASK   -(Optional) shall be of type LOGICAL and either be a scalar or an array of the same shape as ARRAY.
    ! Return value:
    !    The result is of the same type as ARRAY.
    !    If DIM is absent, a scalar with the product of all elements in ARRAY is returned. Otherwise,
    !    an array of rank n-1, where n equals the rank of ARRAY, and a shape similar to that of ARRAY
    !    with dimension DIM dropped is returned.
end function

function RADIX(X)
	! Returns the base of the model representing the entity X.
    ! Standard:
    !    Fortran 95 and later
    ! Arguments:
    !    X  -Shall be of type INTEGER or REAL
    ! Return value:
    !    The return value is a scalar of type INTEGER and of the default integer kind.
end function

subroutine RANDOM_NUMBER(HARVEST)
    ! Returns a single pseudorandom number or an array of pseudorandom numbers from the uniform distribution
    ! over the range 0 <= x < 1.
    ! Standard:
    !    Fortran 95 and later
    ! Arguments:
    !    HARVEST    -Shall be a scalar or an array of type REAL.
end subroutine

subroutine RANDOM_SEED([SIZE, PUT, GET])
	! Restarts or queries the state of the pseudorandom number generator used by RANDOM_NUMBER.
    ! If RANDOM_SEED is called without arguments, it is initialized to a default state.
    ! Standard:
    !    Fortran 95 and later
    ! Arguments:
    !    SIZE   -(Optional) Shall be a scalar and of type default INTEGER, with INTENT(OUT).
    !            It specifies the minimum size of the arrays used with the PUT and GET arguments.
    !    PUT    -(Optional) Shall be an array of type default INTEGER and rank one. It is
    !             INTENT(IN) and the size of the array must be larger than or equal to the number
    !             returned by the SIZE argument.
    !    GET    -(Optional) Shall be an array of type default INTEGER and rank one. It is INTENT(OUT)
    !            and the size of the array must be larger than or equal to the number returned by the SIZE argument.
end subroutine

function RANGE(X)
    ! Returns the decimal exponent range in the model of the type of X.
    ! Standard:
    !    Fortran 95 and later
    ! Arguments:
    !    X  -Shall be of type INTEGER, REAL or COMPLEX.
    ! Return value:
    !    The return value is of type INTEGER and of the default integer kind.
end function

function REAL(A [, KIND])
	! Converts its argument A to a real type.
    ! Standard:
    !    Fortran 77 and later
    ! Arguments:
    !    A      -Shall be INTEGER, REAL, or COMPLEX.
    !    KIND   -(Optional) An INTEGER initialization expression indicating the kind parameter of the result.
    ! Return value:
    !    These functions return a REAL variable or array under the following rules:
    !    (A)
    !        REAL(A) is converted to a default real type if A is an integer or real variable.
    !    (B)
    !        REAL(A) is converted to a real type with the kind type parameter of A if A is a complex variable.
    !    (C)
    !        REAL(A, KIND) is converted to a real type with kind type parameter KIND if A is a complex,
    !        integer, or real variable.
end function

function FLOAT(A)
    ! This is specific name of REAL function.
	! Converts its argument A to a real type.
    ! Standard:
    !    Fortran 77 and later
    ! Arguments:
    !    A      -Shall be default INTEGER.
    real :: FLOAT
    integer :: A
end function

function SAME_TYPE_AS(A,B)
	! Inquires whether the dynamic type of A is the same as the dynamic type of B.
	! Function returns true if the dynamic type of A is the same as the dynamic type of B.
    ! Standard:
    !    Fortran 2003 and later
    ! Arguments:
    ! A   -must be an object of extensible type. If it is a pointer, it must not have an undefined association status.
    ! B   -must be an object of extensible type. If it is a pointer, it must not have an undefined association status.
    ! Return value:
    ! Default LOGICAL scalar
    logical :: SAME_TYPE_AS
end function

function SNGL(A)
    ! This is specific name of REAL function.
	! Converts its argument A to a real type.
    ! Standard:
    !    Fortran 77 and later
    ! Arguments:
    !    A      -Shall be default DOUBLE PRECISION.
    real :: SNGL
    double precision :: A
end function

function REPEAT(STRING, NCOPIES)
	! Concatenates NCOPIES copies of a string.
    ! Standard:
    !    Fortran 95 and later
    ! Arguments:
    !    STRING     -Shall be scalar and of type CHARACTER.
    !    NCOPIES    -Shall be scalar and of type INTEGER.
    ! Return value:
    !    A new scalar of type CHARACTER built up from NCOPIES copies of STRING.
    character(len=*) :: REPEAT, STRING
    integer :: NCOPIES
end function

function RESHAPE(SOURCE, SHAPE[, PAD, ORDER])
	! Reshapes SOURCE to correspond to SHAPE. If necessary, the new array may be padded with
	! elements from PAD or permuted as defined by ORDER.
    ! Standard:
    !    Fortran 95 and later
    ! Arguments:
    !    SOURCE     -Shall be an array of any type.
    !    SHAPE      -Shall be of type INTEGER and an array of rank one. Its values must be positive or zero.
    !    PAD        -(Optional) shall be an array of the same type as SOURCE.
    !    ORDER      -(Optional) shall be of type INTEGER and an array of the same shape as SHAPE. Its values
    !                 shall be a permutation of the numbers from 1 to n, where n is the size of SHAPE. If
    !                 ORDER is absent, the natural ordering shall be assumed.
    ! Return value:
    !    The result is an array of shape SHAPE with the same type as SOURCE.
end function

function RRSPACING(X)
	! Returns the reciprocal of the relative spacing of model numbers near X.
    ! Standard:
    !    Fortran 95 and later
    ! Arguments:
    !    X  -Shall be of type REAL.
    ! Return value:
    !    The return value is of the same type and kind as X. The value returned is equal
    !    to ABS(FRACTION(X)) * FLOAT(RADIX(X))**DIGITS(X).
end function

function SCALE(X, I)
	! Returns X * RADIX(X)**I.
    ! Standard:
    !    Fortran 95 and later
    ! Arguments:
    !    X  -The type of the argument shall be a REAL.
    !    I  -The type of the argument shall be a INTEGER.
    ! Return value:
    !    The return value is of the same type and kind as X. Its value is X * RADIX(X)**I.
end function

function SCAN(STRING, SET[, BACK [, KIND]])
    ! Scans a STRING for any of the characters in a SET of characters.
    ! If BACK is either absent or equals FALSE, this function returns the position of the leftmost
    ! character of STRING that is in SET. If BACK equals TRUE, the rightmost position is returned.
    ! If no character of SET is found in STRING, the result is zero.
    ! Standard:
    !    Fortran 95 and later, with KIND argument Fortran 2003 and later
    ! Arguments:
    !    STRING     -Shall be of type CHARACTER.
    !    SET        -Shall be of type CHARACTER.
    !    BACK       -(Optional) shall be of type LOGICAL.
    !    KIND       -(Optional) An INTEGER initialization expression indicating the kind parameter of the result.
    ! Return value:
    !    The return value is of type INTEGER and of kind KIND. If KIND is absent, the return value
    !    is of default integer kind.
end function

function SELECTED_CHAR_KIND(NAME)
    ! Returns the kind value for the character set named NAME, if a character set with such a name
    ! is supported, or -1 otherwise.
    ! Standard:
    !    Fortran 2003 and later
    ! Arguments:
    !    NAME 	Shall be a scalar and of the default character type.
end function

function SELECTED_INT_KIND(R)
    ! Return the kind value of the smallest integer type that can represent all values ranging
    ! from -10^R (exclusive) to 10^R (exclusive). If there is no integer kind that accommodates
    ! this range, SELECTED_INT_KIND returns -1.
    ! Standard:
    !    Fortran 95 and later
    ! Arguments:
    !    R   -Shall be a scalar and of type INTEGER.
    integer :: R
end function

function SELECTED_REAL_KIND([P, R])
    ! Returns the kind value of a real data type with decimal precision of at least P digits and
    ! exponent range greater at least R.
    ! Standard:
    !    Fortran 95 and later
    ! Arguments:
    !    P    -(Optional) shall be a scalar and of type INTEGER.
    !    R    -(Optional) shall be a scalar and of type INTEGER.
    !    At least one argument shall be present.
    ! Return value:
    !    SELECTED_REAL_KIND returns the value of the kind type parameter of a real data type with
    !    decimal precision of at least P digits and a decimal exponent range of at least R.
    !    If more than one real data type meet the criteria, the kind of the data type with the smallest
    !    decimal precision is returned. If no real data type matches the criteria, the result is
    !
    !    -1 if the processor does not support a real data type with a
    !        precision greater than or equal to P
    !    -2 if the processor does not support a real type with an exponent
    !        range greater than or equal to R
    !    -3 if neither is supported.
end function

function SET_EXPONENT(X, I)
    ! Returns the real number whose fractional part is that that of X and whose exponent part is I.
    ! Standard:
    !    Fortran 95 and later
    ! Arguments:
    !    X      -Shall be of type REAL.
    !    I      -Shall be of type INTEGER.
    ! Return value:
    !    The return value is of the same type and kind as X. The real number whose fractional part
    !    is that that of X and whose exponent part if I is returned; it is FRACTION(X) * RADIX(X)**I.
end function

function SHAPE(SOURCE [, KIND])
    ! Determines the shape of an array.
    ! Standard:
    !    Fortran 95 and later
    ! Arguments:
    !    SOURCE     -Shall be an array or scalar of any type. If SOURCE is a pointer it must be
    !                associated and allocatable arrays must be allocated.
    ! Return value:
    !    An INTEGER array of rank one with as many elements as SOURCE has dimensions. The elements
    !    of the resulting array correspond to the extend of SOURCE along the respective dimensions.
    !    If SOURCE is a scalar, the result is the rank one array of size zero.
end function

function SIGN(A, B)
    ! Returns the value of A with the sign of B.
    ! Standard:
    !    Fortran 77 and later
    ! Arguments:
    !    A      -Shall be of type INTEGER or REAL
    !    B      -Shall be of the same type and kind as A
    ! Return value:
    !    The kind of the return value is that of A and B. If B >= 0 then the result is ABS(A),
    !    else it is -ABS(A).
end function

function DSIGN(A, B)
    ! This is specific name of SIGN function.
    ! Returns the value of A with the sign of B.
    ! Standard:
    !    Fortran 77 and later
    ! Arguments:
    !    A      -Shall be of type DOUBLE PRECISION
    !    B      -Shall be of the same type and kind as A
    ! Return value:
    !    The kind of the return value is that of A and B. If B >= 0 then the result is ABS(A),
    !    else it is -ABS(A).
    double precision :: DSIGN, A, B
end function

function ISIGN(A, B)
    ! This is specific name of SIGN function.
    ! Returns the value of A with the sign of B.
    ! Standard:
    !    Fortran 77 and later
    ! Arguments:
    !    A      -Shall be of type default INTEGER
    !    B      -Shall be of the same type and kind as A
    ! Return value:
    !    The kind of the return value is that of A and B. If B >= 0 then the result is ABS(A),
    !    else it is -ABS(A).
    integer :: ISIGN, A, B
end function

function SIN(X)
    ! Computes the sine of X.
    ! Standard:
    !    Fortran 77 and later
    ! Arguments:
    !    X      -The type shall be REAL or COMPLEX.
    ! Return value:
    !    The return value has same type and kind as X.
end function

function CSIN(X)
    ! This is specific name of SIN function.
    ! Computes the sine of X.
    ! Standard:
    !    Fortran 77 and later
    ! Arguments:
    !    X      -The type shall be COMPLEX.
    ! Return value:
    !    The return value has same type and kind as X.
    complex :: CSIN, X
end function

function DSIN(X)
    ! This is specific name of SIN function.
    ! Computes the sine of X.
    ! Standard:
    !    Fortran 77 and later
    ! Arguments:
    !    X      -The type shall be DOUBLE PRECISION.
    ! Return value:
    !    The return value has same type and kind as X.
    complex :: DSIN, X
end function

function SINH(X)
    ! Computes the hyperbolic sine of X.
    ! Standard:
    !    Fortran 95 and later, for a complex argument Fortran 2008 or later
    ! Arguments:
    !    X   -The type shall be REAL or COMPLEX.
    ! Return value:
    !    The return value has same type and kind as X.
end function

function DSINH(X)
    ! Computes the hyperbolic sine of X.
    ! Standard:
    !    Fortran 95 and later
    ! Arguments:
    !    X   -The type shall be DOUBLE PRECISION.
    ! Return value:
    !    The return value has same type and kind as X.
    double precision :: DSINH, X
end function

function SIZE(ARRAY[, DIM [, KIND]])
    ! Determine the extent of ARRAY along a specified dimension DIM, or the total number of elements
    ! in ARRAY if DIM is absent.
    ! Standard:
    !    Fortran 95 and later, with KIND argument Fortran 2003 and later
    ! Arguments:
    !    ARRAY    -Shall be an array of any type. If ARRAY is a pointer it must be associated and
    !              allocatable arrays must be allocated.
    !    DIM      -(Optional) shall be a scalar of type INTEGER and its value shall be in the range
    !               from 1 to n, where n equals the rank of ARRAY.
    !    KIND     -(Optional) An INTEGER initialization expression indicating the kind parameter
    !               of the result.
    ! Return value:
    !    The return value is of type INTEGER and of kind KIND. If KIND is absent, the return value
    !    is of default integer kind.
end function

function SNGL(A)
    ! Converts the double precision real A to a default real value. This is an archaic form of REAL
    ! that is specific to one type for A.
    ! Standard:
    !    Fortran 77 and later
    ! Arguments:
    !    A 	The type shall be a double precision REAL.
    ! Return value:
    !    The return value is of type default REAL.
end function

function SPACING(X)
    ! Determines the distance between the argument X and the nearest adjacent number of the same type.
    ! Standard:
    !    Fortran 95 and later
    ! Arguments:
    !    X 	Shall be of type REAL.
    ! Return value:
    !    The result is of the same type as the input argument X.
end function

function SPREAD(SOURCE, DIM, NCOPIES)
    ! Replicates a SOURCE array NCOPIES times along a specified dimension DIM.
    ! Standard:
    !    Fortran 95 and later
    ! Arguments:
    !    SOURCE     -Shall be a scalar or an array of any type and a rank less than seven.
    !    DIM        -Shall be a scalar of type INTEGER with a value in the range from 1 to n+1,
    !                 where n equals the rank of SOURCE.
    !    NCOPIES    -Shall be a scalar of type INTEGER.
    ! Return value:
    !    The result is an array of the same type as SOURCE and has rank n+1 where n equals the rank of SOURCE.
end function

function SQRT(X)
    ! Computes the square root of X.
    ! Standard:
    !    Fortran 77 and later
    ! Arguments:
    !    X      -The type shall be REAL or COMPLEX.
    ! Return value:
    !    The return value is of type REAL or COMPLEX. The kind type parameter is the same as X.
end function

function CSQRT(X)
    ! This is specific name of SQRT function.
    ! Computes the square root of X.
    ! Standard:
    !    Fortran 77 and later
    ! Arguments:
    !    X      -The type shall be COMPLEX.
    ! Return value:
    !    The return value is of type COMPLEX.
    complex :: CSQRT, X
end function

function DSQRT(X)
    ! This is specific name of SQRT function.
    ! Computes the square root of X.
    ! Standard:
    !    Fortran 77 and later
    ! Arguments:
    !    X      -The type shall be DOUBLE PRECISION.
    ! Return value:
    !    The return value is of type DOUBLE PRECISION.
    double precision :: DSQRT, X
end function

function SUM(ARRAY[, DIM] [, MASK])
    ! Adds the elements of ARRAY along dimension DIM if the corresponding element in MASK is TRUE.
    ! Standard:
    !    Fortran 95 and later
    ! Syntax:
    !    RESULT = SUM(ARRAY[, MASK])
    !    RESULT = SUM(ARRAY, DIM[, MASK])
    ! Arguments:
    !    ARRAY  -Shall be an array of type INTEGER, REAL or COMPLEX.
    !    DIM    -(Optional) shall be a scalar of type INTEGER with a value in the range from 1 to n,
    !             where n equals the rank of ARRAY.
    !    MASK   -(Optional) shall be of type LOGICAL and either be a scalar or an array of the same shape as ARRAY.
    ! Return value:
    !    The result is of the same type as ARRAY.
    !    If DIM is absent, a scalar with the sum of all elements in ARRAY is returned. Otherwise,
    !    an array of rank n-1, where n equals the rank of ARRAY,and a shape similar to that of ARRAY with
    !    dimension DIM dropped is returned.
end function

subroutine SYSTEM_CLOCK([COUNT, COUNT_RATE, COUNT_MAX])
    ! Determines the COUNT of milliseconds of wall clock time. COUNT_RATE determines the number of clock
    ! ticks per second or zero if there is no processor clock. COUNT_MAX is assigned the maximum value of COUNT,
    ! if a processor clock is available, or otherwise, zero.
    ! Standard:
    !    Fortran 95 and later
    !Arguments:
    !    COUNT  -(Optional) shall be a scalar of type default INTEGER with INTENT(OUT).
    !    COUNT_RATE     -(Optional) shall be a scalar of type default INTEGER with INTENT(OUT).
    !    COUNT_MAX  -(Optional) shall be a scalar of type default INTEGER with INTENT(OUT).
    integer, optional, intent(out) :: COUNT, COUNT_RATE, COUNT_MAX
end subroutine

function TAN(X)
    ! Computes the tangent of X.
    ! Standard:
    !    Fortran 77 and later, for a complex argument Fortran 2008 or later
    ! Arguments:
    !    X  -The type shall be REAL or COMPLEX.
    ! Return value:
    !    The return value has same type and kind as X.
end function

function DTAN(X)
    ! This is specific name of TAN function.
    ! Computes the tangent of X.
    ! Standard:
    !    Fortran 77 and later
    ! Arguments:
    !    X  -The type shall be DOUBLE PRECISION.
    ! Return value:
    !    The return value has same type and kind as X.
    double precision :: DTAN, X
end function

function TANH(X)
    ! Computes the hyperbolic tangent of X.
    ! Standard:
    !    Fortran 77 and later, for a complex argument Fortran 2008 or later
    ! Arguments:
    !    X 	The type shall be REAL or COMPLEX.
    ! Return value:
    !    The return value has same type and kind as X. If X is complex, the imaginary part of the result
    !    is in radians. If X is REAL, the return value lies in the range - 1 <= tanh(x) <= 1 .
end function

function DTANH(X)
    ! This is specific name of TANH function.
    ! Computes the hyperbolic tangent of X.
    ! Standard:
    !    Fortran 77 and later
    ! Arguments:
    !    X 	The type shall be DOUBLE PRECISION.
    ! Return value:
    !    The return value has same type and kind as X.
    !    If X is REAL, the return value lies in the range - 1 <= tanh(x) <= 1 .
    double precision :: DTANH, X
end function

function TINY(X)
    ! Returns the smallest positive (non zero) number in the model of the type of X.
    ! Standard:
    !    Fortran 95 and later
    ! Arguments:
    !    X  -Shall be of type REAL.
    ! Return value:
    !    The return value is of the same type and kind as X
end function

function TRAILZ(I)
    ! Returns the number of trailing zero bits of an integer.
    ! Standard:
    !    Fortran 2008 and later
    ! Arguments:
    !    I  -Shall be of type INTEGER.
    ! Return value:
    !    The type of the return value is the default INTEGER. If all the bits of I are zero,
    !    the result value is BIT_SIZE(I).
end function

function TRANSFER(SOURCE, MOLD[, SIZE])
    ! Interprets the bitwise representation of SOURCE in memory as if it is the representation of a variable
    ! or array of the same type and type parameters as MOLD.
    ! This is approximately equivalent to the C concept of casting one type to another.
    ! Standard:
    !    Fortran 95 and later
    ! Arguments:
    !    SOURCE     -Shall be a scalar or an array of any type.
    !    MOLD       -Shall be a scalar or an array of any type.
    !    SIZE       -(Optional) shall be a scalar of type INTEGER.
    ! Return value:
    !    The result has the same type as MOLD, with the bit level representation of SOURCE. If SIZE is present,
    !    the result is a one-dimensional array of length SIZE. If SIZE is absent but MOLD is an array
    !    (of any size or shape), the result is a one- dimensional array of the minimum length needed to contain the
    !    entirety of the bitwise representation of SOURCE. If SIZE is absent and MOLD is a scalar, the result is a scalar.
    !
    !    If the bitwise representation of the result is longer than that of SOURCE, then the leading bits of the result
    !    correspond to those of SOURCE and any trailing bits are filled arbitrarily.
    !
    !    When the resulting bit representation does not correspond to a valid representation of a variable of the same
    !    type as MOLD, the results are undefined, and subsequent operations on the result cannot be guaranteed to produce
    !    sensible behavior. For example, it is possible to create LOGICAL variables for which VAR and .NOT.VAR both
    !    appear to be true.
end function

function TRANSPOSE(MATRIX)
    ! Transpose an array of rank two. Element (i, j) of the result has the value MATRIX(j, i), for all i, j.
    ! Standard:
    !    Fortran 95 and later
    ! Arguments:
    !    MATRIX     -Shall be an array of any type and have a rank of two.
    ! Return value:
    !    The result has the same type as MATRIX, and has shape (/ m, n /) if MATRIX has shape (/ n, m /).
end function

function TRIM(STRING)
    ! Removes trailing blank characters of a string.
    ! Standard:
    !    Fortran 95 and later
    ! Arguments:
    !    STRING     -Shall be a scalar of type CHARACTER.
    ! Return value:
    !    A scalar of type CHARACTER which length is that of STRING less the number of trailing blanks.
    character(len=*) :: TRIM, STRING
end function

function UBOUND(ARRAY [, DIM [, KIND]])
    ! Returns the upper bounds of an array, or a single upper bound along the DIM dimension.
    ! Standard:
    !    Fortran 95 and later, with KIND argument Fortran 2003 and later
    ! Arguments:
    !    ARRAY  -Shall be an array, of any type.
    !    DIM    -(Optional) Shall be a scalar INTEGER.
    !    KIND   -(Optional) An INTEGER initialization expression indicating the kind parameter of the result.
    ! Return value:
    !    The return value is of type INTEGER and of kind KIND. If KIND is absent, the return value is of
    !    default integer kind. If DIM is absent, the result is an array of the upper bounds of ARRAY.
    !    If DIM is present, the result is a scalar corresponding to the upper bound of the array along that
    !    dimension. If ARRAY is an expression rather than a whole array or array structure component, or if
    !    it has a zero extent along the relevant dimension, the upper bound is taken to be the number of
    !    elements along the relevant dimension.
end function

function UNPACK(VECTOR, MASK, FIELD)
    ! Store the elements of VECTOR in an array of higher rank.
    ! Standard:
    !    Fortran 95 and later
    ! Arguments:
    !    VECTOR     -Shall be an array of any type and rank one. It shall have at least as many
    !                elements as MASK has TRUE values.
    !    MASK       -Shall be an array of type LOGICAL.
    !    FIELD      -Shall be of the same type as VECTOR and have the same shape as MASK.
    ! Return value:
    !    The resulting array corresponds to FIELD with TRUE elements of MASK replaced by values
    !    from VECTOR in array element order.
end function

function VERIFY(STRING, SET[, BACK [, KIND]])
    ! Verifies that all the characters in a SET are present in a STRING.
    ! If BACK is either absent or equals FALSE, this function returns the position of the leftmost character
    ! of STRING that is not in SET. If BACK equals TRUE, the rightmost position is returned. If all
    ! characters of SET are found in STRING, the result is zero.
    ! Standard:
    !    Fortran 95 and later, with KIND argument Fortran 2003 and later
    ! Arguments:
    !    STRING     -Shall be of type CHARACTER.
    !    SET        -Shall be of type CHARACTER.
    !    BACK       -(Optional) shall be of type LOGICAL.
    !    KIND       -(Optional) An INTEGER initialization expression indicating the kind parameter of the result.
    ! Return value:
    !    The return value is of type INTEGER and of kind KIND. If KIND is absent, the return value is of
    !    default integer kind.
    integer(KIND=KIND) :: VERIFY
    character(len=*) :: STRING, SET
    logical, optional :: BACK
    integer, optional :: KIND
end function

function EXTENDS_TYPE_OF (A, MOLD)
    ! Inquires whether the dynamic type of A is an extension type of the dynamic type of MOLD.
    ! If MOLD is unlimited polymorphic and is a disassociated pointer or unallocated allocatable, the result is true.
    ! If A is unlimited polymorphic and is either a disassociated pointer or unallocated allocatable, the result is false.
    ! If the dynamic type of A is an extension type of the dynamic type of MOLD, the result is true.
    ! In other cases, the result is false.
    ! Standard:
    !   Fortran 2003 and later
    ! Arguments:
    ! A  -object of extensible type. If A is a pointer, A must not have an undefined association status.
    ! MOLD   -object of extensible type. If MOLD is a pointer, MOLD must not have an undefined association status.
    ! Return value:
    !    The return value is default logical
    logical :: EXTENDS_TYPE_OF
Result type and attributes

Default logical scalar

end function


# SAME_TYPE_AS -knygoje f2003 yra si funkcija, o gfortran aprasyme jos nematyti. ????


Siame sarase nera specifiniu tipu proceduru. Ju sarasas duotas f2003 knygoje.



