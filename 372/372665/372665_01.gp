a(n) = sum(j=1, n, sum(k=1, n, eulerphi(2*j*k)/eulerphi(j*k)));
for(n=1, 50, print1(a(n),", ")) 