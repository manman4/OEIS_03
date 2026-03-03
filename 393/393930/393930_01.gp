\\ a(0) = 0, a(1) = 1; a(n) = n! * [x^n] (1/n)^2 * exp(n*x) * Sum_{k=1..n-1} a(k)*x^k/k!.
a(n) = if(n<2, n, n!*polcoef(exp(n*x+O(x^(n+5))) * sum(k=1, n-1, a(k)*x^k/k!), n)/n^2);                                                     
for(n=0, 19, print1(a(n),", ")); 