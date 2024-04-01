program grafic_coord
character(len=800) :: fname
character(len=1) :: ax
real,allocatable :: slab(:,:)
common/cosmo/ omegam,omegav
external func_dtdacube

if(iargc().lt.1) then
  print*,'Usage: grafic_coord velocity_file'
  return
endif


call getarg(1,fname)
! call getarg(2,ax)

open(1,file=fname,form='unformatted',status='old')
read(1) np1,np2,np3,dx,x1o,x2o,x3o,astart,omegam,omegav,h0

open(2,file=trim(fname)//'_pos',form='unformatted')
write(2) np1,np2,np3,dx,x1o,x2o,x3o,astart,omegam,omegav,h0


!adot = astart*sqrt(omegav + omegam/astart**3)
!print*, astart, omegam, omegav

ainv = 1. / astart
adot = sqrt(omegam*(ainv-1) + amogev*(astart*astart-1) + 1)
tmp1 = rombint(func_dtdacube, 1e-6, astart, 1e-5)
!print*, tmp1
growth = 2.5 * omegam * adot * tmp1 / astart
tmp  = 1. - omegam - omegav
fupper = 2.5 * omegam / growth - 1.5 * omegam * ainv - tmp
flower = omegam * ainv + omegav * astart * astart + tmp
growthVel = fupper/flower

vFact = 1.  / ( (adot) * 100. * (growthVel) )

print*, 'vFact: ',vFact
print*, 'growthVel: ', growthVel
print*, 'growth: ',growth

allocate(slab(np1,np2))

do k=1,np3
  read(1) slab
  slab = slab * vFact
!  do i=1,np1
!    do j=1,np2
!      if (ax.eq.'x') slab(i,j) = slab(i,j) + dx*(i-0.5)
!      if (ax.eq.'y') slab(i,j) = slab(i,j) + dx*(j-0.5)
!      if (ax.eq.'z') slab(i,j) = slab(i,j) + dx*(k-0.5)
!    enddo
!  enddo
  write(2) slab
enddo
close(2)
close(1)

contains

end program grafic_coord

real function func_dtdacube(a)
!  real,intent(in)::a
  common/cosmo/ omegam,omegav
  func_dtdacube = 1.0 / (omegam * (1.0/a - 1.0) + omegav * (a * a - 1.0) + 1.0)**1.5
end function func_dtdacube

real function rombint(f,a,b,tol)
        parameter (MAXITER=30,MAXJ=5)
        implicit real (a-h,o-z)
        dimension g(MAXJ+1)
        real a, b, tol
        external f
!        interface
!          real function f(x)
!            real,intent(in)::x
!          end function f
!        end interface
        h=0.5*(b-a)
        gmax=h*(f(a)+f(b))
        g(1)=gmax
        nint=1
        error=1.0e20
        i=0
10        i=i+1
          if (i.gt.MAXITER.or.(i.gt.5.and.abs(error).lt.tol)) go to 40
!  Calculate next trapezoidal rule approximation to integral.
          g0=0.0
            do 20 k=1,nint
            g0=g0+f(a+(k+k-1)*h)
20        continue
          g0=0.5*g(1)+h*g0
          h=0.5*h
          nint=nint+nint
          jmax=min(i,MAXJ)
          fourj=1.0
            do 30 j=1,jmax
!  Use Richardson extrapolation.
            fourj=4.0*fourj
            g1=g0+(g0-g(j))/(fourj-1.0)
            g(j)=g0
            g0=g1
30        continue
          if (abs(g0).gt.tol) then
            error=1.0-gmax/g0
          else
            error=gmax
          end if
          gmax=g0
          g(jmax+1)=g0
        go to 10
40      rombint=g0
        if (i.gt.MAXITER.and.abs(error).gt.tol) write(*,*) 'Rombint failed to converge; integral, error=',rombint,error
        return
end function rombint
