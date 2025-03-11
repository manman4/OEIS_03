M=21;

\\ a(n) = Sum_{k=0..n} binomial(4*k+1,k) * binomial(2*k+2,n-k)/(4*k+1).
a(n) = sum(k=0, n, binomial(4*k+1, k) * binomial(2*k+2, n-k) / (4*k+1));
for(n=0, M, print1(a(n), ", "));

\\ a(n) = A381785(n) + A381785(n-1).
a381785(n) = sum(k=0, n, binomial(4*k+1, k)*binomial(2*k+1, n-k)/(4*k+1));
b(n) = a381785(n) + a381785(n-1);
for(n=0, 100, print1(a(n)-b(n), ", "));


 