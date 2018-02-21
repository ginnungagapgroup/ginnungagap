program cubic_mask

character(len=800):: parameterfile
character(len=4):: lnum
character(len=6):: ldim
character(len=3):: xyz = 'xyz'
integer::ix, iy, iz, nx, ny, nz ! high res region start and size at the start level
integer::minlev, maxlev, npad
real::astart, omegam, omegav, H0, dx, boxsize
real, allocatable:: slab(:,:)
common/cosmo/ omegam,omegav
external func_dtdacube

namelist/maskparam/ ix,iy,iz,nx,ny,nz,minlev,maxlev,npad

if(iargc().lt.1) then
  print*,'Usage: cubic_mask parameterfile'
  return
endif

call getarg(1,parameterfile)


open(11,file=parameterfile,status='old')
read(11,nml=maskparam)
close(11)
print*,'Read the following:'
print*,'ix,iy,iz = ', ix,iy,iz
print*,'nx,ny,nz = ', nx,ny,nz
print*,'minlev, maxlev = ', minlev, maxlev

hubble = H0/100.
n1m = 2**maxlev

ixc = ix - 2*npad + 1
iyc = iy - 2*npad + 1
izc = iz - 2*npad + 1
nxc = nx + 4*npad
nyc = ny + 4*npad
nzc = nz + 4*npad


ixm = ix + 1 ! coordinates for ic_refmap
iym = iy + 1
izm = iz + 1
nxm = nx + 1
nym = ny + 1
nzm = nz + 1

do lev = maxlev, minlev, -1
  write(lnum,'(I4)') 1000+lev
  n1 = 2**lev
  
  if(lev.eq.minlev) then
    ixc = 1
    iyc = 1
    izc = 1
    nxc = n1
    nyc = n1
    nzc = n1
  endif
  
  print*,'level '//lnum(2:4)
  print*,'ix,iy,iz = ', ixc,iyc,izc
  print*,'nx,ny,nz = ', nxc,nyc,nzc
  print*,'nxm,nym,nzm = ',nxm,nym,nzm
  !print*,ixm-ixc,iym-iyc,izm-izc,':',ixm-ixc+nxm-1,iym-iyc+nym-1,izm-izc+nzm-1
  
  if(ixc.lt.1.or.iyc.lt.1.or.izc.lt.1.or.ixc+nxc-1.gt.n1.or.iyc+nyc-1.gt.n1.or.izc+nzc-1.gt.n1) then
    print*,' No refinement further possible with such padding!'
    return
  endif
   
  write(ldim,'(I5)') n1

  do iax = 0,3
    if(iax.eq.0) then
      open(13,file='g9p'//trim(adjustl(ldim))//'_delta',form='unformatted',status='old')
      open(12,file='level_'//lnum(2:4)//'/ic_deltab',form='unformatted')
    else 
      open(13,file='g9p'//trim(adjustl(ldim))//'_vel'//xyz(iax:iax),form='unformatted',status='old')
      open(12,file='level_'//lnum(2:4)//'/ic_velc'//xyz(iax:iax),form='unformatted')
      open(14,file='level_'//lnum(2:4)//'/ic_posc'//xyz(iax:iax),form='unformatted')
    endif
    read(13) n1r,n1r,n1r,dx,x1o_r,x2o_r,x3o_r,astart,omegam,omegav,H0
    
    x1o = (ixc-1)*dx
    x2o = (iyc-1)*dx
    x3o = (izc-1)*dx
    
    write(12) nxc,nyc,nzc,dx,x1o,x2o,x3o,astart,omegam,omegav,H0
    if(iax.gt.0) write(14) nxc,nyc,nzc,dx,x1o,x2o,x3o,astart,omegam,omegav,H0
    allocate(slab(n1r,n1r))
    
    ainv = 1. / astart
	adot = sqrt(omegam*(ainv-1) + omegav*(astart*astart-1) + 1)
	tmp1 = rombint(func_dtdacube, 1e-6, astart, 1e-5)
	growth = 2.5 * omegam * adot * tmp1 / astart
	tmp  = 1. - omegam - omegav
	fupper = 2.5 * omegam / growth - 1.5 * omegam * ainv - tmp
	flower = omegam * ainv + omegav * astart * astart + tmp
	growthVel = fupper/flower

	vFact = 1.  / ( (adot) * 100. * (growthVel) )

    ixv = nint(x1o_r/dx)
    iyv = nint(x2o_r/dx)
    izv = nint(x3o_r/dx)
    
    print*, ixv, x1o_r/dx
    
    do k = 1,n1r
      read(13) slab
      kv = k + izv
      if(kv.ge.izc.and.kv.lt.izc+nzc) then
        write(12) slab(ixc-ixv:ixc+nxc-1-ixv, iyc-iyv:iyc+nyc-1-iyv)
        if(iax.gt.0) then
          write(14) slab(ixc-ixv:ixc+nxc-1-ixv, iyc-iyv:iyc+nyc-1-iyv) * vFact
        endif
      endif
    enddo
    deallocate(slab)
  
    close(13)
    close(12)
    if(iax.gt.0) close(14)
  enddo
  
  open(12,file='level_'//lnum(2:4)//'/ic_refmap',form='unformatted')
  open(15,file='level_'//lnum(2:4)//'/ic_pvar_00001',form='unformatted')
 
  write(12) nxc,nyc,nzc,dx,x1o,x2o,x3o,astart,omegam,omegav,H0
  write(15) nxc,nyc,nzc,dx,x1o,x2o,x3o,astart,omegam,omegav,H0
  allocate(slab(n1,n1))
    
  do k = izc, izc+nzc-1
    slab = 0.0
    if(k.ge.izm.and.k.le.izm+nzm) then
      slab(ixm:ixm+nxm, iym:iym+nym) = 1.0
    endif
    write(12) slab(ixc:ixc+nxc-1, iyc:iyc+nyc-1)
    write(15) slab(ixc:ixc+nxc-1, iyc:iyc+nyc-1)
  enddo
  deallocate(slab)
  close(12)
  close(15)
  
  ixc = (ixc-1)/2 - npad +1
  iyc = (iyc-1)/2 - npad +1
  izc = (izc-1)/2 - npad +1
  nxc = nxc/2 + 2*npad 
  nyc = nyc/2 + 2*npad
  nzc = nzc/2 + 2*npad
  
  ixm = (ixm-1)/2+1
  iym = (iym-1)/2+1
  izm = (izm-1)/2+1
  nxm = (nxm-1)/2+1
  nym = (nym-1)/2+1
  nzm = (nzm-1)/2+1
  
enddo

end program cubic_mask

real function func_dtdacube(a)
!  real,intent(in)::a
  common/cosmo/ omegam,omegav
  func_dtdacube = 1. / (omegam * (1./a - 1.0) + omegav * (a * a - 1.0) + 1.0)**1.5
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
        h=0.5d0*(b-a)
        gmax=h*(f(a)+f(b))
        g(1)=gmax
        nint=1
        error=1.0d20
        i=0
10        i=i+1
          if (i.gt.MAXITER.or.(i.gt.5.and.abs(error).lt.tol)) go to 40
!  Calculate next trapezoidal rule approximation to integral.
          g0=0.0d0
            do 20 k=1,nint
            g0=g0+f(a+(k+k-1)*h)
20        continue
          g0=0.5d0*g(1)+h*g0
          h=0.5d0*h
          nint=nint+nint
          jmax=min(i,MAXJ)
          fourj=1.0d0
            do 30 j=1,jmax
!  Use Richardson extrapolation.
            fourj=4.0d0*fourj
            g1=g0+(g0-g(j))/(fourj-1.0d0)
            g(j)=g0
            g0=g1
30        continue
          if (abs(g0).gt.tol) then
            error=1.0d0-gmax/g0
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
