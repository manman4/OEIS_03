a(n) = sum(j=1, n, sum(k=1, n, eulerphi(n*j*k)))/eulerphi(n);
for(n=1, 40, print1(a(n),", ")) 