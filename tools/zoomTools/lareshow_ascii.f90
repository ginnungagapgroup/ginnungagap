program lareshow
character(len=800) :: filein, tmpngrid, tout
character(len=6) :: tmpnum
logical, allocatable :: grid(:,:,:)
integer :: mstart(3), mend(3), msize(3), m(3)
integer, allocatable :: pix(:,:)


if(iargc().lt.2) then
  print*,'Usage: lareshow_ascii lare-file ngrid'
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

npx = msize(1)+2
npy = msize(2)+2

allocate(pix(npx,npy))

m=mstart
ilast = scan(filein,' ')-1
open(2,file=filein(1:ilast)//'.txt')

do while(m(3).ne.mend(3))

pix=0
j=1
m(2)=mstart(2)
do while(m(2).ne.mend(2))
i=1
m(1)=mstart(1)
do while(m(1).ne.mend(1))
  !print*, m
  if(grid(m(1),m(2),m(3))) pix(i,j)=1
  i=i+1
  call next(1)
enddo
j=j+1
call next(2)
enddo

do j=1,npy
tout=''
do i=1,npx
  if(pix(i,j).eq.1) tout(i:)='#'
enddo
write(2,'(A)') tout(1:npx)
enddo

do i=1,npx
 tout(i:)='-'
enddo
write(2,*)
write(2,'(A)') tout(1:npx)
write(2,*)
call next(3)
enddo

close(2)


contains

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



end program lareshow
