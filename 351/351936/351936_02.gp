M=26;

a(n) = if(n<6, 1, sum(k=0, (n-1)\5, (5*k+1)!/5!^k*binomial(n-1, 5*k)*a(n-1-5*k)));
for(n=0, M, print1(a(n), ", "));