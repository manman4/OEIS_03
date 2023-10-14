M=11;

T(n, k) = if(n==0, 0, sumdiv(n, d, eulerphi(n/d)*k^(d-1)));
for(n=0, M, for(k=0, n, print1(T(k, n-k),", ")));