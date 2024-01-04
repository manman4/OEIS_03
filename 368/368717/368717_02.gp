\\ a(n) = n! * Sum_{k=0..n} (-1)^(n-k) * k^4 / k!.
a(n) = n! * sum(k=0, n, (-1)^(n-k) * k^4 / k!);
for(n=0, 21, print1(a(n), ", "));  
