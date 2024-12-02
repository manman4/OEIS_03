\\ a(n) = [x^n] 1/(1 - x/(1 - x))^(4*n).
a(n) = polcoef( 1/(1 - x/(1 - x) + x*O(x^n))^(4*n), n);
for(n=0, 20, print1(a(n), ", "));

\\ a(n) = (1/8)^n * [x^(4*n)] 4/(1 - x/(1 - x))^n for n > 0.
b(n) = polcoef( 4/(1 - x/(1 - x) + x*O(x^(4*n)))^n, 4*n)/8^n;
for(n=1, 20, print1(a(n)-b(n), ", "));