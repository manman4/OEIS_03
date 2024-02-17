\\ a(0) = 1; a(n) = Sum_{k=1..n} (2*k)!/2 * a(n-k).
a(n) = if(n==0, 1, sum(k=1, n, (2*k)!/2*a(n-k)));
for(n=0, 14, print1(a(n), ", "));

