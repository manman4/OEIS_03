M=25;

a(n) = if(n<5, 1, sum(k=0, (n-1)\4, (4*k+1)!/24^k*binomial(n-1, 4*k)*a(n-1-4*k)));
for(n=0, M, print1(a(n), ", "));