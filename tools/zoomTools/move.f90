program move
 
 character(len=280):: fname
 real:: box, center(3)
 integer:: Ntot, numfiles, Nmass
 
 open(1,file='move.tbl',status='old')
 read(1,*) center
 tsnap = 0
 numfiles=1
 
 do while(.true.)
   nfile=0
   do while(numfiles.gt.nfile)
     read(1,'(a)',end=10) fname
     call read_header()
     call repos()
     nfile=nfile+1
   enddo
 enddo
 10 close(1)
 close(11)
 
 
   contains


subroutine read_header()
integer:: npart(6), bytes0(9), bytesleft((256-6*4-8*8-48)/4)
real*8:: massarr(6), time, redshift, boxsize

open(2,file=fname,form='unformatted',status='old')
read(2) npart, massarr, time, redshift, bytes0, numfiles, boxsize, bytesleft
print*,'Redshift: ', redshift

Ntot=sum(npart(2:6))
Box=boxsize
tsnap = time
Nmass=0
do i=1,6
  if(npart(i).gt.0.and.massarr(i).eq.0) Nmass = Nmass + npart(i)
enddo

print*, massarr

open(3,file='moved'//trim(fname),form='unformatted')
write(3) npart, massarr, time, redshift, bytes0, numfiles, boxsize, bytesleft

end subroutine read_header
   
subroutine repos()
real		   :: pos(3,Ntot), vel(3,Ntot), mass(Nmass)
integer		   :: id(Ntot)
logical		   :: zfit

read(2) pos
read(2) vel
read(2) id
if(Nmass.gt.0) read(2) mass
 close(2)
print*, center
do k=1,3
 do i=1,Ntot
  pos(k,i)=pos(k,i)-center(k)+Box/2
  if(pos(k,i).ge.Box) pos(k,i)=pos(k,i)-Box
  if(pos(k,i).lt.0) pos(k,i)=pos(k,i)+Box
 enddo
enddo

write(3) pos
write(3) vel
write(3) id
if(Nmass.gt.0) write(3) mass
 close(3)
end subroutine repos

end program move