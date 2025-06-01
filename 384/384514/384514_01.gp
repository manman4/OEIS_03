M=18;

\\ a(n) = (-6)^(n+1)/7 * Li_{-n}(7), where Li_{n}(x) is the polylogarithm function.
a(n) = (-6)^(n+1)/7 * polylog(-n, 7);
for(n= 0, M, print1(a(n), ", "));