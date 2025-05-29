M=9;

\\ A(n,k) is the n-th q-Catalan number for q=k.
a(n, k) = if(k==1, binomial(2*n, n)/(n+1), (1-k)/(1-k^(n+1))*prod(j=0, n-1, (1-k^(2*n-j))/(1-k^(j+1))));
for(n=0, M, for(k=0, n, print1(a(k,n-k),", ")));


 

