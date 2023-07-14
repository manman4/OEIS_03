a(n) = sumdiv(n, d, 2^(n/d-1)*eulerphi(2*d)); 
for(n=1, 35, print1(a(n),", ")) 

\\ A000013
for(n=1, 35, print1(a(n)/n,", ")) 