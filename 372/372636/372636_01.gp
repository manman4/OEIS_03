a(n) = sum(j=1, n, sum(k=1, n, eulerphi(j*k)/eulerphi(k)));
for(n=1, 44, print1(a(n),", ")) 