a(n) = if(n==1, 1, sum(k=2, n, (-1)^k * k * a(n\k)));           
for(n=1, 58, print1(a(n),", "))
