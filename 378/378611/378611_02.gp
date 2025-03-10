\\ a(n) = [x^n] 1/(1 - x/(1 - x))^(2*n).
a(n) = polcoef( 1/(1 - x/(1 - x) + x*O(x^n))^(2*n), n);
for(n=0, 20, print1(a(n), ", "));

\\ a(n) = (1/2)^n * [x^(2*n)] 2/(1 - x/(1 - x))^n for n > 0.
b(n) = polcoef( 2/(1 - x/(1 - x) + x*O(x^(2*n)))^n, 2*n)/2^n;
for(n=1, 20, print1(a(n)-b(n), ", "));