a(n) = if(n==1, 1, -sum(k=2, n, k * a(n\k)));           
for(n=1, 66, print1(a(n),", "))
