program zoom
integer, parameter:: Nhalo_max=10000
real:: hpos(3,Nhalo_max), hR(Nhalo_max)
 character (len=600):: f_halos, f_low_snap, f_out
 character(len=4):: secname
real, allocatable:: pos(:,:), vel(:,:)
integer, allocatable:: id(:), mask(:,:,:), mask1d(:)
integer:: Ntot, thr, gformat
real:: Box, pmass, tsnap, move(3), msize(3)

factor_ic=3.0
!dx=0

open(1,file='LareWrite.tbl')
read(1,'(A)')f_halos
read(1,'(A)')f_low_snap
read(1,'(A)')f_out
read(1,*) thr
read(1,*) factor_ic
close(1)

open(2,file=trim(f_halos),status='old')
read(2,*)
nh=1
do while(.true.)
  read(2,*,end=10,err=10) a1,hostid,a3,a4,a5,(hpos(k,nh),k=1,3), a9,a10,a11, hR(nh)
  nh=nh+1
!  if(hostid.ge.0) nh=nh-1      ! we don't want subhalos here
enddo
10 close(2)
nh=nh-1
print*,'Halos read: ',nh

 hpos = hpos
 hR = hR

 call read_header(f_low_snap)
 pmass_low=pmass
 allocate(pos(3,Ntot))
 allocate(vel(3,Ntot))
 allocate(id(Ntot))
 if(gformat.eq.2) read(22) secname
 read(22) pos
 if(gformat.eq.2) read(22) secname
 read(22) vel
 if(gformat.eq.2) read(22) secname
 read(22) id
 close(22)
 
 nline=int(float(Ntot)**0.33333333333+0.5)
 n2=nline*nline
 allocate(mask(nline,nline,nline))
 mask=0

 hR=hR*factor_ic
 vel=vel*sqrt(tsnap)
 
 open(17,file=trim(f_out))
 
 nread=0
  do ih=1,nh
    do ip=1,Ntot
      if(inside(pos(1,ip),hpos(1,ih),hr(ih))) then
        nread=nread+1
        jp=id(ip)
        k1 = int((jp-1)/n2)+1
        jj = jp - (k1-1)*n2
        j1 = int((jj-1)/nline)+1
        i1 = jj - (j1-1)*nline
	mask(i1,j1,k1)=1
!        write (17,250) jp,pos(1:3,ip),vel(1:3,ip),i1,j1,k1
      endif
    enddo
  enddo
 
!mask = mask*1000

!do it=1,dx
!  mask(1:nline-1,1:nline,1:nline)=mask(1:nline-1,1:nline,1:nline)+mask(2:nline,1:nline,1:nline)/1000
!  mask(nline,1:nline,1:nline)=mask(nline,1:nline,1:nline)+mask(1,1:nline,1:nline)/1000

!  mask(1:nline,1:nline-1,1:nline)=mask(1:nline,1:nline-1,1:nline)+mask(1:nline,2:nline,1:nline)/1000
!  mask(1:nline,nline,1:nline)=mask(1:nline,nline,1:nline)+mask(1:nline,1,1:nline)/1000

!  mask(1:nline,1:nline,1:nline-1)=mask(1:nline,1:nline,1:nline-1)+mask(1:nline,1:nline,2:nline)/1000
!  mask(1:nline,1:nline,nline)=mask(1:nline,1:nline,nline)+mask(1:nline,1:nline,1)/1000
!enddo


  do i=1,nline
    do j=1,nline
      do k=1,nline
		nneib = 0
		if (mask(i,j,k).eq.0) then
	        do i1 = -1,1
	        do j1 = -1,1
	        do k1 = -1,1
				msk = mask(plus(i,i1),plus(j,j1),plus(k,k1))
				if (msk.eq.1) then
					nneib = nneib + 1
				endif
			enddo
			enddo
			enddo
			if(nneib.ge.thr) mask(i,j,k) = -1
		endif
      enddo
    enddo
  enddo
  mask = abs(mask)

  nwrite=0
    do ip=1,Ntot
      jp=id(ip)
      k1 = int((jp-1)/n2)+1
      jj = jp - (k1-1)*n2
      j1 = int((jj-1)/nline)+1
      i1 = jj - (j1-1)*nline
      if(mask(i1,j1,k1).ge.1) then
        nwrite=nwrite+1
        write (17,250) jp,pos(1:3,ip)/1e3,vel(1:3,ip),i1,j1,k1
      endif
    enddo



 250    Format(i9,3F9.4,3F9.2,3i5,3i9)

 print*, nread, ' particles read'
 print*, nwrite, ' particles written'
 
 close(17)
 
 
 allocate(mask1d(nline))
 
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
   if(m0end.gt.m0addr) m0addr=m0addr+nline
   move(k) = (m0addr+m0end)*Box/2/nline
   msize(k) = (m0addr-m0end)*Box/nline
   if(move(k).gt.Box) move(k)=move(k)-Box
!   print*,m0,m0addr,m0end
 enddo
 print*,'Move center:'
 print*, move
 print*
 print*,'Region size:'
 print*, msize
 
 
 
 contains

integer function plus(i,ip)
	plus = i+ip
	if(plus.lt.1) plus = plus+nline
	if(plus.gt.nline) plus = plus-nline
end function plus

logical function check1d(kdim,i)
  check1d = .false.
    do i1=1,nline
    do i2=1,nline
      if(kdim.eq.1.and.mask(i,i1,i2).ge.1) then
        check1d=.true.
        goto 500
      endif
      if(kdim.eq.2.and.mask(i1,i,i2).ge.1) then
        check1d=.true.
        goto 500
      endif
      if(kdim.eq.3.and.mask(i2,i1,i).ge.1) then
        check1d=.true.
        goto 500
      endif
    enddo
    enddo
 500 continue
end function check1d

logical function inside(x,b,r)
  real:: x(3),b(3),d(3)
  inside=.true.
  d = min(abs(x-b),Box-abs(x-b))
  dd=sum(d*d)
  inside = dd.lt.r*r
end function inside

subroutine read_header(fname)
integer:: npart(6), bytes0(9), bytesleft((256-6*4-8*8-48)/4)
real*8:: massarr(6), time, redshift, boxsize
 character(len=300):: fname
character(len=4):: secname

open(22,file=trim(fname),form='unformatted',status='old')

gformat=1
read(22,err=20) secname
if(secname(1:1).eq.'H')gformat=2
20 continue
if(gformat.eq.1) rewind(22)

read(22) npart, massarr, time, redshift, bytes0, numfiles, boxsize, bytesleft

Ntot=npart(2)
Box=boxsize
tsnap = time
pmass=massarr(2)
end subroutine read_header


end program zoom

