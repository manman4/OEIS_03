\\ a(0) = 1; a(n) = 2 * Sum_{k=0..n-1} (3-k/n) * a(k).
a(n) = if(n==0, 1, 2 * sum(k=0, n-1, (3-k/n) * a(k)) );
for(n=0, 20, print1(a(n),", "))   