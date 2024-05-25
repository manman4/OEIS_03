a(n) = sum(i=1, n, sum(j=1, n, sum(k=1, n, sum(l=1, n, sum(m=1, n, ( n/gcd([i, j, k, l, m, n]) )^3 )))));   
for(n=1, 20, print1(a(n), ", "))

\\ a(n) = Sum_{1 <= x_1, x_2, x_3, x_4, x_5 <= n} ( gcd(x_1, x_2, n)/gcd(x_1, x_2, x_3, x_4, x_5, n) )^5. 
b(n) = sum(i=1, n, sum(j=1, n, sum(k=1, n, sum(l=1, n, sum(m=1, n, ( gcd([i, j, n])/gcd([i, j, k, l, m, n]) )^5 )))));
for(n=1, 20, print1(a(n)-b(n), ", ")) 

