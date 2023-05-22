M=55;

a(n) = sum(k=1, sqrtint(n), (-1)^(k-1)*numbpart(n-k*(3*k+5)/2));
for(n=1, M, print1(a(n), ", "))