\\ a(n) = (1/n) * (binomial(2*n-2,n-1) + 2 * Sum_{k=2..n} k * binomial(2*n-1-k,n-1) * A059231(k-1)) for n > 0. 
{a059231(n) = if( n<0, 0, polcoeff( (1 + 3*x - sqrt(1 - 10*x + 9*x^2 + x^2 * O(x^n))) / (8*x), n))}; 
a(n) = (1/n) * (binomial(2*n-2,n-1) + 2 * sum(k=2, n, k * binomial(2*n-1-k,n-1) * a059231(k-1)));
for(n=1, 20, print1(a(n),", "));