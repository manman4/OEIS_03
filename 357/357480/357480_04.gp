M=23;

a(n) = if(n==0, 0, n * a(n-1) + binomial(n,4));
for(n=0, M, print1(a(n), ", "));