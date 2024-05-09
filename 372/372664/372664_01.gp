a(n) = sum(j=1, n, sum(k=1, n, eulerphi(2*j*k)/eulerphi(k)));
for(n=1, 43, print1(a(n),", ")) 