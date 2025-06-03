M=18;

\\ a(n) = (-2)^(n+1)/5 * Li_{-n}(5/3), where Li_{n}(x) is the polylogarithm function.
a(n) = (-2)^(n+1)*polylog(-n, 5/3)/5;
for(n= 0, M, print1(a(n), ", "));