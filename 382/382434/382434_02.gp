M=21;

\\ a(n) = Sum_{k=0..n} binomial(n,k) * ( binomial(n,k) - binomial(n,k-1) )^3. 
a129123(n) = sum(k=0, n, binomial(n, k) * (binomial(n, k) - binomial(n, k-1))^3);
for(n=0, M, print1(a129123(n), ", "))

\\ a(n) = 2 * A129123(n) - 1.
a(n) = 2 * a129123(n) - 1;
for(n=0, M, print1(a(n), ", "))

