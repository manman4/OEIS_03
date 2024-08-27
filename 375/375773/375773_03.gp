M=21;

\\ a(n) = Sum_{k=0..floor(n/5)} (5*k)! * Stirling2(n,5*k)/k!.
a(n) = sum(k=0, n\5, (5*k)!*stirling(n, 5*k, 2)/k!);
for(n=0, M, print1(a(n), ", "));
