M=18;

\\ a(n) = (-3)^(n+1)/5 * Li_{-n}(5/2), where Li_{n}(x) is the polylogarithm function.
a(n) = (-3)^(n+1)/5*polylog(-n, 5/2);
for(n= 0, M, print1(a(n), ", "));