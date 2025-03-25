M=21;

\\ a(n) = S(6,n), where S(r,n) = Sum_{k=0..floor(n/2)} ( binomial(n,k) - binomial(n,k-1) )^r.
\\ a(n) = Sum_{k=0..floor(n/2)} A008315(n,k)^6.
\\ a(n) = Sum_{k=0..n} A120730(n,k)^6.
\\ a(n) = A357824(n,6).
s(n, r) = sum(k=0, n\2, (binomial(n,k) - binomial(n,k-1))^r);
a(n) = s(n, 6);
for(n=0, M, print1(a(n), ", "))
