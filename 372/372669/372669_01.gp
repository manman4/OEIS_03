a(n) = sum(j=1, n, sum(k=1, n, eulerphi(n*j*k)/eulerphi(n*k)));
for(n=1, 44, print1(a(n),", ")) 