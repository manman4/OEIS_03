M=24;

a(n) = if(n<4, 1, sum(k=0, (n-1)\3, (3*k+1)!/6^k*binomial(n-1, 3*k)*a(n-1-3*k)));
for(n=0, M, print1(a(n), ", "));