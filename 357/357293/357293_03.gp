M=12;

Bell_poly(n, x) = exp(-x)*suminf(k=0, k^n*x^k/k!);
T(n, k) = if(k==0, 0^n, my(w=exp(2*Pi*I/k)); round(sum(j=0, k-1, Bell_poly(n, w^j)))/k);
for(n=0, M, for(k=0, n, print1(T(k, n-k), ", ")));