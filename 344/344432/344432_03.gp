M=36;

a(n) = if(n==0, 0, 2*a(n-1)+moebius(n));
for(n=0, M, print1(a(n), ", "));