M=21;

\\ a(n) = n! * Sum_{k=0..floor(n/3)} 1/k!.
a(n) = n! * sum(k=0, n\3, 1/k!);
for(n=0, M, print1(a(n), ", "))
