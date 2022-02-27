M=22;

a(n) = if(n<3, 1, sum(k=0, (n-1)\2, (2*k+1)!/2^k*binomial(n-1, 2*k)*a(n-1-2*k)));
for(n=0, M, print1(a(n), ", "));