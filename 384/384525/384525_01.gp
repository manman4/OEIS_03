M=18;

\\ a(n) = (-5)^(n+1)/6 * Li_{-n}(6), where Li_{n}(x) is the polylogarithm function.
a(n) = (-5)^(n+1)*polylog(-n, 6)/6;
for(n= 0, M, print1(a(n), ", "));