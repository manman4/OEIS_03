\\ a(n) = Sum_{d|n} A000009(d-1).
a000009(n) = polcoef(prod(k=1, n, 1 + x^k, 1 + x * O(x^n)), n);
a(n) = sumdiv(n, d, a000009(d-1));
for(n=1, 55, print1(a(n), ", "));