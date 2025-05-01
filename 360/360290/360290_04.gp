M=20;

a(n) = sum(k=0, n\2, binomial(n-k, k)*binomial(2*(n-2*k), n-2*k));
\\ a(n) = A383573(n) - A383573(n-2). 
for(n=0, M, print1(a(n) - a(n-2), ", "));

