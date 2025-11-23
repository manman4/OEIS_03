M=13;

\\ a(1) = 1; a(n) = -3*a(n-1) + 6*Sum_{k=1..n-1} binomial(n,k) * a(k) * a(n-k).
a(n) = if(n==1, 1, -3*a(n-1) + 6*sum(k=1, n-1, binomial(n,k) * a(k) * a(n-k)) );
for(n=1, M, print1(a(n),", "));

