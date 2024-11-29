a005809(n) = binomial(3*n, n);
\\ a(n) = Sum_{k=0..n} binomial(3k,k)*binomial(3n-3k,n-k).
a006256(n) = sum(k=0, n, binomial(3*k, k)*binomial(3*(n-k), n-k));

\\ 4*a(n) - 27*a(n-1) = 3*A006256(n) + A005809(n) for n > 0.
a(n) = if(n==0, 1, (3*a006256(n) + a005809(n) + 27*a(n-1))/4);
for(n=0, 20, print1(a(n), ", "));