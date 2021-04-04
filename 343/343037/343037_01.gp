M=14;

T(n, k) = my(m=binomial(n, k)); if(issquare(m), 0, (sqrtint(m)+1)^2-m);
for(n=2, M, for(k=1 , n-1, print1(T(n, k), ", ")))