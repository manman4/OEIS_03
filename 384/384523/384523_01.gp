M=17;

\\ a(n) = (-3)^(n+1)/7 * Li_{-n}(7/4), where Li_{n}(x) is the polylogarithm function.
a(n) = (-3)^(n+1)/7 * polylog(-n, 7/4);
for(n= 0, M, print1(a(n), ", "));