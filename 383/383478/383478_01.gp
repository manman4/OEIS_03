M=23;

\\ a(n) = [x^n] 1/(1 - x - x^2 - x^3)^(n+1).
a(n) = polcoef(1/(1 - x - x^2 - x^3 + x*O(x^n))^(n+1), n);
for(n=0, M, print1(a(n), ", "))



