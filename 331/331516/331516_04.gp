\\ a(n) = binomial(n+2,2) * A059231(n+1).
a059231(n) = if( n<0, 0, polcoeff( (1 + 3*x - sqrt(1 - 10*x + 9*x^2 + x^2 * O(x^n))) / (8*x), n));
a(n) = binomial(n+2, 2) * a059231(n+1);
for(n=0, 25, print1(a(n),", "))  
