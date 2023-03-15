M=17;

a(n) = v=[1,-1,0]; if(n<3, v[n+1], binomial(n-1,2) * a(n-3) - sum(k=1, n, binomial(n-1,k-1) * a(n-k)));
for(n=0, M, print1(a(n), ", "));
