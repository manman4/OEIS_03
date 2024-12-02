\\ a(n) = [x^n] 1/(1 - x/(1 - x))^(3*n).
a(n) = polcoef( 1/(1 - x/(1 - x) + x*O(x^n))^(3*n), n);
for(n=0, 20, print1(a(n), ", "));

\\ a(n) = (1/4)^n * [x^(3*n)] 3/(1 - x/(1 - x))^n for n > 0.
b(n) = polcoef( 3/(1 - x/(1 - x) + x*O(x^(3*n)))^n, 3*n)/4^n;
for(n=1, 20, print1(a(n)-b(n), ", "));