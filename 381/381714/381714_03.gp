M=17;

\\ a(0) = 1; a(n) = (1/n) * Sum_{k=1..n} sigma_3(k^3) * a(n-k).
a(n) = if(n==0, 1, 1/n*sum(k=1, n, sigma(k^3, 3)*a(n-k)));
for(n=0, M, print1(a(n),", "));
 