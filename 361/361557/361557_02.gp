a002657(n) = n! * sum(k=1, n, 1/k!);
a(n) = if(n==0, 1,  sum(k=1, n, a002657(k)*binomial(n-1,k-1)*a(n-k)));
for(n=0, 15, print1(a(n),", "))  