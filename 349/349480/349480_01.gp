M=13;

a(n) = sum(j=0, n, (-1)^(n-j)*prod(k=(j-1)*n+1, j*n, k));
for(n=0, M, print1(a(n), ", "));