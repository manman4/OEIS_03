a(n) = n*moebius(n)-if(n%2, 0, n/2*moebius(n/2));

for(n=1, 50, print1(a(n),", ")) 
