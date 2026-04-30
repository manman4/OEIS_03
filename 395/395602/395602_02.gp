\\ A(n,k) = [x^(n*k-2)] (1-x) * (1+x)^(n*k-4) * (Sum_{j=0..k} x^j)^n.
a(n,k) = polcoef((1-x) * (1+x)^(n*k-4) * sum(j=0, k, x^j)^n, n*k-2);
for(n=2, 12, for(k=2,n-1,  print1(a(k,n-k),", "))); 

