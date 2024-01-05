\\ a(n) = Sum_{k=0..n-1} (-1)^k*n!/k!.
a000240(n) = n! * sum(k=0, n-1, (-1)^k/k!);
for(n=1, 21, print1(a000240(n), ", "));
a(n) = n! - a000240(n);
for(n=0, 23, print1(a(n), ", "));
