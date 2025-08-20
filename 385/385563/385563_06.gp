\\ a(n) = binomial(n+2,2) * A002212(n+1).
a002212(n) = polcoeff( (1 - x - sqrt(1 - 6*x + 5*x^2 + x^2 * O(x^n))) / 2, n+1);
a(n) = binomial(n+2, 2) * a002212(n+1);
for(n=0, 28, print1(a(n),", "));
    