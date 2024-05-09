a(n) = sum(i=1, n, sum(j=1, n, sum(k=1, n, eulerphi(i*j*k)/eulerphi(k))));
for(n=1, 35, print1(a(n),", ")) 