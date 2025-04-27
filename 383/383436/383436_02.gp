\\ a(1) = 1; a(n) = 2 + n * Sum_{k=1..n-1} a(k).
a(n) = if(n==1, 1, 2 + n * sum(k=1, n-1, a(k)));
for(n=1, 20, print1(a(n),", "))          
