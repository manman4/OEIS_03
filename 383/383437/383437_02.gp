M=21;

\\ a(1) = 1; a(n) = 5 + n * Sum_{k=1..n-1} a(k).
a(n) = if(n==1, 1, 5 + n * sum(k=1, n-1, a(k)));
for(n=1, M, print1(a(n),", "))          
