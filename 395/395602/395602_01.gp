\\ A(n,k) = Sum_{i=0..n*k-2} binomial(n*k-4,i) * Sum_{j=0..floor((n*k-i-2)/(k+1))} (-1)^j * binomial(n,j) * binomial(n*(k+1)-i-4-(k+1)*j,n-2).
a(n, k) = sum(i=0, n*k-2, binomial(n*k-4, i) * sum(j=0, (n*k-i-2)\(k+1), (-1)^j * binomial(n, j) * binomial(n*(k+1)-i-4-(k+1)*j, n-2)));
for(n=2, 12, for(k=2,n-1,  print1(a(k,n-k),", "))); 

