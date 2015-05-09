program larefix
character(len=800) :: filein, tmpngrid
character(len=6) :: tmpnum
logical, allocatable :: grid(:,:,:)
integer :: mstart(3), mend(3), msize(3), m(3)



if(iargc().lt.2) then
  print*,'Usage: larefix lare-file ngrid'
  return
endif

call getarg(1,filein)
call getarg(2,tmpngrid)

read(tmpngrid,*) ngrid

allocate(grid(ngrid,ngrid,ngrid))

! read the LaRe
grid=.false.
open(1,file=filein,status='old')
do while(.true.)
  read(1,*,end=10,err=10) jp, t,t,t, t,t,t, i1,j1,k1
  grid(i1,j1,k1) = .true.
enddo
10 close(1)

mstart = 0
mend = ngrid
msize = ngrid

call autocenter()  ! comment out to show all grid as is

ilast = scan(filein,' ')-1


m(3) = mstart(3)
do while(m(3).ne.mend(3))
m(2)=mstart(2)
do while(m(2).ne.mend(2))
m(1)=mstart(1)
do while(m(1).ne.mend(1))
  if((.not.grid(m(1),m(2),m(3))).and.nneib(m(1),m(2),m(3)).gt.16) grid(m(1),m(2),m(3))=.true.
  call next(1)
enddo
call next(2)
enddo
call next(3)
enddo

open(2,file=filein(1:ilast)//'.fix')
do i=1,ngrid
do j=1,ngrid
do k=1,ngrid
  if(grid(i,j,k)) write(2,*) 0,0,0,0,0,0,0, i,j,k
enddo
enddo
enddo
close(2)


contains

function nneib(i,j,k)
  nneib=0
  do ii=i-1,i+1
  do jj=j-1,j+1
  do kk=k-1,k+1
    if(grid(ii,jj,kk)) nneib = nneib+1
  enddo
  enddo
  enddo
end function nneib


subroutine next(k)
m(k)=m(k)+1
if(m(k).gt.ngrid) m(k)=1
end subroutine next

subroutine autocenter()

integer :: mask1d(ngrid)
nline=ngrid
 
 do k=1,3
   mask1d=0
   do ik = 1,nline
     if(check1d(k,ik)) mask1d(ik)=1
   enddo
   m0=0
   m0addr=0
   m0end=0
   m0curr=0
   m0curraddr=1
   m0currend=1
   do iter = 1,2
   do ik = 1,nline
     if(mask1d(ik).eq.0.and.ik.ne.m0currend+1) then
       m0currend=ik-1
       m0curraddr=ik
       m0curr=0
     endif
     if(mask1d(ik).eq.0.and.ik.eq.m0currend+1) then
       m0currend=m0currend+1
       m0curr=m0curr+1
     endif
     if(mask1d(ik).eq.1.and.ik.eq.m0currend+1.and.m0curr.gt.m0) then
       m0=m0curr
       m0addr=m0curraddr
       m0end=m0currend
     endif
   enddo
   if(m0currend.eq.nline) m0currend=0
   enddo
   mstart(k) = m0end-1
   mend(k) = m0addr+1
   if(m0end.gt.m0addr) m0addr=m0addr+nline
   msize(k) = (m0addr-m0end)
   
 enddo

end subroutine autocenter


logical function check1d(kdim,ii)
  check1d = .false.
  nline = ngrid
    do i1=1,nline
    do i2=1,nline
      if(kdim.eq.1.and.grid(ii,i1,i2)) then
        check1d=.true.
        goto 500
      endif
      if(kdim.eq.2.and.grid(i1,ii,i2)) then
        check1d=.true.
        goto 500
      endif
      if(kdim.eq.3.and.grid(i2,i1,ii)) then
        check1d=.true.
        goto 500
      endif
    enddo
    enddo
 500 continue
end function check1d



end program larefix
