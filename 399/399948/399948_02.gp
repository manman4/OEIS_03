\\ a(n) = (-1)^n * (A002104(n) - A002741(n))/2. 
a002104(n) = sum(k=0, n-1,          binomial(n, k) * (n-k-1)!);
a002741(n) = sum(k=0, n-1, (-1)^k * binomial(n, k) * (n - k - 1)!);
a177699(n) = (-1)^n * (a002104(n) - a002741(n))/2;
\\ a(0) = 1; a(n) = Sum_{k=1..n} |A177699(k)| * binomial(n,k) * a(n-k).
a(n) = if(n==0, 1, sum(k=1, n, abs(a177699(k)) * binomial(n, k) * a(n-k)));
for(n=0, 18, print1(a(n), ", "));