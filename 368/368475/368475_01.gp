\\ Sum_{k=0..floor(n/4)} binomial(k,n-4*k)
a017827(n) = sum(k=0, n\4, binomial(k, n-4*k));
for(n=0, 50, print1(a017827(n), ", "));
\\ a(n) = A017827(5*n-4) for n>0.
a(n) = if(n==0, 1, a017827(5*n-4));
for(n=0, 34, print1(a(n), ", "));